#define _CRT_SECURE_NO_WARNINGS
#include "Utils.h"
#include "Draw.h"
#include <Windows.h>
#include <iostream>
#include <ntsecapi.h>
#include <TlHelp32.h>
#include <thread>
#include <random>
#include <cstdint>
#include "XorStr.h"
#include "FontMenu.h"
#include "MenuIcon.h"
#include "MenuHelpers.h"
//? IMGUI
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include <queue> 
#include "LootIcons.h"
#include "Notifications.h"
#include "VMatrix.hpp"
// Data
WNDPROC o_wndproc;
static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

#define NVIDIA
#pragma region Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Win32 message handler
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#ifdef NVIDIA
LRESULT WINAPI WndProc(HWND HookhWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (!Settings.i_AimKey && wParam != 1 && wParam < 50000)
		{
			Settings.i_AimKey = wParam;
		}
		else if (!Settings.i_AimKey && lParam != 1 && lParam < 50000)
		{
			Settings.i_AimKey = lParam;
		}
		else if (!Settings.i_SpeedKey && wParam != 1 && wParam < 50000)
		{
			Settings.i_SpeedKey = wParam;
		}
		else if (!Settings.i_SpeedKey && lParam != 1 && lParam < 50000)
		{
			Settings.i_SpeedKey = lParam;
		}
		break;
	}
	if (Utils.b_ShowMenu)
	{
		ImGui_ImplWin32_WndProcHandler(HookhWnd, msg, wParam, lParam);
	}
	return CallWindowProc(o_wndproc, HookhWnd, msg, wParam, lParam);
}

