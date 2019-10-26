#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <filesystem>
#include <Psapi.h>
#include <Windows.h>
#include <iostream>
#include <wchar.h>
#include <string>
#include <random>
#include "XorStr.h"
#include "imgui.h"
using namespace std;
struct CFG
{
	//? Aimbot
	volatile int  i_AimKey = 2;
	volatile bool b_AimBot;
	volatile float fov = 5;
	volatile bool b_AimAtknocked;
	volatile float f_AimDistance = 100.f;
	volatile bool b_HighlightAimTarget;
	volatile bool b_RCS;
	volatile float f_Smooth = 0.f;
	volatile bool b_Prediction;
	volatile bool b_DrawFOV;
	volatile int i_AimBone = 0;
	//? ESP
	volatile bool b_GlowEsp;
	volatile bool b_BoxESP;
	volatile float f_DistanceEsp = 1000.f;
	volatile float f_BoxDistance = 100.f;
	volatile bool b_ShowHealth;
	volatile bool b_ShowShield;
	volatile bool b_ShowDistance;
	volatile bool b_VisibleCheck;
	volatile bool b_ShowName;
	volatile bool b_ShowSnapline;
	volatile bool b_ShowSkeleton;
	int TypeGlow;
	int TypeBox;
	//? Radar
	volatile bool b_ShowRadar;
	volatile float f_RadarRange = 100.f;
	int RadarPos[2] = { 80,80 };
	//? Loot
	volatile bool b_ShowItemId;
	volatile bool b_ShowBackpacks;
	volatile bool b_ShowWeapon;
	volatile bool b_ShowMedAndArmor;
	volatile bool b_ShowHelmet;
	volatile bool b_ShowBodyShield;
	volatile bool b_ShowKnockdownShield;
	volatile bool b_ShowAmmo;
	volatile bool b_ShowOptics;
	volatile bool b_ShowGrenades;
	volatile float f_LootEspDistance = 20;

	bool b_LvlLoot[4];
	//? Misc
	volatile bool b_InstantHit;
	volatile bool b_SpeedHack;
	volatile float f_SpeedFactor = 0.f;
	volatile bool b_RemoveSpread;
	volatile int i_SpeedKey = 66;
	volatile float SpreadFactor;
	volatile bool b_Bhop;
	//? Styles
	volatile float f_BoxThickness = 1.5f;
	volatile float f_ShieldLineThickness = 1.5f;
	volatile float f_HealthLineThickness = 1.5f;
	volatile float f_BoxRounding = 5.f;
	volatile float f_BoxCornersRounding = 15.f;
	volatile float f_SnaplinesThickness = 1.f;

	int StyleMenu;
	//? Color style
	ImVec4 SkeletonColor{ 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
	ImVec4 BoxColor{ 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
	ImVec4 SnaplinesColor{ 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f };
	//? ////
};
CFG Settings;
namespace fs = std::filesystem;
void GenerateSymbols(wchar_t* Buff, int Length)
{
	mt19937_64 Generator(time(0));
	uniform_int_distribution<int>distribution(97, 122);
	for (int i = 0; i < Length; i++)
	{
		Buff[i] = (wchar_t)distribution(Generator);
	}
}
bool GetFilePathByExtension(const wchar_t* Extension, wchar_t* Directory, wchar_t* OutBuff)
{
	wchar_t Buff[4]{ 0 };
	std::wstring extension(Extension);
	if (Directory == NULL)
	{
		wchar_t TempBuff[256]{ 0 };
		GetSystemDirectoryW(TempBuff, 1024);
		wcscpy(Buff, TempBuff);
		Buff[3] = '\0';
	}
	else
	{
		wcscpy_s(Buff, sizeof(Directory), Directory);
	}
	try
	{
		std::wstring Path(Buff);
		for (auto &p : fs::directory_iterator(Path))
		{
			if (!fs::is_regular_file(p.status()))
				continue;
			std::wstring name(p.path().filename().c_str());

			bool match = !name.compare(name.size() - extension.size(), extension.size(), extension);
			if (!match)
				continue;

			wcscpy(OutBuff, Buff);
			wcscat(OutBuff, name.c_str());
			return true;
		}
		return false;
	}
	catch (std::exception &e)
	{
		return 0;
	}
}
bool ReadCfg(CFG* V)
{
	wchar_t CfgFile[256]{ 0 };
	HANDLE hFile = NULL;
	if (!GetFilePathByExtension(L".dt1", NULL, CfgFile))
	{
		return false;
	}
	hFile = CreateFileW(CfgFile, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	if (!ReadFile(hFile, V, sizeof(*V), NULL, NULL))
	{
		return false;
	}
	CloseHandle(hFile);
	return true;
}
bool WriteCfg(CFG* V)
{
	wchar_t CfgFile[256]{ 0 };
	HANDLE hFile = NULL;
	if (!GetFilePathByExtension(L".dt1", NULL, CfgFile))
	{
		memset(CfgFile, 0, 256);
		GetSystemDirectoryW(CfgFile, 256);
		CfgFile[3] = '\0';
		wchar_t FileName[9]{ 0 };
		GenerateSymbols(FileName, 6);
		wcscat(CfgFile, FileName);
		wcscat(CfgFile, L".dt1");
		hFile = CreateFileW(CfgFile, FILE_ALL_ACCESS, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}
	}
	else
	{
		hFile = CreateFileW(CfgFile, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}
	}
	if (!WriteFile(hFile, V, sizeof(*V), NULL, NULL))
	{
		return false;
	}
	CloseHandle(hFile);
	return true;
}