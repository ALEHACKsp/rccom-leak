#pragma once
#include "VMatrix.hpp"
#include "Offset.h"
#include <Windows.h>
#include <iostream>
#include <thread>
#include <algorithm>
#include <iomanip>
#include <random>
#include "Bones.h"
#include <DirectXMath.h>
#include "XorStr.h"
#include <xnamath.h>
#include <cstdint>
#include "imgui.h"
#include <algorithm>
#include "CFG.h"
#include "lazy_importer.hpp"
#define PI  3.14159265358979323846
using namespace std;

#define  STATUS_SUCCESS                    ((NTSTATUS)0x00000000L)
/*
#define IO_READ              CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_BUFFERED,FILE_SPECIAL_ACCESS)//? From user mod to kernel mod
#define IO_WRITE             CTL_CODE(FILE_DEVICE_UNKNOWN,0x802,METHOD_BUFFERED,FILE_SPECIAL_ACCESS)//? From user mod to kernel mod
#define IO_MMAP              CTL_CODE(FILE_DEVICE_UNKNOWN,0x803,METHOD_BUFFERED,FILE_SPECIAL_ACCESS)//? From user mod to kernel mod
#define IO_GETMODULEINFO     CTL_CODE(FILE_DEVICE_UNKNOWN,0x805,METHOD_BUFFERED,FILE_SPECIAL_ACCESS)//? From user mod to kernel mod
*/
#define IO_READ              0x1
#define IO_WRITE             0x2
#define IO_GETMODULEINFO     0x3
#define IO_SPOOF             0x4
#define IO_GETORIGSERIALNUM  0x5

