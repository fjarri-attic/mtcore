#pragma once

#include "..\mtcore\interfaces.h"
#include "..\mtcore\kernel.h"

#include "..\messages\log.h"
#include "..\messages\renderer.h"
#include "..\messages\game.h"

#include <d3dx9.h>

class TRenderer : public Kernel::ITask
{
private:
	LPDIRECT3D9  pD3D; 
	LPDIRECT3DDEVICE9 pDevice;

public:
	
	void Handler(const InitRenderer &message);
	void Handler(const CloseRenderer &message);
	void Handler(const RestartRenderer &message);
};