#pragma once

#include "detail.h"
#include "interfaces.h"
#include <windows.h>

#include <vector>
#include <map>

namespace Kernel
{
	namespace Internal
	{
		class Thread
		{
			CRITICAL_SECTION CS;
			HANDLE events[2];

			int poolSize;
			Message *pool;
			int forPush, forPop;

			Message PopMessage();
			int WaitForEvents();

			static DWORD WINAPI MainFunc(void* pThread);

		public:
			std::vector<ITask*> Tasks;
			std::map<TypeID, IHandlerProxy*> RoutingTable;

			Thread();
			Thread(const Thread &inst);
			~Thread();


			void InitTasks();

			HANDLE Start();

			void PushMessage(TypeID message_type, void *message);
			void Terminate();

		};
	}
}

