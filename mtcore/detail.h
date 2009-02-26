#pragma once

namespace Kernel
{
	class ITask;
	class IMessage;

	namespace Internal
	{
		typedef int TypeID;
		
		class Message
		{
		public:
			Message() {}
			Message(TypeID type, void *data) : Type(type), Data(data) {}
			TypeID Type;
            void *Data;
		};

		class TypeIDGen
		{
		private:
			static TypeID Counter;
		public:
			TypeID ID;
			TypeIDGen() : ID(Counter++) {}
		};

		template <class TYPE>
		__declspec(noinline) TypeID GetTypeID()
		{
			static TypeIDGen idgen;
			return idgen.ID;
		}

		class IHandlerProxy
		{
		public:
			ITask *task;

			virtual ~IHandlerProxy() {}

			virtual void Execute(void *message) = 0;
		};

		template <class TASK, class MESSAGE>
		class HandlerProxy : public IHandlerProxy
		{
		private:
			typedef void (TASK::*pHandler) (const MESSAGE&);
			pHandler handler;

		public:
			HandlerProxy(pHandler handler)
			{
				this->handler = handler;
			}

			void Execute(void *message)
			{
				(((TASK*)task)->*handler)(*((MESSAGE*)message));
			}
		};
	}
}