#endif // NVIDIA
#ifdef AMD
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	/*if (Utils.b_ShowMenu)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
	}*/
	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	//CallWindowProcA(o_wndproc, hWnd, msg, wParam, lParam);
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
#endif // AMD
bool EnablePrivilege(LPCWSTR lpPrivilegeName)
{
	TOKEN_PRIVILEGES Privilege;
	HANDLE hToken;
	DWORD dwErrorCode;

	Privilege.PrivilegeCount = 1;
	Privilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!LookupPrivilegeValueW(NULL, lpPrivilegeName,
		&Privilege.Privileges[0].Luid))
		return GetLastError();

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES, &hToken))
		return GetLastError();

	if (!AdjustTokenPrivileges(hToken, FALSE, &Privilege, sizeof(Privilege),
		NULL, NULL)) {
		dwErrorCode = GetLastError();
		CloseHandle(hToken);
		return dwErrorCode;
	}

	CloseHandle(hToken);
	return TRUE;
}
//? ////////////////////
//? ////////////////////
NTSTATUS(NTAPI *NtLoadDriver)(IN PUNICODE_STRING DriverServiceName);
bool LoadDriver()
{
	/*NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING pPath;
	UNICODE_STRING pPathReg;
	//HMODULE hNtDll = LoadLibrary(L"ntdll.dll");
	const char NTDLL[] = { 0x6e, 0x74, 0x64, 0x6c, 0x6c, 0x2e, 0x64, 0x6c, 0x6c, 0x00 };
	HMODULE hObsolete = GetModuleHandleA(NTDLL);
	typedef NTSTATUS(__stdcall* LdrDrv)(PUNICODE_STRING);
	LdrDrv NtLoadDriver = (LdrDrv)GetProcAddress(hObsolete, "NtLoadDriver");
	if (NtLoadDriver != NULL)
	{
		printf("NtLoadDriver succesfully found!\n");
	}
	if (EnablePrivilege(L"SeLoadDriverPrivilege"))
	{
		printf("Succefully get SeLoadDriverPrivilege!\n");
	}
	PCWSTR pPathSource = L"B:\Projects\KernelMmap\x64\Release\ForWork\KpInstaller.sys";
	PCWSTR pPathSourceReg = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\FaerDvc";
	RtlInitUnicodeString(&pPath, pPathSource);
	RtlInitUnicodeString(&pPathReg, pPathSourceReg);
	status = NtLoadDriver(&pPathReg);
	if (status == STATUS_SUCCESS)
	{
		printf("Driver succefully loaded!\n");
	}*/
	return true;
}
DWORD GetPID(const wchar_t* ProcName)
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hSnap, &ProcEntry))
	{
		while (Process32Next(hSnap,&ProcEntry))
		{
			if (wcscmp(ProcEntry.szExeFile, ProcName) == 0)
			{
				return ProcEntry.th32ProcessID;
			}
		}
	}
	return 0;
}
#pragma endregion
struct Color
{
	float Red;
	float Green;
	float Blue;
};
#pragma region HackFunc
void EnableHighlight(Driver *DrvObj,PVOID Entity, float r, float g, float b) {
	bool State = true;
    DrvObj->WriteMemory(((BYTE*)Entity + 0x380), &State, sizeof(float)); // Enabling the Glow
	int i = 1;
	DrvObj->WriteMemory(((BYTE*)Entity + 0x300), &i, sizeof(float)); // Enabling the Glow
	float color[3] = { r,g,b};
	DrvObj->WriteMemory(((BYTE*)Entity + 0x1C0), &color, sizeof(color));
	float transparency = 0.7f;//*2.5f;
	float Time = 0.01f;
	for (int offset = 704; offset <= 728; offset += 4)//Setting the of the Glow at all necessary spots
	{
		DrvObj->WriteMemory(((BYTE*)Entity + offset), &Time, sizeof(float));// Setting the time of the Glow to be the Max Float value so it never runs out
	}
	DrvObj->WriteMemory(((BYTE*)Entity + 0x2EC), (float*)&Settings.f_DistanceEsp, sizeof(float)); //Set the Distance of the Glow to Max float value so we can see a long Distance
}
void __fastcall GetBonesPosAddress(Player* pPlayer, int *Id)
{
	for (size_t i = 0; i < 15; i++)
	{
		pPlayer->pBonePos[i] = (BYTE*)pPlayer->BoneMatrixPtr + Id[i] * 0x30;
	}
}
void GetPlayerBones(Driver* DrvObj, Player* pPlayer)
{
	C_Entity Entity;
	string Hero = Entity.ReadModName(DrvObj, pPlayer->Address, xorstr_("#Nop"));
	if (strstr(Hero.c_str(), xorstr_("pilot_medium_holo")) != 0)
	{
		if (strstr(Hero.c_str(), xorstr_("legendary_01")) != 0)
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_medium_holo1);
		}
		else if (strstr(Hero.c_str(), xorstr_("legendary_02")) != 0)
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_medium_holo2);
		}
		else
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_medium_holo);
		}
		//pPlayer->NameHero = xorstr_("pilot_medium_holo");
		return;
	}

	if (strstr(Hero.c_str(), xorstr_("pilot_heavy_caustic")) != 0)
	{
		if (strstr(Hero.c_str(), xorstr_("legendary_01")) != 0)
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_heavy_caustic1);
		}
		else if (strstr(Hero.c_str(), xorstr_("legendary_02")) != 0)
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_heavy_caustic2);
		}
		else
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_heavy_caustic);
		}
		//pPlayer->NameHero = xorstr_("pilot_heavy_caustic");
		return;
	}

	if (strstr(Hero.c_str(), xorstr_("pilot_heavy_gibraltar")) != 0)
	{
		if (strstr(Hero.c_str(), xorstr_("legendary_02")) != 0)
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_heavy_gibraltar2);
		}
		else
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_heavy_gibraltar);
		}
		//pPlayer->NameHero = xorstr_("pilot_heavy_gibraltar");
		return;
	}

	if (strstr(Hero.c_str(), xorstr_("pilot_light_support")) != 0)
	{
		GetBonesPosAddress(pPlayer, _Bone::Bone_lifeline_support);
		//pPlayer->NameHero = xorstr_("pilot_light_support");
		return;
	}

	if (strstr(Hero.c_str(), xorstr_("pilot_heavy_pathfinder")) != 0)
	{
		if (strstr(Hero.c_str(), xorstr_("legendary_01")) != 0)
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_heavy_pathfinder1);
		}
		else if (strstr(Hero.c_str(), xorstr_("legendary_02")) != 0)
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_heavy_pathfinder2);
		}
		else
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_heavy_pathfinder);
		}
		//pPlayer->NameHero = xorstr_("pilot_heavy_pathfinder");
		return;
	}

	if (strstr(Hero.c_str(), xorstr_("pilot_medium_bloodhound")) != 0)
	{
		if (strstr(Hero.c_str(), xorstr_("legendary_01")) != 0)
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_medium_bloodhound1);
		}
		else if (strstr(Hero.c_str(), xorstr_("legendary_02")) != 0)
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_medium_bloodhound2);
		}
		else
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_medium_bloodhound);
		}
		//pPlayer->NameHero = xorstr_("pilot_medium_bloodhound");
		return;
	}

	if (strstr(Hero.c_str(), xorstr_("pilot_light_wraith")) != 0)
	{
		if (strstr(Hero.c_str(), xorstr_("legendary_01")) != 0)
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_light_wraith1);
		}
		else if (strstr(Hero.c_str(), xorstr_("legendary_02")) != 0)
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_light_wraith2);
		}
		else
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_light_wraith);
		}
		//pPlayer->NameHero = xorstr_("pilot_light_wraith");
		return;
	}

	if (strstr(Hero.c_str(), xorstr_("pilot_medium_bangalore")) != 0)
	{
		if (strstr(Hero.c_str(), xorstr_("legendary_01")) != 0)
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_medium_bangalore1);
		}
		else if (strstr(Hero.c_str(), xorstr_("legendary_02")) != 0)
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_medium_bangalore2);
		}
		else
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_medium_bangalore);
		}
		//pPlayer->NameHero = xorstr_("pilot_medium_bangalore");
		return;
	}
	if (strstr(Hero.c_str(), xorstr_("pilot_medium_stim")) != 0)
	{
		if (strstr(Hero.c_str(), xorstr_("legendary_01")) != 0)
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_medium_stim1);
		}
		else if (strstr(Hero.c_str(), xorstr_("legendary_02")) != 0)
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_medium_stim2);
		}
		else
		{
			GetBonesPosAddress(pPlayer, _Bone::Bone_medium_stim);
		}
		//pPlayer->NameHero = xorstr_("pilot_medium_stim");
		return;
	}
}
void FillPlayerList(Driver *DrvObj)
{
	C_Entity Entity;
	while (true)
	{
		if (Entity.IsInGame(DrvObj))
		{
			Utils.CountPlayers = 0;
			PVOID EntHandle = 0;
			LPCSTR IdentifierC = 0;
			int LocalPlayerIndex = 0;
			int LocalPlayerTeamNum = 0;
			int EnemyTeamNum = 0;
			int EntHealth = 0;
			//	DrvObj->ReadMemory(((BYTE*)Utils.BaseModule + 0x2353448), (PVOID)&Utils.LocalPlayer, sizeof(PVOID));
			DrvObj->ReadMemory(((BYTE*)Utils.BaseModule + LocalEntityId), &LocalPlayerIndex, sizeof(float));
			Utils.LocalPlayer = Entity.GetLocalPlayer(DrvObj, Utils.EntitList, LocalPlayerIndex);
			if (!Utils.LocalPlayer)
			{
				ZeroMemory(PlayerList, sizeof(PlayerList));
				ZeroMemory(ItemList, sizeof(ItemList));
				continue;
			}
			DrvObj->ReadMemory(((BYTE*)Utils.LocalPlayer + m_iTeamNum), &LocalPlayerTeamNum, 0x4);
			DrvObj->ReadMemory(((BYTE*)Utils.BaseModule + dwEntityCount), &Utils.EntitCount, sizeof(ULONG));
			Utils.CountItem = 0;
			C_Vec LocalPlayerPos;
			DrvObj->ReadMemory(((BYTE*)Utils.LocalPlayer + m_vecAbsOrigin), &LocalPlayerPos, sizeof(C_Vec));
			for (int i = 0; i < Utils.EntitCount; i++)  // Enumerating through the first 100 Entities in the List because thats where all players are stored
			{
				PVOID EntityAddr = Entity.GetEntityById(DrvObj, i, Utils.EntitList);
				if (EntityAddr == 0) // Checking if its valid
					continue;
				if (EntityAddr != 0)
				{
					DrvObj->ReadMemory(((BYTE*)EntityAddr + m_iSignifierName), &EntHandle, sizeof(PVOID));
					std::string Identifier;
					DrvObj->ReadMemory(EntHandle, &Identifier, sizeof(DWORD64)); // Getting an Identifier that Tells us what the Entity is
					IdentifierC = Identifier.c_str(); // Converting the string because im stupid
					if (strcmp(IdentifierC, xorstr_("player")) == 0) //Comparing if the Identifier states that the Entity is indeed a Player
					{
						DrvObj->ReadMemory(((BYTE*)EntityAddr + m_iHealth), &EntHealth, 0x4);
						if (EntHealth > 0)
						{
							DrvObj->ReadMemory(((BYTE*)EntityAddr + m_iTeamNum), &EnemyTeamNum, 0x4);
							if (EnemyTeamNum != LocalPlayerTeamNum)
							{
								PlayerList[Utils.CountPlayers].Address = EntityAddr;
								PlayerList[Utils.CountPlayers].BoneMatrixPtr = Entity.GetBoneMatrixPtr(DrvObj, EntityAddr);
								GetPlayerBones(DrvObj, &PlayerList[Utils.CountPlayers]);
								if (Settings.b_BoxESP)
								{
									C_Vec pos3d;
									DrvObj->ReadMemory(((BYTE*)PlayerList[Utils.CountPlayers].Address + m_vecAbsOrigin), &pos3d, sizeof(C_Vec));
									if (LocalPlayerPos.distance(pos3d) < Settings.f_BoxDistance * 100)
									{
										PlayerList[Utils.CountPlayers].b_VisibleBox = true;
									}
									else
									{
										PlayerList[Utils.CountPlayers].b_VisibleBox = false;
									}
									if (LocalPlayerPos.distance(pos3d) < Settings.f_RadarRange * 100)
									{
										PlayerList[Utils.CountPlayers].b_VisibleRadar = true;
									}
									else
									{
										PlayerList[Utils.CountPlayers].b_VisibleRadar = false;
									}
								}
								Utils.CountPlayers++;
							}
						}
					}
					if (strstr(IdentifierC, xorstr_("rop_sur")) != 0)// Deafth crates and airdrop "prop_dyn"
					{
						C_Vec ItemPos;
						DrvObj->ReadMemory(((BYTE*)EntityAddr + m_vecAbsOrigin), &ItemPos, sizeof(C_Vec));
						if (LocalPlayerPos.distance(ItemPos) < Settings.f_LootEspDistance * 100.f)
						{
							ItemList[Utils.CountItem].Address = EntityAddr;
							Utils.CountItem++;
						}
					}
				}
			}
			Sleep(1500);
		}
		else
		{
			Utils.LocalPlayer = NULL;
			Sleep(200);
		}
	}
}
inline bool CheckFOV(C_Vec src,C_Vec dst,int FOV)
{
	float m_distance = sqrt(pow(src.x - dst.x, 2) + pow(src.y - dst.y, 2));
	if (m_distance <= FOV && m_distance > 0)
	{
		return true;
	}
	else
		return false;
}
void GlowESP(Driver *DriverObj)
{
	while (true)
	{
		if (Settings.b_GlowEsp && Utils.IsInGame)
		{
			for (int i = 0; i < Utils.CountPlayers; i++)
			{
				if (PlayerList[i].Address != NULL)
				{
					int EnemyHealth = 0;
					DriverObj->ReadMemory(((BYTE*)PlayerList[i].Address + m_iHealth), &EnemyHealth, 0x4);
					if (EnemyHealth > 0)
					{
						if (PlayerList[i].Address == Utils.TargetEntitty.Address && Settings.b_HighlightAimTarget)
						{
							EnableHighlight(DriverObj, (BYTE*)PlayerList[i].Address, 255.f, 26.f, 226.f);
							continue;
						}
						if (Settings.f_DistanceEsp)
						{
							C_Vec EnemyPos;
							C_Vec LocalPlayerPos;
							DriverObj->ReadMemory(((BYTE*)PlayerList[i].Address + m_vecAbsOrigin), &EnemyPos, 0x12);
							DriverObj->ReadMemory(((BYTE*)Utils.LocalPlayer + m_vecAbsOrigin), &LocalPlayerPos, 0x12);
							if (LocalPlayerPos.distance(EnemyPos) > Settings.f_DistanceEsp * 100)
							{
								continue;
							}
						}
						if (Settings.TypeGlow == 1)
						{
							if (EnemyHealth >= 70)
								EnableHighlight(DriverObj, (BYTE*)PlayerList[i].Address, 0.f, 247.f, 12.f);	// Enabling the Glow for that Entity since it is a Player
							if (EnemyHealth > 50 && EnemyHealth <= 69)
								EnableHighlight(DriverObj, (BYTE*)PlayerList[i].Address, 247.f, 127.f, 0.f);
							if (EnemyHealth >= 20 && EnemyHealth <= 50)
								EnableHighlight(DriverObj, (BYTE*)PlayerList[i].Address, 247.f, 238.f, 0.f);
							if (EnemyHealth > 0 && EnemyHealth < 20)
								EnableHighlight(DriverObj, (BYTE*)PlayerList[i].Address, 247.f, 0.f, 0.f);
						}
						else
						{
							if (EnemyHealth >= 70)
								EnableHighlight(DriverObj, (BYTE*)PlayerList[i].Address, 0.0f, 0.969f, 0.047f);	// Enabling the Glow for that Entity since it is a Player
							if (EnemyHealth > 50 && EnemyHealth <= 69)
								EnableHighlight(DriverObj, (BYTE*)PlayerList[i].Address, 0.969f, 0.498f, 0.0f);
							if (EnemyHealth >= 20 && EnemyHealth <= 50)
								EnableHighlight(DriverObj, (BYTE*)PlayerList[i].Address, 0.969f, 0.933f, 0.0f);
							if (EnemyHealth > 0 && EnemyHealth < 20)
								EnableHighlight(DriverObj, (BYTE*)PlayerList[i].Address, 0.969f, 0.0f, 0.0f);
						}
					}
					else
					{
						continue;
					}
				}
			}
			Sleep(50);
		}
		else
		{
			Sleep(300);
		}
	}
}
C_Vec aimCorrection(C_Vec localPos, C_Vec aimPoint, C_Vec targetVelocity, float bulletSpeed, float gravity)
{
	float distance = localPos.distance(aimPoint);
	float flTime = distance / bulletSpeed;
	gravity = flTime * flTime*375.f;

	aimPoint.x += flTime * targetVelocity.x;
	aimPoint.y += flTime * targetVelocity.y;
	aimPoint.z += flTime * targetVelocity.z + gravity;
	return aimPoint;
}
void AimBot(Driver *DriverObj)
{
	C_Entity Entity;
	while (true)
	{
		if (Settings.b_AimBot && Utils.IsInGame && PlayerList[0].Address != NULL)
		{
			for (int i = 0; i < Utils.CountPlayers; i++)  // Enumerating through the first 100 Entities in the List because thats where all players are stored
			{
				C_Vec EnemyPlayerPos;
				C_Vec EnemyBonePos;
				C_Vec LocalViewAngle;
				C_Vec RecoilVec;
				_GameView LocalView;
				C_Vec AimBut;
				if (!Settings.b_AimAtknocked)
				{
					int PlayerBlendingState = 0;
					DriverObj->ReadMemory(((BYTE*)PlayerList[i].Address + m_bleedoutState), &PlayerBlendingState, 0x2);
					if (PlayerBlendingState == 2)
					{

						continue;
					}
				}
				DriverObj->ReadMemory(((BYTE*)Utils.LocalPlayer + ViewAngle), &LocalViewAngle, 0x12);
				//? Get our player cam pos
				Entity.GetView(DriverObj, LocalView);
				//? Get enemy player bone pos
				DriverObj->ReadMemory(((BYTE*)PlayerList[i].Address + m_vecAbsOrigin), &EnemyPlayerPos, 0x12);
				if (Settings.i_AimBone == 1)
				{
					Entity.GetBonePos(DriverObj, EnemyBonePos, PlayerList[i].BoneMatrixPtr, bone_id::c_spineC);
				}
				else
				{
					Entity.GetBonePos(DriverObj, EnemyBonePos, PlayerList[i].BoneMatrixPtr, bone_id::c_head);
				}
				EnemyBonePos += EnemyPlayerPos;
				if (LocalView.m_Pos.distance(EnemyBonePos) > Settings.f_AimDistance * 100)
				{
					continue;
				}
				AimBut.VectorAngles((EnemyBonePos - LocalView.m_Pos));
				AimBut.AngleNormalize();
				if (CheckFOV(LocalViewAngle, AimBut, Settings.fov) == true)
				{
					Utils.TargetEntitty = PlayerList[i];
					while (GetAsyncKeyState(Settings.i_AimKey))
					{
						//? Get our player cam pos
						DriverObj->ReadMemory(((BYTE*)Utils.LocalPlayer + ViewAngle), &LocalViewAngle, 0x12);
						Entity.GetView(DriverObj, LocalView);
						//? Get enemy player bone pos
						if (Settings.i_AimBone == 1)
						{
							Entity.GetBonePos(DriverObj, EnemyBonePos, Utils.TargetEntitty.BoneMatrixPtr, bone_id::c_spineC);
						}
						else
						{
							Entity.GetBonePos(DriverObj, EnemyBonePos, Utils.TargetEntitty.BoneMatrixPtr, bone_id::c_head);
						}
						DriverObj->ReadMemory(((BYTE*)Utils.TargetEntitty.Address + m_vecAbsOrigin), &EnemyPlayerPos, 0x12);
						EnemyBonePos += EnemyPlayerPos;
						if (Settings.b_Prediction || Settings.b_RemoveSpread)
						{
							Entity.GetPrimaryWeapon(DriverObj);
							if (Utils.WeaponAddr == NULL)
							{
								break;
							}
						}
						if (Settings.b_Prediction)
						{
							C_Vec TargetVelocity;
							DriverObj->ReadMemory(((BYTE*)Utils.TargetEntitty.Address + (int)m_vecAbsVelocity), &TargetVelocity, 0x12);
							float WeaponBulletSpeed = 0;
							if (!Settings.b_InstantHit)
							{
								DriverObj->ReadMemory(((BYTE*)Utils.WeaponAddr + BulletSpeed), &WeaponBulletSpeed, 0x4);
							}
							else
							{
								WeaponBulletSpeed = Utils.WeaponBulletSpeed;
							}
							if (WeaponBulletSpeed != 1.f)//Check weapon
							{
								float Distance = LocalView.m_Pos.distance(EnemyBonePos);
								float flTime = Distance / fabs(WeaponBulletSpeed);
								float gravity = flTime * flTime*375.f;
								EnemyBonePos.x += flTime * TargetVelocity.x;
								EnemyBonePos.y += flTime * TargetVelocity.y;
								EnemyBonePos.z += flTime * TargetVelocity.z + gravity;
							}
						}
						AimBut.VectorAngles((EnemyBonePos - LocalView.m_Pos));
						AimBut.AngleNormalize();
						AimBut.ClampAng();
						if (Settings.f_Smooth != NULL)
						{
							C_Vec SmoothAngle;
							SmoothAngle.SmoothAngles(LocalViewAngle, AimBut, SmoothAngle, (float)Settings.f_Smooth);
							AimBut = SmoothAngle;
						}
						if (Settings.b_RCS)
						{
							DriverObj->ReadMemory(((BYTE*)Utils.LocalPlayer + m_vecAimPunch), &RecoilVec, 0x12);
							if (RecoilVec.x != 0 || RecoilVec.y != 0)
							{
								AimBut -= RecoilVec;
							}
						}
						int LocalHealth = 0;
						DriverObj->ReadMemory(((BYTE*)Utils.LocalPlayer + m_iHealth), &LocalHealth, 0x2);
						if (LocalHealth > 0)
						{
							if (Settings.b_InstantHit)
							{
								DriverObj->WriteMemory(((BYTE*)Utils.WeaponAddr + BulletSpeed), (LPVOID)&Utils.WeaponBulletSpeed, 0x4);
							}
							if (Settings.b_RemoveSpread)
							{
								WeaponSpread.x = Settings.SpreadFactor;
								WeaponSpread.y = Settings.SpreadFactor;
								WeaponSpread.z = Settings.SpreadFactor;
								DriverObj->WriteMemory(((BYTE*)Utils.WeaponAddr + m_vecWeaponSpread), &WeaponSpread, sizeof(Spread));
							}
							if (Utils.b_RemoveBreath)
							{
								//LocalView.m_Pos -= PlayerBreath;
								//Utils.LocalBreath = (LocalViewAngle + LastBreath) - Utils.PlayerBreath;
								//AimBut = (AimBut + LastBreath) - PlayerBreath;
								/*PlayerBreath = AimBut - PlayerBreath;*/
								//Utils.NewAngle = AimBut - Utils.PlayerBreath;
								//AimBut += LastBreath;
								//AimBut -= Utils.PlayerBreath;
								//Utils.LastBreath = Utils.PlayerBreath;
							}
							//Utils.AimAngle = AimBut;
							AimBut.AngleNormalize();
							AimBut.ClampAng();
							//bool b_SendPackets = false;
							//DriverObj->WriteMemory(((BYTE*)Utils.BaseModule + net_droppackets), &b_SendPackets, sizeof(bool));
							DriverObj->WriteMemory(((BYTE*)Utils.LocalPlayer + ViewAngle), &AimBut, 0x12);
							/*Sleep(6);
							b_SendPackets = true;
							DriverObj->WriteMemory(((BYTE*)Utils.BaseModule + net_droppackets), &b_SendPackets, sizeof(bool));*/
						}
						continue;
					}
					Utils.TargetEntitty.Address = 0;
					continue;
				}
				continue;
			}
			Sleep(5);
		}
		else
		{
			Sleep(400);
		}
	}
}
void SpeedHack(Driver *DriverObj)
{
	while (true)
	{
		/*if (Settings.b_SpeedHack && Utils.IsInGame && Utils.LocalPlayer && PlayerList[0].Address != NULL)
		{
			if (GetAsyncKeyState(Settings.i_SpeedKey))
			{
				DriverObj->WriteMemory(((BYTE*)Utils.BaseModule + tickspeed), (LPVOID)&Settings.f_SpeedFactor, 0x4);
			}
			else
			{
				float TempSpeed = 1.f;
				DriverObj->WriteMemory(((BYTE*)Utils.BaseModule + tickspeed), (LPVOID)&TempSpeed, 0x4);
			}
		}*/
		if (GetAsyncKeyState(VK_SPACE) && Settings.b_Bhop)
		{
			int Value = 6;
			DriverObj->WriteMemory(((BYTE*)Utils.BaseModule + ForceJump), &Value, 0x4);
		}
		Sleep(100);
	}
}
#pragma endregion
HWND FindMainWindow(DWORD pid)
{
	std::pair<HWND, DWORD> params = { 0, pid };
	BOOL bResult = EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
	{
		auto pParams = (std::pair<HWND, DWORD>*)(lParam);

		DWORD processId;
		if (GetWindowThreadProcessId(hwnd, &processId) && processId == pParams->second)
		{
			SetLastError(-1);
			pParams->first = hwnd;
			return FALSE;
		}
		return TRUE;
	}, (LPARAM)&params);


	if (!bResult && GetLastError() == -1 && params.first)
		return params.first;

	return 0;
}
void MainFunc()
{
	DWORD PID = GetPID(xorstr_(L"r5apex.exe"));
	Driver DriverObj(PID);
	Utils.BaseModule = DriverObj.GetModuleBase();
	Utils.EntitList = (PVOID)((BYTE*)Utils.BaseModule + EntityList);
	thread UpdateInfo(FillPlayerList, &DriverObj);
	Sleep(1);
	thread MakeGlow(GlowESP, &DriverObj);
	thread MakeAimBot(AimBot, &DriverObj);
	//thread MakeSpeedHack(SpeedHack, &DriverObj);
	GetDesktopResolution(Utils.Width, Utils.Height);
#pragma region NewMenu
	DrawingClass m_DrawObject(Utils.Width, Utils.Height, true);
#ifdef AMD
	m_DrawObject.lpfnWndProc = WndProc;
	m_DrawObject.InitWindow();
#endif 
	if (!CreateDeviceD3D(m_DrawObject.g_hWnd))
	{
#ifdef DEBUGPRINT
		MessageBox(0,xorstr_(L"Can't create device"), xorstr_(L"Error"), MB_OK);
#endif 
		return;
	}
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = NULL;
	ImGui_ImplWin32_Init(m_DrawObject.g_hWnd);
	ImGui_ImplDX11_CreateDeviceObjects();
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
	io.Fonts->AddFontFromMemoryCompressedBase85TTF(_compressed_data_base85, 16.0f, NULL, io.Fonts->GetGlyphRangesDefault());
	io.Fonts->AddFontFromMemoryCompressedBase85TTF(IconFont_base85, 72.0f, NULL, io.Fonts->GetGlyphRangesDefault());
	io.Fonts->AddFontFromMemoryCompressedBase85TTF(_compressed_data_base85, 22.0f, NULL, io.Fonts->GetGlyphRangesDefault());
	//IM_ASSERT(font != NULL);
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
	// Main loop
	MSG msg{ 0 };
	C_Entity Entity;
	C_Radar  Radar;
	_GameView LocalView;
	ShaderResourceIni(g_pd3dDevice);
	while (true)
	{
#ifdef AMD
		SetWindowPos(m_DrawObject.g_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		if (GetAsyncKeyState(VK_INSERT))
		{
			Utils.b_ShowMenu = !Utils.b_ShowMenu;
			Sleep(200);
			if (!Utils.b_ShowMenu)
			{
				SetWindowLongPtr(m_DrawObject.g_hWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
				Utils.MenuSize = { 0,0 };
			}
			else
			{
				SetWindowLongPtr(m_DrawObject.g_hWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW);
			}
		}
		if (Utils.MenuSize.x < 600.f || Utils.MenuSize.y < 375.f)
		{
			if (Utils.MenuSize.x < 600.f)
			{
				Utils.MenuSize.x += 10;
			}
			if (Utils.MenuSize.y < 375.f)
			{
				Utils.MenuSize.y += 5;
			}
		}
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (!Settings.i_AimKey && msg.wParam != 1 && msg.wParam < 50000)
			{
				Settings.i_AimKey = msg.wParam;
			}
			else if (!Settings.i_AimKey && msg.lParam != 1 && msg.lParam < 50000)
			{
				Settings.i_AimKey = msg.lParam;
			}
			else if (!Settings.i_SpeedKey && msg.wParam != 1 && msg.wParam < 50000)
			{
				Settings.i_SpeedKey = msg.wParam;
			}
			else if (!Settings.i_SpeedKey && msg.lParam != 1 && msg.lParam < 50000)
			{
				Settings.i_SpeedKey = msg.lParam;
			}
		}
#endif
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		static short tab = 0;
		static int SubTab = 0;
		if (Utils.b_ShowMenu)
		{
			if (Settings.StyleMenu == 0)
			{
				ImGui::StyleNewMenu();
			}
			else
			{
				ImGui::StyleColorsDark();
			}
			ImGui::Begin(xorstr_("Rise Cheats Apex Legends Project"), (bool*)&Utils.b_ShowMenu, Utils.MenuSize, -1.f, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);
			ImGui::SetWindowSize(Utils.MenuSize);
			ImGui::BeginChild(xorstr_("##tabs"),ImVec2(100, 338), true, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
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
			if (ImGui::Button(xorstr_("H")))
			{
				tab = 3;
			}
			ImGui::PopFont();
			ImGui::EndChild();
			ImGui::SameLine();
			ImGui::BeginChild(xorstr_("##main"), ImVec2(0, 0), false, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);
			{

				if (tab == 0)
				{
					if (ImGui::ButtonEx(xorstr_("Players"), ImVec2(160, 25), ImGuiButtonFlags_::ImGuiButtonFlags_PressedOnClick))
					{
						SubTab = 0;
					}
					ImGui::SameLine();
					if (ImGui::ButtonEx(xorstr_("Loot"), ImVec2(160, 25), ImGuiButtonFlags_::ImGuiButtonFlags_PressedOnClick))
					{
						SubTab = 1;
					}
					ImGui::SameLine();
					if (ImGui::ButtonEx(xorstr_("Radar"), ImVec2(160, 25), ImGuiButtonFlags_::ImGuiButtonFlags_PressedOnClick))
					{
						SubTab = 2;
					}
					if (SubTab == 0)
					{
						ImGui::Checkbox(xorstr_("Glow esp"), (bool*)&Settings.b_GlowEsp);
						ImGui::Combo(xorstr_("Type glow esp"), (int*)&Settings.TypeGlow, TypesGlow, IM_ARRAYSIZE(TypesGlow));
						ImGui::SliderFloat(xorstr_("Glow esp distance"), (float*)&Settings.f_DistanceEsp, 0.f, 1000.f);
						ImGui::Checkbox(xorstr_("Box esp"), (bool*)&Settings.b_BoxESP);
						ImGui::SliderFloat(xorstr_("Box esp distance"), (float*)&Settings.f_BoxDistance, 0.f, 400.f);
						ImGui::Combo(xorstr_("Type box esp"), (int*)&Settings.TypeBox, TypesBox, IM_ARRAYSIZE(TypesBox));
						ImGui::Checkbox(xorstr_("Show health"), (bool*)&Settings.b_ShowHealth);
						ImGui::Checkbox(xorstr_("Show shield"), (bool*)&Settings.b_ShowShield);
						ImGui::Checkbox(xorstr_("Show distance"), (bool*)&Settings.b_ShowDistance);
						ImGui::Checkbox(xorstr_("Snaplines"), (bool*)&Settings.b_ShowSnapline);
						ImGui::Checkbox(xorstr_("Skeleton"), (bool*)&Settings.b_ShowSkeleton);
					}
					if (SubTab == 1)
					{
						ImGui::SliderFloat(xorstr_("Loot esp distance"), (float*)&Settings.f_LootEspDistance, 0.f, 100.f);
						if (ImGui::BeginCombo(xorstr_("Loot lvl"),LootPreviewValue.c_str()))
						{
							LootPreviewValue = " ";
							vector<string>PreviewValue;
							for (size_t i = 0; i < IM_ARRAYSIZE(LootLvls); i++)
							{
								ImGui::Selectable(LootLvls[i], &Settings.b_LvlLoot[i], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
								if (Settings.b_LvlLoot[i])
								{
									PreviewValue.push_back(LootLvls[i]);
								}
							}
							for (size_t i = 0; i < PreviewValue.size(); i++)
							{
								if (PreviewValue.size() == 1)
									LootPreviewValue += PreviewValue.at(i);
								else if (!(i == PreviewValue.size() - 1))
									LootPreviewValue += PreviewValue.at(i) + ",";
								else
									LootPreviewValue += PreviewValue.at(i);
							}
							ImGui::EndCombo();
						}
						ImGui::Checkbox(xorstr_("Show item id"), (bool*)&Settings.b_ShowItemId);
						ImGui::Checkbox(xorstr_("Backpacks"), (bool*)&Settings.b_ShowBackpacks);
						ImGui::Checkbox(xorstr_("Med and armor"), (bool*)&Settings.b_ShowMedAndArmor);
						ImGui::Checkbox(xorstr_("Weapon"), (bool*)&Settings.b_ShowWeapon);
						ImGui::Checkbox(xorstr_("Helmet"), (bool*)&Settings.b_ShowHelmet);
						ImGui::Checkbox(xorstr_("Body shield"), (bool*)&Settings.b_ShowBodyShield);
						ImGui::Checkbox(xorstr_("Knockdown shield"), (bool*)&Settings.b_ShowKnockdownShield);
						ImGui::Checkbox(xorstr_("Ammo"), (bool*)&Settings.b_ShowAmmo);
						ImGui::Checkbox(xorstr_("Optics"), (bool*)&Settings.b_ShowOptics);
						ImGui::Checkbox(xorstr_("Grenades"), (bool*)&Settings.b_ShowGrenades);
					}
					if (SubTab == 2)
					{
						ImGui::Checkbox(xorstr_("Active"), (bool*)&Settings.b_ShowRadar);
						ImGui::SliderInt2(xorstr_("Radar position"), Settings.RadarPos, 0, 4000);
						ImGui::SliderFloat(xorstr_("Radar range"), (float*)&Settings.f_RadarRange, 0, 1000);
					}
				}
				if (tab == 1)
				{
					ImGui::Checkbox(xorstr_("Active"), (bool*)&Settings.b_AimBot);
					if (Settings.i_AimKey)
					{
						ZeroMemory(AimKeyLabel, sizeof(AimKeyLabel));
						itoa(Settings.i_AimKey, NumberAimKey, 10);
						strcat(AimKeyLabel, "AimKey: ");
						strcat(AimKeyLabel, NumberAimKey);
					}
					ImGui::SameLine();
					if (ImGui::Button(AimKeyLabel))
					{
						Settings.i_AimKey = 0;
						strcpy(AimKeyLabel, "...");
					}
					ImGui::Combo(xorstr_("Aim bone"), (int*)&Settings.i_AimBone, AimBones, IM_ARRAYSIZE(AimBones));
					//ImGui::SliderInt(xorstr_("Aim Bone"), (int*)&Settings.i_AimBone, 0, 1);
					ImGui::SliderFloat(xorstr_("FOV"), (float*)&Settings.fov, 0.0f, 50.0f);
					ImGui::SliderFloat(xorstr_("Distance"), (float*)&Settings.f_AimDistance, 0.0f, 1000.0f);
					//ImGui::SliderFloat(xorstr_("Offset"), (float*)&Settings.offsetAim, 0.0f, 20.0f);
					ImGui::SliderFloat(xorstr_("Smooth"), (float*)&Settings.f_Smooth, 0.f, 100.f);
					ImGui::Checkbox(xorstr_("Draw FOV"), (bool*)&Settings.b_DrawFOV);
					ImGui::Checkbox(xorstr_("Prediction"), (bool*)&Settings.b_Prediction);
					//ImGui::Checkbox(xorstr_("Breath compensation"), (bool*)&Settings.b_RemoveBreath);
					ImGui::Checkbox(xorstr_("Aim at knocked players"), (bool*)&Settings.b_AimAtknocked);
					ImGui::Checkbox(xorstr_("Highlight aim target"), (bool*)&Settings.b_HighlightAimTarget);
					ImGui::Checkbox(xorstr_("RCS"), (bool*)&Settings.b_RCS);
				}
				if (tab == 2)
				{
					ImGui::Checkbox(xorstr_("Instant Bullet"), (bool*)&Settings.b_InstantHit);
					ImGui::Checkbox(xorstr_("Bhop"), (bool*)&Settings.b_Bhop);
					ImGui::Checkbox(xorstr_("NoSpread"), (bool*)&Settings.b_RemoveSpread);
					ImGui::SliderFloat(xorstr_("Spread factor"), (float*)&Settings.SpreadFactor, -10.0f, 0.0f);
					/*ImGui::Checkbox(xorstr_("Speed Hack"), (bool*)&Settings.b_SpeedHack);
					if (Settings.i_SpeedKey)
					{
						ZeroMemory(SpeedKeyLabel, sizeof(SpeedKeyLabel));
						itoa(Settings.i_SpeedKey, NumberSpeedKey, 10);
						strcat(SpeedKeyLabel, xorstr_("SpeedKey: "));
						strcat(SpeedKeyLabel, NumberSpeedKey);
					}
					ImGui::SameLine();
					if (ImGui::Button(SpeedKeyLabel))
					{
						Settings.i_SpeedKey = 0;
						strcpy(SpeedKeyLabel, "...");
					}
					ImGui::SliderFloat(xorstr_("Speed Factor"), (float*)&Settings.f_SpeedFactor, 0.0f, 6.0f);*/
					if (ImGui::Button(xorstr_("Save cfg"),ImVec2(245,25)))
					{
						if (!WriteCfg(&Settings))
						{
							AddNotifity(xorstr_("Error at saving cfg!"));
						}
						else
						{
							AddNotifity(xorstr_("Cfg successfully saved!"));
						}
					}
					ImGui::SameLine();
					if (ImGui::Button(xorstr_("Load cfg"), ImVec2(245, 25)))
					{
						if(!ReadCfg(&Settings))
						{
							AddNotifity(xorstr_("Saved cfg not found!"));
						}
						else
						{
							AddNotifity(xorstr_("Cfg successfully loaded!"));
						}
					}
					if (ImGui::Button(xorstr_("Unload")))
					{
						ExitProcess(0);
					}
				}
				if (tab == 3)
				{
					ImGui::Combo(xorstr_("Style menu"), (int*)&Settings.StyleMenu, StyleMenu, IM_ARRAYSIZE(StyleMenu));
					ImGui::SliderFloat(xorstr_("Box thickness"), (float*)&Settings.f_BoxThickness, 1.f, 10.f);
					ImGui::SliderFloat(xorstr_("Shield line thickness"), (float*)&Settings.f_ShieldLineThickness, 1.f, 10.f);
					ImGui::SliderFloat(xorstr_("Health line thickness"), (float*)&Settings.f_HealthLineThickness, 1.f, 10.f);
					ImGui::SliderFloat(xorstr_("Box corners rounding"), (float*)&Settings.f_BoxCornersRounding, 0.f, 20.f);
					ImGui::SliderFloat(xorstr_("Box rounding"), (float*)&Settings.f_BoxRounding, 5.f, 20.f);
					ImGui::SliderFloat(xorstr_("Snaplines thickness"), (float*)&Settings.f_SnaplinesThickness, 1.f, 10.f);
					float TempCol[3]{ Settings.BoxColor.x,Settings.BoxColor.y,Settings.BoxColor.z };
					if (ImGui::ColorEdit3(xorstr_("Box color"), TempCol))
					{
						Settings.BoxColor.x = TempCol[0];
						Settings.BoxColor.y = TempCol[1];
						Settings.BoxColor.z = TempCol[2];
					}
					float TempCol1[3]{ Settings.SkeletonColor.x,Settings.SkeletonColor.y,Settings.SkeletonColor.z };
					if (ImGui::ColorEdit3(xorstr_("Skeleton color"), TempCol1))
					{
						Settings.SkeletonColor.x = TempCol1[0];
						Settings.SkeletonColor.y = TempCol1[1];
						Settings.SkeletonColor.z = TempCol1[2];
					}
					float TempCol2[3]{ Settings.SnaplinesColor.x,Settings.SnaplinesColor.y,Settings.SnaplinesColor.z };
					if (ImGui::ColorEdit3(xorstr_("Snaplines color"), TempCol2))
					{
						Settings.SnaplinesColor.x = TempCol2[0];
						Settings.SnaplinesColor.y = TempCol2[1];
						Settings.SnaplinesColor.z = TempCol2[2];
					}

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
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		DrawNotifications();
		float color[] = { 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
		C_Vec localViewAngle;
		VMatrix Matrix;
		if (Settings.b_ShowRadar)
		{
			ImVec2 DrawSize(300, 300);
			DriverObj.ReadMemory(((BYTE*)Utils.LocalPlayer + ViewAngle), &localViewAngle, sizeof(C_Vec));

			draw_list->AddLine(
				ImVec2(Settings.RadarPos[0] + DrawSize.x / 2.f, Settings.RadarPos[1]),
				ImVec2(Settings.RadarPos[0] + DrawSize.x / 2.f, Settings.RadarPos[1] + DrawSize.y),
				ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));
			draw_list->AddLine(
				ImVec2(Settings.RadarPos[0], Settings.RadarPos[1] + DrawSize.y / 2.f),
				ImVec2(Settings.RadarPos[0] + DrawSize.x, Settings.RadarPos[1] + DrawSize.y / 2.f),
				ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));
		}
		if (Settings.b_BoxESP)
		{
			Matrix = Entity.GetView(&DriverObj, LocalView);
			for (int i = 0; i < Utils.CountPlayers; i++)
			{
				C_Vec pos, pos3d, top, top3d;
				if (PlayerList[i].b_VisibleRadar != NULL || PlayerList[i].b_VisibleBox != NULL)
				{
					DriverObj.ReadMemory(((BYTE*)PlayerList[i].Address + m_vecAbsOrigin), &pos3d, sizeof(C_Vec));
					if (Settings.b_ShowRadar)
					{
						Radar.DrawPlayer(pos3d, localViewAngle, LocalView.m_Pos);
					}
				}
				if (PlayerList[i].b_VisibleBox != NULL)
				{
					top3d = pos3d + C_Vec(0, 0, 64);
					if (WorldToScreen(pos3d, pos,Matrix) && WorldToScreen(top3d, top,Matrix))
					{
						int height = (pos.y - top.y);
						int width = height / 2;
						if (Settings.TypeBox == 0)
						{
							WORD VisibleFlag = 0;
							DriverObj.ReadMemory(((BYTE*)PlayerList[i].Address + 0x300), &VisibleFlag, sizeof(WORD));
							if (!VisibleFlag)
							{
								draw_list->AddRect(ImVec2((pos.x - width / 2), top.y), ImVec2((pos.x - width / 2) + width, top.y + height),
									ImGui::GetColorU32(Settings.BoxColor), Settings.f_BoxRounding, Settings.f_BoxCornersRounding, Settings.f_BoxThickness);
							}
							else
							{
								draw_list->AddRect(ImVec2((pos.x - width / 2), top.y), ImVec2((pos.x - width / 2) + width, top.y + height),
									ImGui::GetColorU32(ImVec4(0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f)), Settings.f_BoxRounding, Settings.f_BoxCornersRounding, Settings.f_BoxThickness);
							}
						}
						else if(Settings.TypeBox == 1)
						{
							//left down
							draw_list->AddLine(ImVec2(pos.x - width / 2, pos.y), ImVec2(pos.x - width / 4, pos.y),
								ImGui::GetColorU32(Settings.BoxColor), Settings.f_BoxThickness);
							
							draw_list->AddLine(ImVec2(pos.x - width / 2, pos.y), ImVec2(pos.x - width / 2, pos.y - height / 3),
								ImGui::GetColorU32(Settings.BoxColor), Settings.f_BoxThickness);
							//rigth down
							draw_list->AddLine(ImVec2(pos.x + width / 2, pos.y), ImVec2(pos.x + width / 4, pos.y),
								ImGui::GetColorU32(Settings.BoxColor), Settings.f_BoxThickness);

							draw_list->AddLine(ImVec2(pos.x + width / 2, pos.y), ImVec2(pos.x + width / 2, pos.y - height / 3),
								ImGui::GetColorU32(Settings.BoxColor), Settings.f_BoxThickness);
							//left up
							draw_list->AddLine(ImVec2(pos.x - width / 2, top.y), ImVec2(pos.x - width / 4, top.y),
								ImGui::GetColorU32(Settings.BoxColor), Settings.f_BoxThickness);

							draw_list->AddLine(ImVec2(pos.x - width / 2, top.y), ImVec2(pos.x - width / 2, top.y + height / 3),
								ImGui::GetColorU32(Settings.BoxColor), Settings.f_BoxThickness);
							//right up
							draw_list->AddLine(ImVec2(pos.x + width / 2, top.y), ImVec2(pos.x + width / 4, top.y),
								ImGui::GetColorU32(Settings.BoxColor), Settings.f_BoxThickness);

							draw_list->AddLine(ImVec2(pos.x + width / 2, top.y), ImVec2(pos.x + width / 2, top.y + height / 3),
								ImGui::GetColorU32(Settings.BoxColor), Settings.f_BoxThickness);
						}
						else if (Settings.TypeBox == 2)
						{
							PVOID pEntityCollision = 0;
							C_Vec MinCollision, MaxCollision;
							DriverObj.ReadMemory(((BYTE*)PlayerList[i].Address + m_Collision + m_vecMins), &MinCollision, sizeof(C_Vec));
							DriverObj.ReadMemory(((BYTE*)PlayerList[i].Address + m_Collision + m_vecMaxs), &MaxCollision, sizeof(C_Vec));
							C_Vec corners[8] =
							{
								C_Vec(MinCollision.x,MinCollision.y,MinCollision.z),
								C_Vec(MinCollision.x,MaxCollision.y,MinCollision.z),
								C_Vec(MaxCollision.x,MaxCollision.y,MinCollision.z),
								C_Vec(MaxCollision.x,MinCollision.y,MinCollision.z),
								C_Vec(MinCollision.x,MinCollision.y,MaxCollision.z),
								C_Vec(MinCollision.x,MaxCollision.y,MaxCollision.z),
								C_Vec(MaxCollision.x,MaxCollision.y,MaxCollision.z),
								C_Vec(MaxCollision.x,MinCollision.y,MaxCollision.z)
							};
							C_Vec EntityViewAngle;
							DriverObj.ReadMemory(((BYTE*)PlayerList[i].Address + m_localAngles), &EntityViewAngle, sizeof(C_Vec));
							float ang = EntityViewAngle.y;

							for (int i = 0; i <= 7; i++)
								corners[i].Rotate2D(ang);

							C_Vec _corners[8];
							for (int i = 0; i <= 7; i++)
								WorldToScreen(pos3d + corners[i], _corners[i],Matrix);

							int x1 = Utils.Width * 2, y1 = Utils.Height * 2, x2 = -Utils.Width, y2 = -Utils.Height;

							for (int i = 0; i <= 7; i++)
							{
								x1 = (int)(min(x1, _corners[i].x));
								y1 = (int)(min(y1, _corners[i].y));
								x2 = (int)(max(x2, _corners[i].x));
								y2 = (int)(max(y2, _corners[i].y));
							}

							int diff = x2 - x1;
							int diff2 = y2 - y1;

							for (int i = 1; i <= 4; i++)
							{
								draw_list->AddLine(ImVec2(_corners[i - 1].x, _corners[i - 1].y), ImVec2(_corners[i % 4].x, _corners[i % 4].y),
									ImGui::GetColorU32(Settings.BoxColor), Settings.f_BoxThickness);                   
								draw_list->AddLine(ImVec2(_corners[i - 1].x, _corners[i - 1].y), ImVec2(_corners[i + 3].x, _corners[i + 3].y),
									ImGui::GetColorU32(Settings.BoxColor), Settings.f_BoxThickness);
								draw_list->AddLine(ImVec2(_corners[i + 3].x, _corners[i + 3].y), ImVec2(_corners[i % 4 + 4].x, _corners[i % 4 + 4].y),
									ImGui::GetColorU32(Settings.BoxColor), Settings.f_BoxThickness);
							}
						}
						if (Settings.b_ShowHealth)
						{
							//? Background
							draw_list->AddRect(ImVec2((pos.x - width / 2) - 4, pos.y),
								ImVec2((pos.x - width / 2) - 4, pos.y - height),
								ImGui::GetColorU32(ImVec4(0.501960814f, 0.501960814f, 0.501960814f, 0.500000000f)), 1, 0, Settings.f_HealthLineThickness);
							//////////////
							draw_list->AddRect(ImVec2((pos.x - width / 2) - 4, pos.y),
								ImVec2((pos.x - width / 2) - 4, pos.y - Entity.GetHealth(&DriverObj, PlayerList[i].Address) * height / 100),
								ImGui::GetColorU32(ImVec4(0 / 255.f, 255 / 255.f, 47 / 255.f, 1)), 1, 0, Settings.f_HealthLineThickness);
							//Edgy
							for (int i = 0; i < 10; i++)
							{
								draw_list->AddLine(ImVec2((pos.x - width / 2) - 4, (pos.y - ((height / 10.f) * i))),
									ImVec2((pos.x - width / 2) - 4 - Settings.f_HealthLineThickness, (pos.y - ((height / 10.f) * i))), ImGui::GetColorU32(ImVec4(0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f)), Settings.f_HealthLineThickness);
							}
						}
						if (Settings.b_ShowShield)
						{
							//? Background 
							draw_list->AddRect(ImVec2((pos.x - width / 2) + 4 + width, pos.y),
								ImVec2((pos.x - width / 2) + 4 + width, pos.y - height),
								ImGui::GetColorU32(ImVec4(0.501960814f, 0.501960814f, 0.501960814f, 0.500000000f)), 1, 0, Settings.f_ShieldLineThickness);
							//////////////
							draw_list->AddRect(ImVec2((pos.x - width / 2) + 4 + width, pos.y),
								ImVec2((pos.x - width / 2) + 4 + width, pos.y - Entity.GetShield(&DriverObj, PlayerList[i].Address) * height / 100),
								ImGui::GetColorU32(ImVec4(0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f)), 1, 0, Settings.f_ShieldLineThickness);
							//Edgy
							for (int i = 0; i < 10; i++)
							{
								draw_list->AddLine(ImVec2((pos.x - width / 2) + 4, (pos.y - ((height / 10.f) * i))),
									ImVec2((pos.x - width / 2) + 4 - Settings.f_ShieldLineThickness, (pos.y - ((height / 10.f) * i))), ImGui::GetColorU32(ImVec4(0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f)), Settings.f_ShieldLineThickness);
							}
						}
						if (Settings.b_ShowDistance)
						{
							char Buff[128]{ 0 };
							sprintf(Buff, "%i", (int)round(LocalView.m_Pos.distance(pos3d) / 100));
							draw_list->AddText(ImVec2((pos.x), pos.y + 1), ImGui::GetColorU32(ImVec4(0.000000000f, 1.000000000f, 0.000000000f, 1.000000000f)), Buff);
						}
						if (Settings.b_ShowSnapline)
						{
							if (Settings.b_HighlightAimTarget && !Settings.b_GlowEsp && Utils.TargetEntitty.Address == PlayerList[i].Address)
							{
								draw_list->AddLine(ImVec2(Utils.Width / 2, Utils.Height / 2), ImVec2(pos.x, pos.y),
									ImGui::GetColorU32(ImVec4(0.000000000f, 1.000000000f, 0.000000000f, 1.000000000f)), Settings.f_SnaplinesThickness);
							}
							else
							{
								draw_list->AddLine(ImVec2(Utils.Width / 2, Utils.Height / 2), ImVec2(pos.x, pos.y),
									ImGui::GetColorU32(Settings.SnaplinesColor), Settings.f_SnaplinesThickness);
							}
						}
						if (Settings.b_ShowSkeleton)
						{
							C_Vec BonePos, DrawBonePos, c_HeadPos, NeckPos, l_shoulder, r_shoulder, c_pelvis, l_foot, r_foot, r_thigh, l_thigh, r_knee, l_knee, r_elbow, l_elbow, r_wrist, l_wrist;
#pragma region oldBone
							/*Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].BoneMatrixPtr, bone_id::c_head);
							BonePos += pos3d;
							LocalView.WorldToScreen(BonePos, DrawBonePos);
							c_HeadPos = DrawBonePos;
							/////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].BoneMatrixPtr, bone_id::c_neckA);
							BonePos += pos3d;
							LocalView.WorldToScreen(BonePos, DrawBonePos);
							NeckPos = DrawBonePos;
							/////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].BoneMatrixPtr, bone_id::r_shoulder);
							BonePos += pos3d;
							LocalView.WorldToScreen(BonePos, DrawBonePos);
							r_shoulder = DrawBonePos;
							/////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].BoneMatrixPtr, bone_id::l_shoulderTwist);
							BonePos += pos3d;
							LocalView.WorldToScreen(BonePos, DrawBonePos);
							l_shoulder = DrawBonePos;
							/////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].BoneMatrixPtr, bone_id::c_hip);
							BonePos += pos3d;
							LocalView.WorldToScreen(BonePos, DrawBonePos);
							c_pelvis = DrawBonePos;
							////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].BoneMatrixPtr, bone_id::l_thigh);
							BonePos += pos3d;
							LocalView.WorldToScreen(BonePos, DrawBonePos);
							l_thigh = DrawBonePos;
							////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].BoneMatrixPtr, bone_id::r_thigh);
							BonePos += pos3d;
							LocalView.WorldToScreen(BonePos, DrawBonePos);
							r_thigh = DrawBonePos;
							////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].BoneMatrixPtr, bone_id::r_kneeB);
							BonePos += pos3d;
							LocalView.WorldToScreen(BonePos, DrawBonePos);
							r_knee = DrawBonePos;
							////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].BoneMatrixPtr, bone_id::l_kneeB);
							BonePos += pos3d;
							LocalView.WorldToScreen(BonePos, DrawBonePos);
							l_knee = DrawBonePos;
							////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].BoneMatrixPtr, bone_id::l_ankle);
							BonePos += pos3d;
							LocalView.WorldToScreen(BonePos, DrawBonePos);
							l_foot = DrawBonePos;
							///////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].BoneMatrixPtr, bone_id::r_ball);
							BonePos += pos3d;
							LocalView.WorldToScreen(BonePos, DrawBonePos);
							r_foot = DrawBonePos;*/
#pragma endregion
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].pBonePos[0]);
							BonePos += pos3d;
							WorldToScreen(BonePos, DrawBonePos,Matrix);
							c_HeadPos = DrawBonePos;
							/////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].pBonePos[1]);
							BonePos += pos3d;
							WorldToScreen(BonePos, DrawBonePos, Matrix);
							NeckPos = DrawBonePos;
							/////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].pBonePos[6]);
							BonePos += pos3d;
							WorldToScreen(BonePos, DrawBonePos, Matrix);
							r_shoulder = DrawBonePos;
							/////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].pBonePos[3]);
							BonePos += pos3d;
							WorldToScreen(BonePos, DrawBonePos, Matrix);
							l_shoulder = DrawBonePos;
							/////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].pBonePos[7]);//? ////////
							BonePos += pos3d;
							WorldToScreen(BonePos, DrawBonePos, Matrix);
							r_elbow = DrawBonePos;
							/////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].pBonePos[4]);
							BonePos += pos3d;
							WorldToScreen(BonePos, DrawBonePos, Matrix);
							l_elbow = DrawBonePos;
							/////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].pBonePos[8]);
							BonePos += pos3d;
							WorldToScreen(BonePos, DrawBonePos, Matrix);
							r_wrist = DrawBonePos;
							/////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].pBonePos[5]);
							BonePos += pos3d;
							WorldToScreen(BonePos, DrawBonePos, Matrix);
							l_wrist = DrawBonePos;
							/////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].pBonePos[2]);
							BonePos += pos3d;
							WorldToScreen(BonePos, DrawBonePos, Matrix);
							c_pelvis = DrawBonePos;
							////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].pBonePos[9]);
							BonePos += pos3d;
							WorldToScreen(BonePos, DrawBonePos, Matrix);
							l_thigh = DrawBonePos;
							////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].pBonePos[12]);
							BonePos += pos3d;
							WorldToScreen(BonePos, DrawBonePos, Matrix);
							r_thigh = DrawBonePos;
							////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].pBonePos[13]);
							BonePos += pos3d;
							WorldToScreen(BonePos, DrawBonePos, Matrix);
							r_knee = DrawBonePos;
							////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].pBonePos[10]);
							BonePos += pos3d;
							WorldToScreen(BonePos, DrawBonePos, Matrix);
							l_knee = DrawBonePos;
							////////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].pBonePos[11]);
							BonePos += pos3d;
							WorldToScreen(BonePos, DrawBonePos, Matrix);
							l_foot = DrawBonePos;
							///////////////////
							Entity.GetBonePos(&DriverObj, BonePos, PlayerList[i].pBonePos[14]);
							BonePos += pos3d;
							WorldToScreen(BonePos, DrawBonePos, Matrix);
							r_foot = DrawBonePos;
							draw_list->AddCircle(ImVec2(c_HeadPos.x, c_HeadPos.y), c_HeadPos.distance(NeckPos) / 1.5, ImGui::GetColorU32(Settings.SkeletonColor), 100);
							draw_list->AddLine(ImVec2(c_HeadPos.x, c_HeadPos.y), ImVec2(NeckPos.x, NeckPos.y), ImGui::GetColorU32(Settings.SkeletonColor));

							draw_list->AddLine(ImVec2(NeckPos.x, NeckPos.y), ImVec2(r_shoulder.x, r_shoulder.y), ImGui::GetColorU32(Settings.SkeletonColor));
							draw_list->AddLine(ImVec2(NeckPos.x, NeckPos.y), ImVec2(l_shoulder.x, l_shoulder.y), ImGui::GetColorU32(Settings.SkeletonColor));

							draw_list->AddLine(ImVec2(r_shoulder.x, r_shoulder.y), ImVec2(r_elbow.x, r_elbow.y), ImGui::GetColorU32(Settings.SkeletonColor));
							draw_list->AddLine(ImVec2(l_shoulder.x, l_shoulder.y), ImVec2(l_elbow.x, l_elbow.y), ImGui::GetColorU32(Settings.SkeletonColor));

							draw_list->AddLine(ImVec2(r_elbow.x, r_elbow.y), ImVec2(r_wrist.x, r_wrist.y), ImGui::GetColorU32(Settings.SkeletonColor));
							draw_list->AddLine(ImVec2(l_elbow.x, l_elbow.y), ImVec2(l_wrist.x, l_wrist.y), ImGui::GetColorU32(Settings.SkeletonColor));

							draw_list->AddLine(ImVec2(NeckPos.x, NeckPos.y), ImVec2(c_pelvis.x, c_pelvis.y), ImGui::GetColorU32(Settings.SkeletonColor));

							draw_list->AddLine(ImVec2(c_pelvis.x, c_pelvis.y), ImVec2(r_thigh.x, r_thigh.y), ImGui::GetColorU32(Settings.SkeletonColor));
							draw_list->AddLine(ImVec2(c_pelvis.x, c_pelvis.y), ImVec2(l_thigh.x, l_thigh.y), ImGui::GetColorU32(Settings.SkeletonColor));

							draw_list->AddLine(ImVec2(r_thigh.x, r_thigh.y), ImVec2(r_knee.x, r_knee.y), ImGui::GetColorU32(Settings.SkeletonColor));
							draw_list->AddLine(ImVec2(l_thigh.x, l_thigh.y), ImVec2(l_knee.x, l_knee.y), ImGui::GetColorU32(Settings.SkeletonColor));

							draw_list->AddLine(ImVec2(r_knee.x, r_knee.y), ImVec2(r_foot.x, r_foot.y), ImGui::GetColorU32(Settings.SkeletonColor));
							draw_list->AddLine(ImVec2(l_knee.x, l_knee.y), ImVec2(l_foot.x, l_foot.y), ImGui::GetColorU32(Settings.SkeletonColor));
						}
					}
				}
			}
		}
#pragma region LootESP
		if (Settings.b_ShowMedAndArmor || Settings.b_ShowWeapon || Settings.b_ShowBackpacks || Settings.b_ShowAmmo ||  Settings.b_ShowBodyShield || Settings.b_ShowHelmet || Settings.b_ShowKnockdownShield)
		{
			if (Utils.IsInGame)
			{
				if (!LocalView.m_Pos.x && !LocalView.m_Pos.y)
					Entity.GetView(&DriverObj, LocalView);
				for (int i = 0; i < Utils.CountItem; i++)
				{
					C_Vec pos, pos3d, top, top3d;
					PVOID ItemAddr = ItemList[i].Address;
					if (ItemAddr)
					{
						DriverObj.ReadMemory(((BYTE*)ItemAddr + m_vecAbsOrigin), &pos3d, sizeof(C_Vec));
						top3d = pos3d + C_Vec(0, 0, 64);
						if (WorldToScreen(pos3d, pos,Matrix) && WorldToScreen(top3d, top,Matrix))
						{
							C_Vec ItemPos;
							int ItemId = 0;
							ImVec4 ItemColor = { 0,0,0,0 };
							ImVec2 ItemSize = { 0,0 };
							ID3D11ShaderResourceView* pTexture = NULL;
							DriverObj.ReadMemory(((BYTE*)ItemAddr + m_customScriptInt), &ItemId, sizeof(int));
							if (Settings.b_ShowMedAndArmor)
							{
								if (ItemId == 40)
								{
									pTexture = pIBuf_Accelerant;
									ItemSize = { Utils.LongCommonItemsSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 41)
								{

									pTexture = pIBuf_Phoenix_Kit;
									ItemSize = { Utils.CommonItemsSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 42)
								{
									pTexture = pIBuf_Health_Main_Large;
									ItemSize = { Utils.LongCommonItemsSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 43)
								{
									pTexture = pIBuf_Health_Main_Small;
									ItemSize = { Utils.LongCommonItemsSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 44)
								{
									pTexture = pIBuf_Shield_Battery_Large;
									ItemSize = { Utils.CommonItemsSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 45)
								{
									pTexture = pIBuf_Shield_Battery_Small;
									ItemSize = { Utils.CommonItemsSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
							}
							if (Settings.b_ShowBackpacks)
							{
								if (Settings.b_LvlLoot[0])
								{
									if (ItemId == 58)//Backpack lvl_1
									{
										pTexture = pIBuf_Backpack_Light;
										ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
										ItemSize = { Utils.BackpacksIconSize };
									}
								}
								if (Settings.b_LvlLoot[1])
								{
									if (ItemId == 59)//Backpack lvl_2
									{
										pTexture = pIBuf_Backpack_Medium;
										ItemColor = { 0.678431392f, 0.847058892f, 0.901960850f, 1.000000000f };
										ItemSize = { Utils.BackpacksIconSize };
									}
								}
								if (Settings.b_LvlLoot[2])
								{
									if (ItemId == 60)//Backpack lvl_3
									{
										pTexture = pIBuf_Backpack_Heavy;
										ItemColor = { 1.000000000f, 0.752941251f, 0.796078503f, 1.000000000f };
										ItemSize = { Utils.BackpacksIconSize };
									}
								}
								if (Settings.b_LvlLoot[3])
								{
									if (ItemId == 61)//Backpack lvl_4
									{
										pTexture = pIBuf_Backpack_Heavy;
										ItemColor = { 1.000000000f, 0.843137324f, 0.000000000f, 1.000000000f };
										ItemSize = { Utils.BackpacksIconSize };
									}
								}
							}
							if (Settings.b_ShowWeapon)
							{
								/*if (ItemId == 0)//Kraber
								{
									pTexture = pIBuf_At_Rifle;
									ItemSize = { Utils.LongWeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}*/
								/*case 1)//"Mastiff"
									pTexture = "Mastiff";
								/*case 2)//"At_Rifle2"
									pTexture = "At_Rifle2";*/
								if (ItemId == 1)//Kraber
								{
									pTexture = pIBuf_At_Rifle;
									ItemSize = { Utils.LongWeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 2)//Mastiff
								{
									pTexture = pIBuf_Mastiff_Stgn;
									ItemSize = { Utils.WeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 4)//"Havoc"
								{
									pTexture = pIBuf_Havoc;
									ItemSize = { Utils.LongWeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								/*case 4)//At_Rifle4"
									pTexture = "At_Rifle4";*/
								if (ItemId == 6)//"Преданность"
								{
									pTexture = pIBuf_Hemlock;
									ItemSize = { Utils.LongWeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								/*case 6)
									pTexture = "At_Rifle6";*/
								if (ItemId == 8)//"Tripletake"
								{
									pTexture = pIBuf_Doubletake;
									ItemSize = { Utils.LongWeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 10)
								{
									pTexture = pIBuf_Hemlock;
									ItemSize = { Utils.LongWeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 13)//"Scout-G7"
								{
									pTexture = pIBuf_G2;
									ItemSize = { Utils.LongWeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 15)
								{
									pTexture = pIBuf_Alternator_Smg;
									ItemSize = { Utils.WeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 17)//"R-99"
								{
									pTexture = pIBuf_R97;
									ItemSize = { Utils.LongWeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 13)
								{
									pTexture = pIBuf_Prowler_Smg;
									ItemSize = { Utils.WeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 20)// "Longbow"
								{
									pTexture = pIBuf_Rspn101_Dmr;
									ItemSize = { Utils.LongWeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 17)
								{
									pTexture = pIBuf_Lmg_Hemlok;
									ItemSize = { Utils.WeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 23)// "R-301"
								{
									pTexture = pIBuf_Rspn101;
									ItemSize = { Utils.LongWeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 25)// "EVA-8"
								{
									pTexture = pIBuf_W1128;
									ItemSize = { Utils.LongWeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 26)// "Peacekeeper"
								{
									pTexture = pIBuf_Peacekeeper;
									ItemSize = { Utils.LongWeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 28)
								{
									pTexture = pIBuf_Pstl_Sa3;
									ItemSize = { Utils.WeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 30)//"Wingman"
								{
									pTexture = pIBuf_B3wing;
									ItemSize = { Utils.WeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 32)
								{
									pTexture = pIBuf_P2011;
									ItemSize = { Utils.WeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 34)
								{
									pTexture = pIBuf_P2011_Auto;//RE-45
									ItemSize = { Utils.WeaponIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
							}
							if (Settings.b_ShowKnockdownShield)
							{
								if (Settings.b_LvlLoot[0])
								{
									if (ItemId == 54)
									{
										ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
										ItemSize = { Utils.ArmorIconSize };
										pTexture = pIBuf_Shield_Down;//lvl1
									}
								}
								if (Settings.b_LvlLoot[1])
								{
									if (ItemId == 55)
									{
										ItemColor = { 0.678431392f, 0.847058892f, 0.901960850f, 1.000000000f };
										ItemSize = { Utils.ArmorIconSize };
										pTexture = pIBuf_Shield_Down;//lvl2
									}
								}
								if (Settings.b_LvlLoot[2])
								{
									if (ItemId == 56)
									{
										ItemColor = { 1.000000000f, 0.752941251f, 0.796078503f, 1.000000000f };
										ItemSize = { Utils.ArmorIconSize };
										pTexture = pIBuf_Shield_Down;//lvl3
									}
								}
								if (Settings.b_LvlLoot[3])
								{
									if (ItemId == 57)
									{
										ItemColor = { 1.000000000f, 0.843137324f, 0.000000000f, 1.000000000f };
										ItemSize = { Utils.ArmorIconSize };
										pTexture = pIBuf_Shield_Down;//lvl4
									}
								}
							}
							if (Settings.b_ShowHelmet)
							{
								if (Settings.b_LvlLoot[0])
								{
									if (ItemId == 46)// helmet lvl 1
									{
										pTexture = pIBuf_Shield_Upgrade_Head;
										ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
										ItemSize = { Utils.ArmorIconSize };
									}
								}
								if (Settings.b_LvlLoot[1])
								{
									if (ItemId == 47)// helmet lvl 2
									{
										pTexture = pIBuf_Shield_Upgrade_Head;
										ItemColor = { 0.678431392f, 0.847058892f, 0.901960850f, 1.000000000f };
										ItemSize = { Utils.ArmorIconSize };
									}
								}
								if (Settings.b_LvlLoot[2])
								{
									if (ItemId == 48)// helmet lvl 3
									{
										pTexture = pIBuf_Shield_Upgrade_Head;
										ItemColor = { 1.000000000f, 0.752941251f, 0.796078503f, 1.000000000f };
										ItemSize = { Utils.ArmorIconSize };
									}
								}
								if (Settings.b_LvlLoot[3])
								{
									if (ItemId == 49)// helmet lvl 4
									{
										pTexture = pIBuf_Shield_Upgrade_Head;
										ItemColor = { 1.000000000f, 0.843137324f, 0.000000000f, 1.000000000f };
										ItemSize = { Utils.ArmorIconSize };
									}
								}
							}
							if (Settings.b_ShowBodyShield)
							{
								if (Settings.b_LvlLoot[0])
								{
									if (ItemId == 50)// shield lvl 1
									{
										pTexture = pIBuf_Shield_Upgrade_Body;
										ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
										ItemSize = { Utils.ArmorIconSize };
									}
								}

								if (Settings.b_LvlLoot[1])
								{
									if (ItemId == 51)// shield lvl 2
									{
										pTexture = pIBuf_Shield_Upgrade_Body;
										ItemColor = { 0.678431392f, 0.847058892f, 0.901960850f, 1.000000000f };
										ItemSize = { Utils.ArmorIconSize };
									}
								}
								if (Settings.b_LvlLoot[2])
								{
									if (ItemId == 52)
									{// shield lvl 3
										pTexture = pIBuf_Shield_Upgrade_Body;
										ItemColor = { 1.000000000f, 0.752941251f, 0.796078503f, 1.000000000f };
										ItemSize = { Utils.ArmorIconSize };
									}
								}
								if (Settings.b_LvlLoot[3])
								{
									if (ItemId == 53)// shield lvl 4
									{
										pTexture = pIBuf_Shield_Upgrade_Body;
										ItemColor = { 1.000000000f, 0.843137324f, 0.000000000f, 1.000000000f };
										ItemSize = { Utils.ArmorIconSize };
									}
								}
							}
							if (Settings.b_ShowAmmo)
							{
								if (ItemId == 36)
								{
									pTexture = pIBuf_Sc;
									ItemSize = { Utils.AmmoIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 37)
								{
									pTexture = pIBuf_Energy;
									ItemSize = { Utils.AmmoIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 38)
								{
									pTexture = pIBuf_Shg;
									ItemSize = { Utils.AmmoIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 39)
								{
									pTexture = pIBuf_Hc;
									ItemSize = { Utils.AmmoIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
							}
							if (Settings.b_ShowOptics)
							{
								if (Settings.b_LvlLoot[0])//Common
								{
									if (ItemId == 65)
									{
										pTexture = pIBuf_Optic_Cq_Holo_Var;
										ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
										ItemSize = { Utils.LongCommonItemsSize };
									}
									else if (ItemId == 67)
									{
										pTexture = pIBuf_Optic_Cq_Hcog_R1;
										ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
										ItemSize = { Utils.LongCommonItemsSize };
									}
								}
								if (Settings.b_LvlLoot[1])//Rare
								{
									if (ItemId == 66)
									{
										pTexture = pIBuf_Optic_Cq_Holo_Var_2x;
										ItemColor = { 0.678431392f, 0.847058892f, 0.901960850f, 1.000000000f };
										ItemSize = { Utils.LongCommonItemsSize };
									}
									else if (ItemId == 68)
									{
										pTexture = pIBuf_Optic_Cq_Hcog_R2;
										ItemColor = { 0.678431392f, 0.847058892f, 0.901960850f, 1.000000000f };
										ItemSize = { Utils.LongCommonItemsSize };
									}
									else if (ItemId == 72)//6x scope
									{
										pTexture = pIBuf_Optic_Rng_Sni_Dcom;
										ItemColor = { 0.678431392f, 0.847058892f, 0.901960850f, 1.000000000f };
										ItemSize = { Utils.LongCommonItemsSize };
									}
								}
								if (Settings.b_LvlLoot[2])//Epic
								{
									if (ItemId == 70)
									{
										pTexture = pIBuf_Optic_Rng_Hcog_Acgs;
										ItemColor = { 1.000000000f, 0.752941251f, 0.796078503f, 1.000000000f };
										ItemSize = { Utils.LongCommonItemsSize };
									}
									else if (ItemId == 72)
									{
										pTexture = pIBuf_Optic_Rng_Aog_Var_R1;
										ItemColor = { 1.000000000f, 0.752941251f, 0.796078503f, 1.000000000f };
										ItemSize = { Utils.LongCommonItemsSize };
									}
									else if (ItemId == 73)//4x - 8x scope
									{
										pTexture = pIBuf_Optic_Rng_Sni_Var_Talon;
										ItemColor = { 1.000000000f, 0.752941251f, 0.796078503f, 1.000000000f };
										ItemSize = { Utils.LongCommonItemsSize };
									}
								}
								if (Settings.b_LvlLoot[3])//Legendary
								{
									if (ItemId == 69)
									{
										pTexture = pIBuf_Optic_Rng_Hcog_Acgs;
										ItemColor = { 1.000000000f, 0.843137324f, 0.000000000f, 1.000000000f };
										ItemSize = { Utils.LongCommonItemsSize };
									}
									else if (ItemId == 74)//4x-10x scope
									{
										pTexture = pIBuf_Optic_Cq_Threat;
										ItemColor = { 1.000000000f, 0.843137324f, 0.000000000f, 1.000000000f };
										ItemSize = { Utils.LongCommonItemsSize };
									}
								}
							}
							if (Settings.b_ShowGrenades)
							{
								if (ItemId == 62)
								{
									pTexture = pIBuf_Thermite_Grenade;
									ItemSize = { Utils.GrenadesIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 63)
								{
									pTexture = pIBuf_M20_Grenade;
									ItemSize = { Utils.GrenadesIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
								if (ItemId == 64)
								{
									pTexture = pIBuf_Shuriken;
									ItemSize = { Utils.GrenadesIconSize };
									ItemColor = { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f };
								}
							}
							draw_list->AddImage(pTexture, ImVec2(pos.x, pos.y), ImVec2(pos.x + ItemSize.x, pos.y + ItemSize.y), ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(ItemColor));
							if (Settings.b_ShowItemId)
							{
								char Buff[128]{ 0 };
								sprintf(Buff, "%i ", ItemId);
								draw_list->AddText(ImVec2(pos.x, pos.y - 1), ImGui::GetColorU32(ItemColor), Buff);
							}
						}
					}
				}
			}
		}
#pragma endregion
		if (Settings.b_DrawFOV)
		{
			draw_list->AddCircle(ImVec2(Utils.Width / 2, Utils.Height / 2), Settings.fov * 15, ImColor(255, 0, 0, 255), 100, 2.f);
		}
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);

		// Rendering
		ImGui::Render();
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		 g_pSwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync
	}
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	//Sleep(50000);
#pragma endregion
	}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	  CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MainFunc, NULL, NULL, NULL);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
