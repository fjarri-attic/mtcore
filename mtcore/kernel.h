#pragma once

#include "detail.h"
#include "interfaces.h"
#include <Windows.h>

#include <string>

namespace Kernel
{
	class WindowParameters
	{
	public:
		std::string ClassName;
		std::string WindowName;
        
		bool FullScreen;
		int Xpos;
		int Ypos;
		int Width;
        int Height;

		WNDPROC WndProc;
	};

	namespace Internal
	{
		void AddTask(ITask *task, TypeID task_type, int thread_num);
		void RegisterHandler(TypeID task_type, TypeID message_type, IHandlerProxy *handler_proxy);
		void SendMessage(TypeID message_type, void *message);
	}

	bool AlreadyRunning(const char *class_name, bool switch_to_existing);
	void SetWindowParams(const WindowParameters &params);
	HWND MayCreateWindow();
	void ShowWindow();
	HWND RestartWindow();

	void StartMessageQueue();

	void Init(int threads_num);
	void StartCycle();
	void Release();
	void Terminate();

	void SetStartingParameters(HINSTANCE hInstance, LPSTR lpCmdLine, int nShowCmd);

	template <class TASK>
	void AddTask(int thread_num);

	template <class TASK, class MESSAGE>
	void RegisterHandler();

	template<class MESSAGE>
	void SendMessage(const MESSAGE &message);
}

// Implementation
namespace Kernel
{
	template <class TASK>
	void AddTask(int thread_num)
	{
		Internal::AddTask(new TASK, Internal::GetTypeID<TASK>(), thread_num); //
	}

	template <class TASK, class MESSAGE>
	void RegisterHandler()
	{
		Internal::RegisterHandler(Internal::GetTypeID<TASK>(), Internal::GetTypeID<MESSAGE>(), 
			new Internal::HandlerProxy<TASK, MESSAGE>(&TASK::Handler)); //
	}

	template<class MESSAGE>
	static void SendMessage(const MESSAGE &message)
	{
		Internal::SendMessage(Internal::GetTypeID<MESSAGE>(), new MESSAGE(message));//
	}
}