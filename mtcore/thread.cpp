#include "thread.h"

#define EVENT_TERMINATE 1
#define EVENT_QUEUE_NOT_EMPTY 0

namespace Kernel
{
	namespace Internal
	{
		DWORD WINAPI Thread::MainFunc(void* pThread)
		{
			Message message;
			Thread &This = *((Thread*)(pThread));

			while(This.WaitForEvents() == EVENT_QUEUE_NOT_EMPTY)
			{
				message = This.PopMessage();

				This.RoutingTable[message.Type]->Execute(message.Data);

				delete message.Data; //
			}

			for(int i = 0; i < (int) This.Tasks.size(); ++i)
			{
				This.Tasks[i]->Terminate();
				delete This.Tasks[i];	//
			}

			return 0;
		}

		Thread::Thread() 
		{ 
			events[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
			events[1] = CreateEvent(NULL, TRUE, FALSE, NULL);
			InitializeCriticalSection(&CS);

			poolSize = 10;
			pool = new Message[poolSize];//
			forPush = 0;
			forPop = 0;
		}

		Thread::Thread(const Thread &inst)
		{
			events[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
			events[1] = CreateEvent(NULL, TRUE, FALSE, NULL);
			InitializeCriticalSection(&CS);

			poolSize = inst.poolSize;
			pool = new Message[poolSize];//
			forPush = 0;
			forPop = 0;      
		}

		Thread::~Thread()
		{
			delete []pool;//

			CloseHandle(events[0]);
			CloseHandle(events[1]);
			DeleteCriticalSection(&CS);
		}

		HANDLE Thread::Start()
		{
			DWORD ThreadID;

			return CreateThread(NULL, 0, MainFunc, (void*)this, 
				STACK_SIZE_PARAM_IS_A_RESERVATION, &ThreadID);		
		}

		void Thread::InitTasks()
		{
			for(int i=0; i < (int)Tasks.size(); i++)
				Tasks[i]->Init();
		}

		int Thread::WaitForEvents()
		{
			int res;

			res = WaitForMultipleObjects(2, events, FALSE, INFINITE);

            return res - WAIT_OBJECT_0;
		}

		Message Thread::PopMessage()
		{
			Message temp;

			InitializeCriticalSection(&CS);

			temp = pool[forPop];
			forPop++;

			if(forPop == poolSize)
				forPop = 0;

			if(forPop == forPush)
				ResetEvent(events[EVENT_QUEUE_NOT_EMPTY]);

			LeaveCriticalSection(&CS);

			return temp;
		}

		void Thread::PushMessage(TypeID message_type, void *message)
		{
			InitializeCriticalSection(&CS);

			pool[forPush].Type = message_type;
			pool[forPush].Data = message;

			forPush++;
			if(forPush == poolSize)
				forPush = 0;

			if(forPush == forPop)
			{
				Message *temp_pool = new Message[poolSize*2];//
				memcpy(temp_pool, pool, forPush * sizeof(Message));
				memcpy(temp_pool + forPop + poolSize, pool + forPop, (poolSize - forPop) * sizeof(Message));

				forPop += poolSize;
				poolSize *= 2;

				delete []pool;//
				pool = temp_pool;
			}

			SetEvent(events[EVENT_QUEUE_NOT_EMPTY]);

			LeaveCriticalSection(&CS);
		}

		void Thread::Terminate()
		{
			SetEvent(events[EVENT_TERMINATE]);
		}
	}
}

