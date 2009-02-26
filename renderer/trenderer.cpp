#include "trenderer.h"
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) 
	{
	case WM_CLOSE:
		Kernel::SendMessage(WriteToLog("WM_CLOSE\n"));
		Kernel::SendMessage(UserWantsExit());
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


void TRenderer::Handler(const InitRenderer &message)
{
	Kernel::WindowParameters params;
	HWND hWnd;

	params.ClassName = "MT test";
	params.WindowName = "MT test";
	params.FullScreen = false;
	params.Height = 600;
	params.Width = 800;
	params.Xpos = 10;
	params.Ypos = 10;
	params.WndProc = WndProc;

	Kernel::SetWindowParams(params);
	hWnd = Kernel::MayCreateWindow();
	
	Kernel::SendMessage(WriteToLog("Starting D3D\n"));

	pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	d3dpp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &(pDevice) ); 

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

	Kernel::StartMessageQueue();
	Kernel::ShowWindow();
}

void TRenderer::Handler(const CloseRenderer &message)
{
	Kernel::SendMessage(WriteToLog("Releasing D3D\n"));
	if(pDevice != NULL) 
		pDevice->Release();

	if(pD3D != NULL)
		pD3D->Release();

	Kernel::Terminate();
}

void TRenderer::Handler(const RestartRenderer &message)
{
	Kernel::WindowParameters params;
	HWND hWnd;

	params.ClassName = "MT test";
	params.WindowName = "MT test 2";
	params.FullScreen = false;
	params.Height = 800;
	params.Width = 600;
	params.Xpos = 10;
	params.Ypos = 10;
	params.WndProc = WndProc;

	Kernel::SetWindowParams(params);
	hWnd = Kernel::RestartWindow();

	Kernel::SendMessage(WriteToLog("Starting D3D\n"));

	pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	d3dpp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &(pDevice) ); 

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

	Kernel::StartMessageQueue();
	Kernel::ShowWindow();
}
