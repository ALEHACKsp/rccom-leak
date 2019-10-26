#pragma once
#include <d3d11.h>
#include "Textures.h"
ID3D11ShaderResourceView*	pIBuf_Energy;
ID3D11ShaderResourceView*	pIBuf_Hc;
ID3D11ShaderResourceView*	pIBuf_Sc;
ID3D11ShaderResourceView*	pIBuf_Shg;

ID3D11ShaderResourceView*	pIBuf_Havoc;
ID3D11ShaderResourceView*	pIBuf_Hemlock;
ID3D11ShaderResourceView*	pIBuf_Doubletake;

ID3D11ShaderResourceView*	pIBuf_G2;
ID3D11ShaderResourceView*	pIBuf_P2011;
ID3D11ShaderResourceView*	pIBuf_R97;
ID3D11ShaderResourceView*	pIBuf_Rspn101;
ID3D11ShaderResourceView*	pIBuf_P2011_Auto;
ID3D11ShaderResourceView*	pIBuf_Alternator_Smg;

ID3D11ShaderResourceView*	pIBuf_W1128;
ID3D11ShaderResourceView*	pIBuf_Peacekeeper;
ID3D11ShaderResourceView*	pIBuf_Pstl_Sa3;

ID3D11ShaderResourceView*	pIBuf_Mastiff_Stgn;
ID3D11ShaderResourceView*	pIBuf_At_Rifle;

ID3D11ShaderResourceView*	pIBuf_M1a1_Hemlok;
ID3D11ShaderResourceView*	pIBuf_Prowler_Smg;
ID3D11ShaderResourceView*	pIBuf_Lmg_Hemlok;
ID3D11ShaderResourceView*	pIBuf_Vinson;
ID3D11ShaderResourceView*	pIBuf_B3wing;
ID3D11ShaderResourceView*	pIBuf_Rspn101_Dmr;

ID3D11ShaderResourceView*	pIBuf_Thermite_Grenade;
ID3D11ShaderResourceView*	pIBuf_Shuriken;
ID3D11ShaderResourceView*	pIBuf_M20_Grenade;

ID3D11ShaderResourceView*	pIBuf_Shield_Battery_Small;
ID3D11ShaderResourceView*	pIBuf_Shield_Battery_Large;

ID3D11ShaderResourceView*	pIBuf_Health_Main_Small;
ID3D11ShaderResourceView*	pIBuf_Health_Main_Large;

ID3D11ShaderResourceView*	pIBuf_Phoenix_Kit;

ID3D11ShaderResourceView*	pIBuf_Accelerant;

ID3D11ShaderResourceView*	pIBuf_Backpack_Light;
ID3D11ShaderResourceView*	pIBuf_Backpack_Medium;
ID3D11ShaderResourceView*	pIBuf_Backpack_Heavy;
ID3D11ShaderResourceView*	pIBuf_Shield_Down;
ID3D11ShaderResourceView*	pIBuf_Shield_Upgrade_Body;
ID3D11ShaderResourceView*	pIBuf_Shield_Upgrade_Head;

ID3D11ShaderResourceView*	pIBuf_Optic_Cq_Holo_Var;
ID3D11ShaderResourceView*	pIBuf_Optic_Cq_Holo_Var_2x;
ID3D11ShaderResourceView*	pIBuf_Optic_Cq_Hcog_R1;
ID3D11ShaderResourceView*	pIBuf_Optic_Cq_Hcog_R2;
ID3D11ShaderResourceView*	pIBuf_Optic_Cq_Threat;
ID3D11ShaderResourceView*	pIBuf_Optic_Rng_Hcog_Acgs;
ID3D11ShaderResourceView*	pIBuf_Optic_Rng_Aog_Var_R1;
ID3D11ShaderResourceView*	pIBuf_Optic_Rng_Sni_Dcom;
ID3D11ShaderResourceView*	pIBuf_Optic_Rng_Sni_Var_Talon;
ID3D11ShaderResourceView*	pIBuf_Optic_Rng_Aog_Var_R2;

ID3D11ShaderResourceView*	pIBuf_Suppr_V2b;

ID3D11ShaderResourceView*	pIBuf_Mag_V1b;
ID3D11ShaderResourceView*	pIBuf_Mag_V2b;
ID3D11ShaderResourceView*	pIBuf_Mag_V3b;

