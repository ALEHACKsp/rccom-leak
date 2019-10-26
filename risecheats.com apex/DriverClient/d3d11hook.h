#pragma once
#include "MinHook.h"
#include <Windows.h>
#include <d3d11.h>
#include "Utils.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include <d3d11.h>
#include "FontMenu.h"
#include "MenuIcon.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <mutex>
#pragma comment(lib,"libMinHook.x64.lib")
#pragma warning(disable:4996)
char NumberSpeedKey[2]{ 0 };
char SpeedKeyLabel[12]{ 0 };
char NumberAimKey[2]{ 0 };
char AimKeyLabel[12]{ 0 };

std::once_flag firstTime;
typedef HRESULT(__stdcall *D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

static ID3D11Device*            g_pDevice = nullptr;
static ID3D11DeviceContext*     g_pContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
ID3D11RenderTargetView *g_prendertargetview = nullptr;

DWORD64*                          pSwapChainVTable = nullptr;
DWORD64*                          pDeviceContextVTable = nullptr;

D3D11PresentHook phookD3D11Present = NULL;
WNDPROC o_wndproc;
HWND hWnd;

ImDrawList* draw_list;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//MessageBoxA(0, "Salam", "Slam", MB_OK);
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_INSERT)
		{
			Utils.b_ShowMenu = !Utils.b_ShowMenu;
		}
		if (!Utils.i_AimKey && wParam != 1)
		{
			Utils.i_AimKey = wParam;
		}
		if (!Utils.i_SpeedKey && wParam != 1)
		{
			Utils.i_SpeedKey = wParam;
		}
		break;
	}
	if (Utils.b_ShowMenu)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
	}
	return CallWindowProc(o_wndproc, hWnd, msg, wParam, lParam);
}
HRESULT __stdcall hookD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	int Width = 0, Height = 0;
	GetDesktopResolution(Width, Height);
	std::call_once(firstTime, [&]{
		pSwapChain->GetDevice(__uuidof(g_pDevice), (void**)&g_pDevice);
		g_pDevice->GetImmediateContext(&g_pContext);

		ID3D11Texture2D *pBackBuffer;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);


		g_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_prendertargetview);

		//shit way of doing it
		o_wndproc = (WNDPROC)(SetWindowLongPtrA(FindWindowA(NULL, "NVIDIA GeForce Overlay"), GWLP_WNDPROC, reinterpret_cast<uintptr_t>(WndProc)));

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.IniFilename = NULL;
		// Setup Platform/Renderer bindings
		//ImGui_ImplWin32_Init(hWnd,g_pDevice,g_pContext);
		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplDX11_CreateDeviceObjects();
		ImGui_ImplDX11_Init(g_pDevice, g_pContext);
		io.Fonts->AddFontFromMemoryCompressedBase85TTF(_compressed_data_base85, 16.0f, NULL, io.Fonts->GetGlyphRangesDefault());
		io.Fonts->AddFontFromMemoryCompressedBase85TTF(IconFont_base85, 72.0f, NULL, io.Fonts->GetGlyphRangesDefault());
	});
	g_pContext->OMSetRenderTargets(1, &g_prendertargetview, NULL);
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	SetMenu(hWnd, NULL);
	SetWindowLongPtr(hWnd, GWL_STYLE, WS_VISIBLE);
	//SetWindowLongPtr(m_DrawObject.g_hWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST); // WS_EX_NOACTIVATE  and WS_EX_TOOLWINDOW removes it from taskbar
	SetLayeredWindowAttributes(hWnd, 0, RGB(0, 0, 0), LWA_COLORKEY);
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	static short tab = 0;
	if (Utils.b_ShowMenu) //ImGui::ShowDemoWindow()
	{
		//ImGui::SetWindowSize(ImVec2(600, 1000), ImGuiCond_Always);
		ImGui::Begin(xorstr_("Anvial project"), (bool*)&Utils.b_ShowMenu, ImVec2(600, 290), -1.f, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
		ImGui::BeginChild(xorstr_("##tabs"), ImVec2(100, 255), true, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
		ImGui::PushFont(io.Fonts->Fonts[1]);
		if (ImGui::Button(xorstr_("D")))
		{
			tab = 0;
		}
		if (ImGui::Button(xorstr_("A")))
		{
			tab = 1;
		}
		if (ImGui::Button(xorstr_("G")))
		{
			tab = 2;
		}
		ImGui::PopFont();
		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::BeginChild(xorstr_("##main"), ImVec2(0, 0), false, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);
		{

			if (tab == 0)
			{
				ImGui::Checkbox(xorstr_("Glow ESP"), (bool*)&Utils.b_GlowEsp);
				ImGui::SliderFloat(xorstr_("Distance ESP"), (float*)&Utils.f_DistanceEsp, 0.f, 1000.f);
				//ImGui::Checkbox(xorstr_("Box ESP"), (bool*)&Utils.b_BoxESP);
			}
			if (tab == 1)
			{
				ImGui::Checkbox(xorstr_("Active"), (bool*)&Utils.b_AimBot);
				if (Utils.i_AimKey)
				{
					ZeroMemory(AimKeyLabel, sizeof(AimKeyLabel));
					itoa(Utils.i_AimKey, NumberAimKey, 10);
					strcat(AimKeyLabel, "AimKey: ");
					strcat(AimKeyLabel, NumberAimKey);
				}
				ImGui::SameLine();
				if (ImGui::Button(AimKeyLabel))
				{
					Utils.i_AimKey = 0;
					strcpy(AimKeyLabel, "...");
				}
				ImGui::SliderInt(xorstr_("Aim Bone"), (int*)&Utils.i_AimBone, 0, 1);
				ImGui::SliderFloat(xorstr_("FOV"), (float*)&Utils.fov, 0.0f, 50.0f);
				ImGui::SliderFloat(xorstr_("Distance"), (float*)&Utils.f_AimDistance, 0.0f, 1000.0f);
				//ImGui::SliderFloat(xorstr_("Offset"), (float*)&Utils.offsetAim, 0.0f, 20.0f);
				ImGui::SliderInt(xorstr_("Smooth"), (int*)&Utils.i_Smooth, 0, 10);
				ImGui::Checkbox(xorstr_("Draw FOV"), (bool*)&Utils.b_DrawFOV);
				ImGui::Checkbox(xorstr_("Prediction"), (bool*)&Utils.b_Prediction);
				ImGui::Checkbox(xorstr_("Aim at knocked players"), (bool*)&Utils.b_AimAtknocked);
				ImGui::Checkbox(xorstr_("Highlight aim target"), (bool*)&Utils.b_HighlightAimTarget);
				ImGui::Checkbox(xorstr_("RCS"), (bool*)&Utils.b_RCS);
			}
			if (tab == 2)
			{
				ImGui::Checkbox(xorstr_("Instant Bullet"), (bool*)&Utils.b_InstantHit);
				ImGui::Checkbox(xorstr_("NoSpread"), (bool*)&Utils.b_RemoveSpread);
				ImGui::SliderFloat(xorstr_("Spread factor"), (float*)&Utils.SpreadFactor, -10.0f, 0.0f);
				ImGui::Checkbox(xorstr_("Speed Hack"), (bool*)&Utils.b_SpeedHack);
				if (Utils.i_SpeedKey)
				{
					ZeroMemory(SpeedKeyLabel, sizeof(SpeedKeyLabel));
					itoa(Utils.i_SpeedKey, NumberSpeedKey, 10);
					strcat(SpeedKeyLabel, xorstr_("SpeedKey: "));
					strcat(SpeedKeyLabel, NumberSpeedKey);
				}
				ImGui::SameLine();
				if (ImGui::Button(SpeedKeyLabel))
				{
					Utils.i_SpeedKey = 0;
					strcpy(SpeedKeyLabel, "...");
				}
				ImGui::SliderFloat(xorstr_("Speed Factor"), (float*)&Utils.f_SpeedFactor, 0.0f, 6.0f);
			}
		}
		ImGui::EndChild();
		ImGui::End();
	}
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f });
	ImGui::Begin(xorstr_("##esp"), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);
	draw_list = ImGui::GetWindowDrawList();
	float color[] = { 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
	/*if (Utils.b_BoxESP)
	{
		for (int i = 0; i < Utils.CountPlayers; i++)
		{
			if (PlayerList[i].Address != NULL)
			{
				draw_list->AddRect(PlayerList[i].DrawInf.From, PlayerList[i].DrawInf.To, ImGui::GetColorU32(ImVec4(color[0], color[1], color[2], color[3])), 5, 0, 3);
			}
		}
	}*/
	if (Utils.b_DrawFOV)
	{
		draw_list->AddCircle(ImVec2(Width / 2, Height / 2), Utils.fov * 15, ImColor(255, 0, 0, 255), 100, 2.f);
	}
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);

	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return phookD3D11Present(pSwapChain, SyncInterval, Flags);
}
DWORD __stdcall InitializeHook()
{
	hWnd = FindWindowA(NULL, "NVIDIA GeForce Overlay");//FindMainWindow(GetCurrentProcessId());
	if (!hWnd)
	{
		MessageBoxA(0, "Can't find wnd!", "Err", MB_OK);
		return NULL;
	}
	D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC sd;
	{
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.Windowed = TRUE;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &level, 1, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pDevice, nullptr, &g_pContext);
	if (FAILED(hr))
	{
		MessageBox(hWnd, L"Failed to create device and swapchain.", L"Fatal Error", MB_ICONERROR);
		return E_FAIL;
	}

	pSwapChainVTable = reinterpret_cast<DWORD64*>(g_pSwapChain);
	pSwapChainVTable = reinterpret_cast<DWORD64*>(pSwapChainVTable[0]);

	pDeviceContextVTable = reinterpret_cast<DWORD64*>(g_pContext);
	pDeviceContextVTable = reinterpret_cast<DWORD64*>(pDeviceContextVTable[0]);

	if (MH_Initialize() != MH_OK) { return 1; }
	if (MH_CreateHook((DWORD64*)pSwapChainVTable[8], hookD3D11Present, reinterpret_cast<void**>(&phookD3D11Present)) != MH_OK) { return 1; }
	if (MH_EnableHook((DWORD64*)pSwapChainVTable[8]) != MH_OK) { return 1; }

	DWORD old_protect;
	VirtualProtect(phookD3D11Present, 2, PAGE_EXECUTE_READWRITE, &old_protect);

	g_pDevice->Release();
	g_pContext->Release();
	g_pSwapChain->Release();
	MessageBoxA(0, "Salam", "Aleykum", MB_OK);
	return S_OK;
}