#pragma once

namespace Kernel
{
	class ITask
	{
	public:
		virtual ~ITask()			{}

		virtual void Init()			{}
		virtual void Terminate()	{}	
	};
}