#include <map>
#include <vector>
#include <fstream>

#include "thread.h"
#include "kernel.h"

namespace Kernel
{
	void MayCreateWindow(const WindowParameters &params);

	namespace Internal
	{
		class KernelData
		{
		public:
			std::vector<Thread> Threads;
			std::vector<HANDLE> ThreadHandles;

			std::map<TypeID, ITask*> TaskType2TaskPtr;
			std::map<TypeID, int> TaskType2ThreadNum;
			std::map<TypeID, int> RoutingTable;

			HINSTANCE hInstance;
			LPSTR lpCmdLine;
			int nShowCmd;
			HWND hWnd;

			WindowParameters Window;

			HANDLE StartMessageQueue;
			HANDLE CreateWindow;
			HANDLE WindowCreated;

			DWORD MainThread;

			bool NeedRestart;
		};
	}

	static Internal::KernelData *kernelData;

	void Init(int threads_num)
	{
		kernelData = new Internal::KernelData;

		kernelData->Threads.resize(threads_num);		
		kernelData->ThreadHandles.resize(threads_num);
		
		kernelData->StartMessageQueue = CreateEvent(NULL, FALSE, FALSE, NULL);
		kernelData->CreateWindow = CreateEvent(NULL, FALSE, FALSE, NULL);
		kernelData->WindowCreated = CreateEvent(NULL, FALSE, FALSE, NULL);

		kernelData->MainThread = GetCurrentThreadId();

		kernelData->hWnd = 0;
		kernelData->NeedRestart = true;
	}

	void Release()
	{
		CloseHandle(kernelData->StartMessageQueue);
		CloseHandle(kernelData->CreateWindow);
		CloseHandle(kernelData->WindowCreated);

		delete kernelData;
	}

	void StartCycle()
	{
		std::size_t i;
		MSG msg;

		kernelData->TaskType2TaskPtr.clear();
		kernelData->TaskType2ThreadNum.clear();

		for(i = 0; i < kernelData->Threads.size(); i++)
			kernelData->Threads[i].InitTasks();

		for(i = 0; i < kernelData->ThreadHandles.size(); i++)
			kernelData->ThreadHandles[i] = kernelData->Threads[i].Start();

		while(kernelData->NeedRestart)
		{
			kernelData->NeedRestart = false;

			if(!kernelData->hWnd)
			{
				WaitForSingleObject(kernelData->CreateWindow, INFINITE);
				MayCreateWindow(kernelData->Window);
				SetEvent(kernelData->WindowCreated);
			}

			WaitForSingleObject(kernelData->StartMessageQueue, INFINITE);

			while(GetMessage(&msg, NULL, 0, 0))
			{
				if(msg.message == WM_QUIT)
					break;

				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			}

			DestroyWindow(kernelData->hWnd);
			kernelData->hWnd = 0;
		}
        

		for(i = 0; i < kernelData->Threads.size(); i++)
			kernelData->Threads[i].Terminate();

		WaitForMultipleObjects((DWORD)kernelData->ThreadHandles.size(), &(kernelData->ThreadHandles[0]), TRUE, INFINITE);
	}

	HWND MayCreateWindow()
	{
		DWORD CurrentThread = GetCurrentThreadId();

		if(CurrentThread == kernelData->MainThread)
		{
			MayCreateWindow(kernelData->Window);            
		}
		else
		{
			SetEvent(kernelData->CreateWindow);
			WaitForSingleObject(kernelData->WindowCreated, INFINITE);
		}

		return kernelData->hWnd;
	}

	HWND RestartWindow()
	{
		kernelData->NeedRestart = true;
		PostThreadMessage(kernelData->MainThread, WM_QUIT, 0, 0);

		return MayCreateWindow();
	}

	void Terminate()
	{
		kernelData->NeedRestart = false;
		PostThreadMessage(kernelData->MainThread, WM_QUIT, 0, 0);
	}

    void StartMessageQueue()
	{
		SetEvent(kernelData->StartMessageQueue);
	}

	void SetWindowParams(const WindowParameters &params)
	{
		kernelData->Window = params;
	}

	void Internal::AddTask(ITask *task, TypeID task_type, int thread_num)
	{
		kernelData->TaskType2TaskPtr[task_type] = task;
		kernelData->TaskType2ThreadNum[task_type] = thread_num;
		kernelData->Threads[thread_num].Tasks.push_back(task);
	}

	void Internal::RegisterHandler(TypeID task_type, TypeID message_type, IHandlerProxy *handler_proxy)
	{
		ITask *task = kernelData->TaskType2TaskPtr[task_type];
		int thread_num = kernelData->TaskType2ThreadNum[task_type];

		handler_proxy->task = task;

		kernelData->Threads[thread_num].RoutingTable[message_type] = handler_proxy;
		kernelData->RoutingTable[message_type] = thread_num; 
	}

	void Internal::SendMessage(TypeID message_type, void *message)
	{
		kernelData->Threads[kernelData->RoutingTable[message_type]].PushMessage(message_type, message);
	}

	void SetStartingParameters(HINSTANCE hInstance, LPSTR lpCmdLine, int nShowCmd)
	{
		kernelData->hInstance = hInstance;
		kernelData->lpCmdLine = lpCmdLine;
		kernelData->nShowCmd = nShowCmd;
	}

	HINSTANCE GetHInstance()
	{
		return kernelData->hInstance;
	}

	bool AlreadyRunning(const char *class_name, bool switch_to_existing)
	{
		HWND hWnd;
		
		hWnd = FindWindow(class_name, NULL);

		if(!hWnd)
			return false;

		if(switch_to_existing)
		{
			if(IsIconic(hWnd))
				ShowWindow(hWnd, SW_RESTORE);
			SetForegroundWindow(hWnd);
		}

		return true;
	}

	void MayCreateWindow(const WindowParameters &params)
	{
		WNDCLASSEX wc;

		memset(&wc, 0, sizeof(wc));

		wc.lpszClassName= params.ClassName.c_str();		
		wc.lpfnWndProc	= params.WndProc;	
		wc.style		= CS_CLASSDC;		
		wc.hInstance	= kernelData->hInstance;				
		wc.hIcon		= LoadIcon(NULL,IDI_APPLICATION);
		wc.hCursor		= LoadCursor(NULL,IDC_ARROW);	
		wc.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);	
		wc.lpszMenuName	= NULL;			
		wc.cbClsExtra	= 0;			
		wc.cbWndExtra	= 0;			
		wc.cbSize = sizeof(wc);

		RegisterClassEx(&wc);

		kernelData->hWnd = CreateWindow(params.ClassName.c_str(), params.WindowName.c_str(),
			WS_OVERLAPPEDWINDOW, params.Xpos, params.Ypos, params.Width, params.Height,
			NULL, NULL, kernelData->hInstance, NULL);
	}

	void ShowWindow()
	{
		ShowWindow(kernelData->hWnd, SW_SHOWDEFAULT);
		UpdateWindow(kernelData->hWnd);
	}
}