ID3D11ShaderResourceView*	pIBuf_Stock_Folded_Regular;
ID3D11ShaderResourceView*	pIBuf_Stock_Folded_Sniper;

ID3D11ShaderResourceView*	pIBuf_Chip_V1;
ID3D11ShaderResourceView*	pIBuf_Chip_V2;
ID3D11ShaderResourceView*	pIBuf_Chip_V3;
ID3D11ShaderResourceView*	pIBuf_Chip_V4;



void __fastcall ShaderResourceIni(ID3D11Device * D3d11Device)
{
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_AmmoNrg, sizeof(Texture_AmmoNrg), 0, 0, &pIBuf_Energy, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_AmmoHc, sizeof(Texture_AmmoHc), 0, 0, &pIBuf_Hc, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_AmmoSc, sizeof(Texture_AmmoSc), 0, 0, &pIBuf_Sc, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_AmmoShg, sizeof(Texture_AmmoShg), 0, 0, &pIBuf_Shg, 0);

	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Havoc, sizeof(Texture_Havoc), 0, 0, &pIBuf_Havoc, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Hemlock, sizeof(Texture_Hemlock), 0, 0, &pIBuf_Hemlock, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Doubletake, sizeof(Texture_Doubletake), 0, 0, &pIBuf_Doubletake, 0);

	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_G2, sizeof(Texture_G2), 0, 0, &pIBuf_G2, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_P2011, sizeof(Texture_P2011), 0, 0, &pIBuf_P2011, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_R97, sizeof(Texture_R97), 0, 0, &pIBuf_R97, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Rspn101, sizeof(Texture_Rspn101), 0, 0, &pIBuf_Rspn101, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_P2011_Auto, sizeof(Texture_P2011_Auto), 0, 0, &pIBuf_P2011_Auto, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Alternator_Smg, sizeof(Texture_Alternator_Smg), 0, 0, &pIBuf_Alternator_Smg, 0);

	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_W1128, sizeof(Texture_W1128), 0, 0, &pIBuf_W1128, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Peacekeeper, sizeof(Texture_Peacekeeper), 0, 0, &pIBuf_Peacekeeper, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Pstl_Sa3, sizeof(Texture_Pstl_Sa3), 0, 0, &pIBuf_Pstl_Sa3, 0);

	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Mastiff_Stgn, sizeof(Texture_Mastiff_Stgn), 0, 0, &pIBuf_Mastiff_Stgn, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_At_Rifle, sizeof(Texture_At_Rifle), 0, 0, &pIBuf_At_Rifle, 0);

	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_M1a1_Hemlok, sizeof(Texture_M1a1_Hemlok), 0, 0, &pIBuf_M1a1_Hemlok, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Prowler_Smg, sizeof(Texture_Prowler_Smg), 0, 0, &pIBuf_Prowler_Smg, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Lmg_Hemlok, sizeof(Texture_Lmg_Hemlok), 0, 0, &pIBuf_Lmg_Hemlok, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Vinson, sizeof(Texture_Vinson), 0, 0, &pIBuf_Vinson, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_B3wing, sizeof(Texture_B3wing), 0, 0, &pIBuf_B3wing, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Rspn101_Dmr, sizeof(Texture_Rspn101_Dmr), 0, 0, &pIBuf_Rspn101_Dmr, 0);

	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Thermite_Grenade, sizeof(Texture_Thermite_Grenade), 0, 0, &pIBuf_Thermite_Grenade, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Shuriken, sizeof(Texture_Shuriken), 0, 0, &pIBuf_Shuriken, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_M20_Grenade, sizeof(Texture_M20_Grenade), 0, 0, &pIBuf_M20_Grenade, 0);

	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Shield_Battery_Small, sizeof(Texture_Shield_Battery_Small), 0, 0, &pIBuf_Shield_Battery_Small, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Shield_Battery_Large, sizeof(Texture_Shield_Battery_Large), 0, 0, &pIBuf_Shield_Battery_Large, 0);

	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Health_Main_Small, sizeof(Texture_Health_Main_Small), 0, 0, &pIBuf_Health_Main_Small, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Health_Main_Large, sizeof(Texture_Health_Main_Large), 0, 0, &pIBuf_Health_Main_Large, 0);

	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Phoenix_Kit, sizeof(Texture_Phoenix_Kit), 0, 0, &pIBuf_Phoenix_Kit, 0);

	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Accelerant, sizeof(Texture_Accelerant), 0, 0, &pIBuf_Accelerant, 0);

	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Backpack_Light, sizeof(Texture_Backpack_Light), 0, 0, &pIBuf_Backpack_Light, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Backpack_Medium, sizeof(Texture_Backpack_Medium), 0, 0, &pIBuf_Backpack_Medium, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Backpack_Heavy, sizeof(Texture_Backpack_Heavy), 0, 0, &pIBuf_Backpack_Heavy, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Shield_Down, sizeof(Texture_Shield_Down), 0, 0, &pIBuf_Shield_Down, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Shield_Upgrade_Body, sizeof(Texture_Shield_Upgrade_Body), 0, 0, &pIBuf_Shield_Upgrade_Body, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Shield_Upgrade_Head, sizeof(Texture_Shield_Upgrade_Head), 0, 0, &pIBuf_Shield_Upgrade_Head, 0);

	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Optic_Cq_Holo_Var_2x, sizeof(Texture_Optic_Cq_Holo_Var_2x), 0, 0, &pIBuf_Optic_Cq_Holo_Var_2x, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Optic_Cq_Holo_Var, sizeof(Texture_Optic_Cq_Holo_Var), 0, 0, &pIBuf_Optic_Cq_Holo_Var, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Optic_Cq_Hcog_R1, sizeof(Texture_Optic_Cq_Hcog_R1), 0, 0, &pIBuf_Optic_Cq_Hcog_R1, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Optic_Cq_Hcog_R2, sizeof(Texture_Optic_Cq_Hcog_R2), 0, 0, &pIBuf_Optic_Cq_Hcog_R2, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Optic_Cq_Threat, sizeof(Texture_Optic_Cq_Threat), 0, 0, &pIBuf_Optic_Cq_Threat, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Optic_Rng_Hcog_Acgs, sizeof(Texture_Optic_Rng_Hcog_Acgs), 0, 0, &pIBuf_Optic_Rng_Hcog_Acgs, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Optic_Rng_Aog_Var_R1, sizeof(Texture_Optic_Rng_Aog_Var_R1), 0, 0, &pIBuf_Optic_Rng_Aog_Var_R1, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Optic_Rng_Sni_Dcom, sizeof(Texture_Optic_Rng_Sni_Dcom), 0, 0, &pIBuf_Optic_Rng_Sni_Dcom, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Optic_Rng_Sni_Var_Talon, sizeof(Texture_Optic_Rng_Sni_Var_Talon), 0, 0, &pIBuf_Optic_Rng_Sni_Var_Talon, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Optic_Rng_Aog_Var_R2, sizeof(Texture_Optic_Rng_Aog_Var_R2), 0, 0, &pIBuf_Optic_Rng_Aog_Var_R2, 0);

	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Suppr_V2b, sizeof(Texture_Suppr_V2b), 0, 0, &pIBuf_Suppr_V2b, 0);

	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Mag_V1b, sizeof(Texture_Mag_V1b), 0, 0, &pIBuf_Mag_V1b, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Mag_V2b, sizeof(Texture_Mag_V2b), 0, 0, &pIBuf_Mag_V2b, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Mag_V3b, sizeof(Texture_Mag_V3b), 0, 0, &pIBuf_Mag_V3b, 0);

	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Stock_Folded_Regular, sizeof(Texture_Stock_Folded_Regular), 0, 0, &pIBuf_Stock_Folded_Regular, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Stock_Folded_Sniper, sizeof(Texture_Stock_Folded_Sniper), 0, 0, &pIBuf_Stock_Folded_Sniper, 0);

	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Chip_V1, sizeof(Texture_Chip_V1), 0, 0, &pIBuf_Chip_V1, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Chip_V2, sizeof(Texture_Chip_V2), 0, 0, &pIBuf_Chip_V2, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Chip_V3, sizeof(Texture_Chip_V3), 0, 0, &pIBuf_Chip_V3, 0);
	D3DX11CreateShaderResourceViewFromMemory(D3d11Device, Texture_Chip_V4, sizeof(Texture_Chip_V4), 0, 0, &pIBuf_Chip_V4, 0);
}