class C_Vec
{
public:
	float x, y, z;
	C_Vec()
	{
		x = y = z = 0;
	}
	C_Vec(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	inline C_Vec& operator-=(const C_Vec& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	inline C_Vec& operator+=(const C_Vec& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	inline C_Vec& operator*=(const C_Vec& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	inline float distance(const C_Vec &vec) {
		return sqrt((x - vec.x) * (x - vec.x) + (y - vec.y) * (y - vec.y) + (z - vec.z) * (z - vec.z));
	}
	inline C_Vec operator-(void)
	{
		return C_Vec(-x, -y, -z);
	}
	inline C_Vec& operator/=(const C_Vec& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}
	inline C_Vec& operator/(const float v)
	{
		x /= v;
		y /= v;
		z /= v;
		return *this;
	}
	inline C_Vec operator-(const C_Vec v)
	{
		return C_Vec(x - v.x, y - v.y, z - v.z);
	}
	inline C_Vec operator+(const C_Vec& v)
	{
		return C_Vec(x + v.x, y + v.y, z + v.z);
	}
	inline C_Vec& operator=(const C_Vec& v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		return *this;
	}
	inline C_Vec& operator/=(float v)
	{
		this->x /= v;
		this->y /= v;
		this->z /= v;
		return *this;
	}
	inline C_Vec& operator*=(float v)
	{
		this->x *= v;
		this->y *= v;
		this->z *= v;
		return *this;
	}
	inline float DegreeToRadian(float degree)
	{
		return degree * (PI / 180);

	}
	inline C_Vec& RadianToDegree(C_Vec radians)
	{

		x = radians.x * (180 / PI);
		y = radians.y * (180 / PI);
		//degrees.z = radians.z * (180 / PI);
	}
	inline void SinCos(const float &r, float &s, float &c)
	{
		s = sin(r);
		c = cos(r);
	}
	inline C_Vec& Rotate2D(const float &f)
	{
		float _x, _y;

		float s, c;

		SinCos(DegreeToRadian(f), s, c);

		_x = x;
		_y = y;

		x = (_x * c) - (_y * s);
		y = (_x * s) + (_y * c);

		return *this;
	}
	inline C_Vec& ClampAng()
	{
		if (y > 180.0f)
			y = 180.0f;
		else if (y < -180.0f)
			y = -180.0f;

		if (x > 89.0f)
			x = 89.0f;
		else if (x < -89.0f)
			x = -89.0f;

		z = 0;
		return *this;
	}
	inline C_Vec& AngleNormalize()
	{
		if (x > 89.0f && x <= 180.0f)
			x = 89.0f;
		while (x > 180.f)
			x -= 360.f;
		while (x < -89.0f)
			x = -89.0f;
		while (y > 180.f)
			y -= 360.f;
		while (y < -180.f)
			y += 360.f;
		z = 0;

		return *this;
	}
	inline C_Vec& SmoothAngles(C_Vec MyViewAngles, C_Vec AimAngles, C_Vec &OutAngles, float SmoothFactor)
	{
		OutAngles = AimAngles - MyViewAngles;
		OutAngles /= SmoothFactor;
		OutAngles += MyViewAngles;

		return *this;
	}
	inline C_Vec& VectorAngles(const C_Vec vForward)
	{
		float tmp, yaw, pitch;

		if (vForward.y == 0 && vForward.x == 0)
		{
			yaw = 0;
			if (vForward.z > 0)
			{
				pitch = 270;
			}
			else
			{
				pitch = 90;
			}
		}
		else
		{
			yaw = (atan2(vForward.y, vForward.x) * 180 / PI);
			if (yaw < 0)
			{
				yaw += 360;
			}
			tmp = sqrt(vForward.x * vForward.x + vForward.y * vForward.y);
			pitch = (atan2(-vForward.z, tmp) * 180 / PI);

			if (pitch < 0)
			{
				pitch += 360;
			}
		}

		x = pitch;
		y = yaw;
		z = 0;

		return *this;
	}
};

struct Bone_t
{
	float x;
	int Reserved[3];
	float y;
	int Reserved2[3];
	float z;
};
struct _GameView
{
	PVOID pAddr;
	C_Vec m_Pos;
	//bool __fastcall WorldToScreen(C_Vec Position, C_Vec& DrawPosition);
	//bool __fastcall WorldToScreen(C_Vec Position, ImVec2 &DrawPosition);
	_GameView()
	{
		pAddr = 0;
		m_Pos = C_Vec(0, 0, 0);
	}
};
struct Item
{
	PVOID Address;
	string ItemName;
};
typedef struct _KERNEL_REQUEST
{
	HANDLE              PID;
	PVOID               Address;
	PVOID               Buffer;
	PVOID               Response;//? Using only for IO_GETMODULEINFO
	ULONG               Size;
}*PKERNEL_REQUEST;

typedef struct _KERNEL_REQUEST_TEST
{
	USHORT              CtlCode;
	HANDLE              CurrentPID;
	HANDLE              TargetPID;
	PVOID               Address;
	PVOID               Buffer;
	PVOID               Response;//? Using only for IO_GETMODULEINFO
	ULONG               Size;
}*PKERNEL_REQUEST_TEST;

typedef struct DrawInfo
{
	ImVec2 From;
	ImVec2 To;
};
struct Player
{
	PVOID Address;
	PVOID BoneMatrixPtr;
	DrawInfo DrawInf;
	string NameHero;
	string Name;
	PVOID pBonePos[15];
	bool b_VisibleBox;
	bool b_VisibleRadar;
};
const char* AimBones[] = { "Head","Body" };
const char* LootLvls[] = { "Lvl 1","Lvl 2","Lvl 3","Lvl 4" };
const char* TypesGlow[] = { "Default","Bright" };
const char* StyleMenu[] = { "New","Old" };
const char* TypesBox[] = { "Default","Corners","3D Box" };
static string LootPreviewValue;
struct Hack
{
	PVOID BaseModule;
	PVOID EntitList;
	volatile PVOID WeaponAddr;
	volatile PVOID LocalPlayer;
	Player TargetEntitty;
	int CountPlayers;
	ULONG EntitCount = 0;
	const float WeaponBulletSpeed = 34000.f;
	volatile bool IsInGame;
	volatile bool b_RemoveBreath;
	volatile bool b_ShowMenu = true;
	int CountItem;
	//? Temp
	C_Vec NewAngle;
	C_Vec AimAngle;
	C_Vec LastBreath;
	C_Vec PlayerBreath;
	//? ////
	//? Style
	ImVec2 WeaponIconSize{ 50,30 };
	ImVec2 LongWeaponIconSize{ 90,30 };
	ImVec2 CommonItemsSize{ 32,44 };
	ImVec2 LongCommonItemsSize{ 47,35 };
	ImVec2 GrenadesIconSize{ 30,30 };
	ImVec2 ArmorIconSize{ 40,28 };
	ImVec2 AmmoIconSize{ 35,30 };
	ImVec2 BackpacksIconSize{ 37,35 };

	ImVec2 MenuSize;

	ImVec4 RadarBackGroundColor{ 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
	ImVec4 RadarPlayersColor{ 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };

	int Width = 0, Height = 0;
}Utils;
struct Spread
{
	volatile float x = Settings.SpreadFactor;
	//int Reserved[3];
	volatile __int64 y = Settings.SpreadFactor;
	int Reserved1;
	volatile float z = Settings.SpreadFactor;
	//volatile __int64 z = Utils.SpreadFactor;
}WeaponSpread;
#define IO_HACK              CTL_CODE(FILE_DEVICE_UNKNOWN,0x803,METHOD_BUFFERED,FILE_SPECIAL_ACCESS)
class Driver
{
public:
	Driver(DWORD TargetPID) : TargetPID{ TargetPID }
	{
		hDrv = LI_FN(CreateFileA)(xorstr_("\\\\.\\Qdeycn"), GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)0, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, (HANDLE)0);
		CurrentPID = GetCurrentProcessId();
	}
	~Driver()
	{
		CloseHandle(hDrv);
	}
	/*Driver(DWORD TargetPID) : TargetPID{ TargetPID }
	{
		ControlFunc = LI_FN(GetProcAddress)(LoadLibraryA(xorstr_("gdi32full.dll")), xorstr_("NtGdiDdDDINetDispStopMiracastDisplayDevice"));

		if (!ControlFunc)
		{
			ControlFunc = LI_FN(GetProcAddress)(LoadLibraryA(xorstr_("win32u.dll")), xorstr_("NtGdiDdDDINetDispStopMiracastDisplayDevice"));
			if (!ControlFunc)
				return;
		}
		CurrentPID = GetCurrentProcessId();
		 //= static_cast<tFunction>(NtGdiDdDDINetDispStopMiracastDisplayDevice);

	}*/
	template<typename ... A>
	void ControlDriver(_KERNEL_REQUEST_TEST* Request)
	{
		DeviceIoControl(hDrv, IO_HACK, Request, sizeof(_KERNEL_REQUEST_TEST), 0, 0, 0, 0);
		/*using tFunction = uint64_t(__stdcall*)(A...);
		const auto control = static_cast<tFunction>(ControlFunc);
		control(arguments ...);*/
	}
	void ReadMemory(PVOID Address, PVOID Buffer, ULONG size)
	{
		if (TargetPID == NULL)
			return;
		_KERNEL_REQUEST_TEST Request = { 0 };
		Request.CtlCode = IO_READ;
		Request.Address = Address;
		Request.Buffer = Buffer;
		Request.TargetPID = (HANDLE)TargetPID;
		Request.CurrentPID = (HANDLE)CurrentPID;
		Request.Size = size;
		ControlDriver(&Request);
	}
	void WriteMemory(PVOID Address, LPVOID Buffer, ULONG Size)
	{
		if (TargetPID == NULL)
			return;
		_KERNEL_REQUEST_TEST Request = { 0 };
		Request.CtlCode = IO_WRITE;
		Request.Address = Address;
		Request.Buffer = Buffer;
		Request.TargetPID = (HANDLE)TargetPID;
		Request.CurrentPID = (HANDLE)CurrentPID;
		Request.Size = Size;
		ControlDriver(&Request);
	}
	PVOID GetModuleBase()
	{
		if (TargetPID == NULL)
			return 0;
		PVOID ModuleBase = 0;
		_KERNEL_REQUEST_TEST Request = { 0 };
		wchar_t Buff[2];
		Request.Buffer = Buff;
		Request.CtlCode = IO_GETMODULEINFO;
		Request.Response = &ModuleBase;
		Request.TargetPID = (HANDLE)TargetPID;
		Request.CurrentPID = (HANDLE)CurrentPID;
		ControlDriver(&Request);
		return ModuleBase;
	}
	void* ControlFunc;
	HANDLE hDrv;
	DWORD TargetPID;
	DWORD CurrentPID;
};
class C_Entity
{
public:
	inline PVOID GetLocalPlayer(Driver *DrvObj, PVOID EntitList, int LocalPlayerIndex)
	{
		for (int i = 0; i < 100; i++)
		{
			PVOID Entity = GetEntityById(DrvObj, i, EntitList);
			if (!Entity)continue;
			PVOID EntHandle = 0;
			DrvObj->ReadMemory(((BYTE*)Entity + m_iSignifierName), &EntHandle, sizeof(PVOID));
			std::string Identifier;
			DrvObj->ReadMemory(EntHandle, &Identifier, sizeof(DWORD64)); // Getting an Identifier that Tells us what the Entity is
			LPCSTR IdentifierC = Identifier.c_str();
			if (strcmp(IdentifierC, xorstr_("player")) == 0)
			{
				int EntityIndex = 0;
				DrvObj->ReadMemory(((BYTE*)Entity + 0x8), &EntityIndex, sizeof(float));
				if (EntityIndex == LocalPlayerIndex)
				{
					return Entity;
				}
			}
		}
		return nullptr;
	}
	inline void GetPrimaryWeapon(Driver* DriverObj)
	{
		DWORD WeaponIndex = 0;
		DriverObj->ReadMemory(((BYTE*)Utils.LocalPlayer + m_latestPrimaryWeapons), (PVOID*)&WeaponIndex, 0x4);
		WeaponIndex &= 0xFFFF;
		Utils.WeaponAddr = GetEntityById(DriverObj, WeaponIndex, Utils.EntitList);
	}
	inline PVOID GetEntityById(Driver *DrvObj, int Ent, PVOID EntitList)
	{
		if (!EntitList) // Checking if there even is a BaseEntity if not than it makes no sense to try and read the Entitylist anyways
			return NULL;
		PVOID EntityAddr = 0;
		DrvObj->ReadMemory((BYTE*)EntitList + (Ent << 5), &EntityAddr, sizeof(PVOID)); // Getting the Entity at the Given ID
		return EntityAddr;
	}
	inline PVOID GetBoneMatrixPtr(Driver* DriverObj,PVOID Entity)
	{
		PVOID BoneMatrixPtr = 0;
		DriverObj->ReadMemory(((BYTE*)Entity + BoneMatrix), &BoneMatrixPtr, sizeof(PVOID));
		return BoneMatrixPtr;
	}
	inline void GetBonePos(Driver* DriverObj, C_Vec& BonePos, PVOID BoneMatrixPtr, int BoneIndex)
	{
		XMMATRIX TempBonePos;
		DriverObj->ReadMemory(((BYTE*)BoneMatrixPtr + BoneIndex * 0x30), &TempBonePos, sizeof(XMMATRIX));
		BonePos.x = TempBonePos._14;
		BonePos.y = TempBonePos._24;
		BonePos.z = TempBonePos._34;
	}
	inline void GetBonePos(Driver* DriverObj, C_Vec& BonePos, PVOID BoneAddr)
	{
		XMMATRIX TempBonePos;
		DriverObj->ReadMemory(BoneAddr, &TempBonePos, sizeof(XMMATRIX));
		BonePos.x = TempBonePos._14;
		BonePos.y = TempBonePos._24;
		BonePos.z = TempBonePos._34;
	}
	inline void GetPlayerName(Driver* DriverObj, Player* TargetPlayer)
	{
		PVOID NameAddr = 0;
		char Name[MAX_PATH]{ 0 };
		DriverObj->ReadMemory(((BYTE*)TargetPlayer->Address + m_iName), &NameAddr, sizeof(PVOID));
		if (!NameAddr)
			return;
		DriverObj->ReadMemory(NameAddr, &Name, sizeof(Name));
		TargetPlayer->Name = Name;
	}
	bool IsInGame(Driver* DrvObj)
	{
		C_Entity Entity;
		int LocalPlayerIndex = 0;
		int LocalPlayerHealth = 0;
		DrvObj->ReadMemory(((BYTE*)Utils.BaseModule + LocalEntityId), &LocalPlayerIndex, sizeof(float));
		Utils.LocalPlayer = Entity.GetLocalPlayer(DrvObj, Utils.EntitList, LocalPlayerIndex);
		DrvObj->ReadMemory(((BYTE*)Utils.LocalPlayer + m_iHealth), &LocalPlayerHealth, sizeof(float));
		if (LocalPlayerHealth != NULL)
		{
			Utils.IsInGame = true;
		}
		if (LocalPlayerHealth == NULL)
		{
			Utils.IsInGame = false;
		}
		return Utils.IsInGame;
	}
	inline string ReadModName(Driver* DrvObj,PVOID Address, const char* unknow)
	{
		char Name[MAX_PATH] = {};
		PVOID Temp;
		DrvObj->ReadMemory(((BYTE*)Address + m_ModelName), &Temp, sizeof(PVOID));
		DrvObj->ReadMemory(Temp, &Name, sizeof(Name));
		return string(Name);
	}
	VMatrix GetView(Driver* DrvObj,_GameView& GameView)
	{
		DrvObj->ReadMemory(((BYTE*)Utils.BaseModule + 0xC579FF8), &GameView.pAddr, sizeof(PVOID));
		DrvObj->ReadMemory(((BYTE*)GameView.pAddr + 0xF2140), &GameView.m_Pos, 0x12);
		PVOID pMatrix = 0;
		VMatrix matrix;
		DrvObj->ReadMemory((BYTE*)((uintptr_t)GameView.pAddr + 0x1A93D0),&pMatrix, sizeof(PVOID));
		DrvObj->ReadMemory(pMatrix, &matrix, sizeof(VMatrix));
		return matrix;
	}
	inline int __fastcall GetHealth(Driver* DriverObj,PVOID Entity)
	{
		int Health = 0;
		DriverObj->ReadMemory(((BYTE*)Entity + m_iHealth), &Health, sizeof(float));
		if (!Health)
		{
			Health = 1;
		}
		return Health;
	}
	inline int __fastcall GetMaxHealth(Driver* DriverObj, PVOID Entity)
	{
		int MaxHealth = 0;
		DriverObj->ReadMemory(((BYTE*)Entity + m_iMaxHealth), &MaxHealth, sizeof(float));
		if (!MaxHealth)
		{
			MaxHealth = 1;
		}
		return MaxHealth;
	}
	inline int __fastcall GetShield(Driver* DriverObj, PVOID Entity)
	{
		int Shield = 0;
		DriverObj->ReadMemory(((BYTE*)Entity + m_shieldHealth), &Shield, sizeof(float));
		if (Shield == 1)
		{
			Shield = 50;
		}
		else if (Shield == 2)
		{
			Shield = 75;
		}
		else if (Shield == 3)
		{
			Shield = 100;
		}
		else if (Shield == 4)
		{
			Shield = 100;
		}
		else if (Shield == 0)
		{
			Shield = 1;
		}
		return Shield;
	}
	inline int __fastcall GetMaxShield(Driver* DriverObj, PVOID Entity)
	{
		int MaxShield = 0;
		DriverObj->ReadMemory(((BYTE*)Entity + m_armorType), &MaxShield, sizeof(float));
		if (MaxShield == 1)
		{
			MaxShield = 50;
		}
		else if (MaxShield == 2)
		{
			MaxShield = 75;
		}
		else if (MaxShield == 3)
		{
			MaxShield = 100;
		}
		else if (MaxShield == 4)
		{
			MaxShield = 100;
		}
		else if (MaxShield == 0)
		{
			MaxShield = 1;
		}
		return MaxShield;
	}
	inline C_Vec GetPlayerPos(Driver* DrvObj,PVOID PlayerAddr)
	{
		C_Vec PlayerPos;
		DrvObj->ReadMemory(((BYTE*)PlayerAddr + m_vecAbsOrigin), &PlayerPos, 0x12);
		return PlayerPos;
	}
};
bool __fastcall WorldToScreen(C_Vec Position, C_Vec& DrawPosition,VMatrix& m_Matrix)
{
	float w = 0.0f;
	DrawPosition.x = (m_Matrix.m[0][0] * Position.x) + (m_Matrix.m[0][1] * Position.y) + (m_Matrix.m[0][2] * Position.z + m_Matrix.m[0][3]);
	DrawPosition.y = (m_Matrix.m[1][0] * Position.x) + (m_Matrix.m[1][1] * Position.y) + (m_Matrix.m[1][2] * Position.z + m_Matrix.m[1][3]);
	w = (m_Matrix.m[3][0] * Position.x) + (m_Matrix.m[3][1] * Position.y) + (m_Matrix.m[3][2] * Position.z + m_Matrix.m[3][3]);

	if (w < 0.01f)
		return false;

	float invw = 1.0f / w;
	DrawPosition.x *= invw;
	DrawPosition.y *= invw;

	ImGuiIO& io = ImGui::GetIO();
	int ScreenWidth = io.DisplaySize.x;
	int ScreenHeight = io.DisplaySize.y;

	float x = ScreenWidth / 2;
	float y = ScreenHeight / 2;
	x += 0.5 * DrawPosition.x * ScreenWidth + 0.5;
	y -= 0.5 * DrawPosition.y * ScreenHeight + 0.5;
	DrawPosition.x = x;
	DrawPosition.y = y;
	return true;
}
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}
Item ItemList[1000]{ 0 };
Player PlayerList[100];



class C_Radar
{
public:
	void RadarRange(float* x, float* y, float range)
	{
		if (fabs((*x)) > range || fabs((*y)) > range)
		{
			if ((*y) > (*x))
			{
				if ((*y) > -(*x))
				{
					(*x) = range * (*x) / (*y);
					(*y) = range;
				}
				else
				{
					(*y) = -range * (*y) / (*x);
					(*x) = -range;
				}
			}
			else
			{
				if ((*y) > -(*x))
				{
					(*y) = range * (*y) / (*x);
					(*x) = range;
				}
				else
				{
					(*x) = -range * (*x) / (*y);
					(*y) = -range;
				}
			}
		}
	}
	void CalcRadarPoint(C_Vec vOrigin, int& screenx, int& screeny,C_Vec LocalvAngle,C_Vec LocalOrigin)
	{
		float dx = vOrigin.x - LocalOrigin.x;
		float dy = vOrigin.y - LocalOrigin.y;

		float fYaw = float(LocalvAngle.y * (PI / 180.0));

		float fsin_yaw = sin(fYaw);
		float fminus_cos_yaw = -cos(fYaw);

		float x = dy * fminus_cos_yaw + dx * fsin_yaw;
		float y = dx * fminus_cos_yaw - dy * fsin_yaw;

		float range = Settings.f_RadarRange * 100.f;

		RadarRange(&x, &y, range);

		ImVec2 DrawSize(300, 300);

		int rad_x = Settings.RadarPos[0];
		int rad_y = Settings.RadarPos[1];

		float r_siz_x = DrawSize.x;
		float r_siz_y = DrawSize.y;

		int x_max = (int)r_siz_x + rad_x - 5;
		int y_max = (int)r_siz_y + rad_y - 5;

		screenx = rad_x + ((int)r_siz_x / 2 + int(x / range * r_siz_x));
		screeny = rad_y + ((int)r_siz_y / 2 + int(y / range * r_siz_y));

		if (screenx > x_max)
			screenx = x_max;

		if (screenx < rad_x)
			screenx = rad_x;

		if (screeny > y_max)
			screeny = y_max;

		if (screeny < rad_y)
			screeny = rad_y;
	}
	void DrawPlayer(C_Vec EntityOrigin, C_Vec LocalvAngle, C_Vec LocalOrigin)
	{
		int screenx = 0;
		int screeny = 0;
		CalcRadarPoint(EntityOrigin, screenx, screeny,LocalvAngle,LocalOrigin);

		ImDrawList* Draw = ImGui::GetWindowDrawList();

		Draw->AddRectFilled(ImVec2((float)screenx, (float)screeny),
			ImVec2((float)screenx + 5, (float)screeny + 5),
			ImGui::GetColorU32(Settings.BoxColor));
	}
};
