#pragma once
namespace  _Bone
{

	int Bone_medium_bloodhound[15] = { 12, 9,7,14, 18, 28,47, 51, 58,88, 89, 91,93, 94, 96 };
	int Bone_medium_bloodhound1[15] = { 12, 9,7, 14, 18, 21,	40, 44, 47, 65, 66, 68, 70, 71, 73 };
	int Bone_medium_bloodhound2[15] = { 12, 9,7, 14, 17, 20, 39, 42, 45,63, 64, 66, 68, 69, 71 };

	int Bone_heavy_gibraltar[15] = { 12, 9,7, 14, 18, 21, 41, 45, 48,72, 73, 75, 77, 78, 80 };
	int Bone_heavy_gibraltar2[15] = { 12, 9,7,	14, 17, 20,40, 43, 46,64, 65, 67,69, 70, 72 };

	int Bone_lifeline_support[15] = { 12, 9,7,	14, 17, 20,39, 42, 45,63, 64, 66,68, 69, 71 };

	int Bone_heavy_pathfinder[15] = { 10, 9, 7,	14, 16, 17,27, 29, 30,66, 67, 68,70, 71, 72 };
	int Bone_heavy_pathfinder1[15] = { 10, 9, 7,	14, 17, 19,27, 30, 32,68, 69, 70,72, 73, 74 };
	int Bone_heavy_pathfinder2[15] = { 10, 9, 7,	14, 17, 19,27, 29, 31,67, 68, 69,71, 72, 73 };

	int Bone_light_wraith[15] = { 10, 9, 7,	14, 17, 20,39, 42, 45,91, 92, 94,96, 97, 99 };
	int Bone_light_wraith1[15] = { 10, 9, 7,	14, 17, 20,39, 42, 45,79, 80, 82,84, 85, 87 };
	int Bone_light_wraith2[15] = { 10, 9, 7,	14, 16, 19,38, 40, 42,60, 61, 63,65, 66, 68 };

	int Bone_medium_bangalore[15] = { 12, 9,7,	14, 17, 20,43, 46, 49,81, 82, 84,86, 87, 89 };
	int Bone_medium_bangalore1[15] = { 12, 9,7,	14, 16, 18,41, 43, 45,77, 78, 80,82, 83, 85 };
	int Bone_medium_bangalore2[15] = { 12, 9,7,	14, 17, 19,42, 45, 47,79, 80, 82,84, 85, 87 };

	int Bone_heavy_caustic[15] = { 10, 9, 7,	14, 16, 19,38, 40, 43,74, 75, 77,79, 80, 82 };
	int Bone_heavy_caustic1[15] = { 10, 9, 7,	14, 17, 20,39, 42, 45,72, 73, 75,77, 78, 80 };
	int Bone_heavy_caustic2[15] = { 10, 9, 7,	14, 16, 18,37, 39, 41,60, 61, 62,64, 65, 66 };

	int Bone_medium_holo[15] = { 10, 9, 7,	14, 16, 19,41, 43, 46,84, 85, 87,89, 90, 92 };
	int Bone_medium_holo1[15] = { 10, 9, 7,	21, 24, 27,46, 49, 52,78, 79, 81,83, 84, 86 };
	int Bone_medium_holo2[15] = { 10, 9, 7,	14, 17, 20,42, 45, 48,69, 70, 72,74, 75, 77 };

	int Bone_medium_stim[15] = { 12, 9,7,	15, 17, 20,43, 46, 49,81, 82, 83,85, 86, 87 };
	int Bone_medium_stim1[15] = { 12, 9,7,	15, 17, 20,43, 46, 49,81, 82, 83,85, 86, 87 };
	int Bone_medium_stim2[15] = { 12, 9,7,	15, 17, 20,43, 46, 49,81, 82, 83,85, 86, 87 };
};
enum class Game
{

	gPresent = 0x164B28,
	gEntityList = 0x1F9AE68,//世界
	gGameLocal = 0x234DEE8,//本人

	gGameSpeed = 0x18DEB10,
	gGameSpeedRSP = 0x2ecebf,
	gVoidMemory = 0x12a8A00,

	//gVisable = 0xc18a940,
	//FnVisable1 = 0x2661e0,
	FnVisable = 0x92e730,
	gManu = 0x23853D0,//==3||27FC600=101;判断是否在菜单    


	gView = 0xC579FF8,//0xC5F0A10,//0xC5EFA10,//0xC1BABF0,//矩阵
	pView_Pos = 0xF2140,
	pView_Mat = 0xf20c0,//VIEW_MATRIX 
	pView_Fov = 0xf2100,

	pRenderer_Pos = 0x9f0, // GameRenderer
	pRenderer_Matrix = 0xb40, // GameRenderer
	pRenderer_Cmp = 0x3228,
};
enum class ObjectManager
{
	Info_ID = 0x8,
	Info_TypeName = 0x500,//m_iSignifierName
	Info_ModName = 0x818,
	Info_Dead = 0x2368,
	Info_Bone_Address = 0xed8,//mboneMatrtix
	Info_Bone_Next = 0x30,
	Info_Velocity = 0x120,//预判坐标/玩家速度
	Info_Pos_Min = 0x12C,//敌人坐标
	Info_Pos_Top = 0x3ccc,
};
enum bone_id
{
	c_delta,
	c_start,
	c_pov,
	c_camera,
	c_hip,
	c_spineA,
	c_spineB,
	c_spineC,
	ja_c_propGun,
	c_neckA,
	jx_c_neckB,
	c_neckB,
	c_head,
	l_clav,
	l_shoulder,
	l_shoulderTwist,
	l_shoulderMid,
	l_elbowB,
	l_elbow,
	l_forearm,
	l_wrist,
	ja_l_propHandv,
	l_finThumbA,
	l_finThumbB,
	l_finThumbC,
	l_finIndexA,
	l_finIndexB,
	l_finIndexC,
	l_finMidA,
	l_finMidB,
	l_finMidC,
	l_finRingCarpal,
	l_finRingA,
	l_finRingB,
	l_finRingC,
	l_finPinkyA,
	l_finPinkyB,
	l_finPinkyC,
	r_clav,
	r_shoulder,
	r_shoulderTwist,
	r_shoulderMid,
	r_elbowB,
	r_elbow,
	r_forearm,
	r_wrist,
	ja_r_propHand,
	r_finThumbA,
	r_finThumbB,
	r_finThumbC,
	r_finIndexA,
	r_finIndexB,
	r_finIndexC,
	r_finMidA,
	r_finMidB,
	r_finMidC,
	r_finRingCarpal,
	r_finRingA,
	r_finRingB,
	r_finRingC,
	r_finPinkyA,
	r_finPinkyB,
	r_finPinkyC,
	l_thigh,
	l_kneeB,
	l_knee,
	l_ankle,
	l_ball,
	r_thigh,
	r_kneeB,
	r_knee,
	r_ankle,
	r_ball,
	r_eyeball,
	l_eyeball,
	c_teethTop,
	c_jawA,
	c_tongue_a,
	c_tongue_b,
	c_tongue_c,
	c_tongue_d,
	c_teethBot_manual,
	c_mouthv,
	r_lipUpperOuter_sticky,
	r_lipCorner_sticky,
	c_lipUpper_b,
	c_lipUpper_a,
	r_lipLowerOuter_sticky,
	l_lipUpperCenter_b,
	c_lipLower_a,
	c_lipLower_b,
	l_innerlipUpper_sticky,
	l_lipUpperCenter_a,
	r_lipLower_sticky,
	l_lipLowerOuter_b,
	l_lipLowerOuter_a,
	r_lipUpperCenter_b,
	r_lipCorner_a,
	r_innerlipUpper_sticky,
	r_lipLowerOuter_a,
	r_lipLowerOuter_b,
	r_lipUpperCenter_a,
	l_lipLower_sticky,
	r_lipCorner_b,
	r_lipUpperOuter_b,
	l_innerlipUpper_b,
	r_innerlipUpper_b,
	l_innerlipUpper_a,
	r_innerlipUpper_a,
	r_lipUpperOuter_a,
	l_lipCorner_b,
	l_lipLowerOuter_sticky,
	l_lipLower_b,
	l_lipLower_a,
	l_lipCorner_a,
	l_lipUpperCenter_sticky,
	r_lipLower_b,
	r_lipLower_a,
	l_lipUpperOuter_b,
	l_lipUpperOuter_a,
	r_lipUpperCenter_sticky,
	l_lipCorner_sticky,
	l_lipUpperOuter_sticky,
	c_noseLower,
	r_cheekOuter,
	r_nasolabiaEdge,
	c_noseBridge,
	c_forehead,
	l_nasolabiaEdge,
	l_cheekOuter,
	l_eyelidLowerInner,
	c_noseArc,
	l_eyelidUpperFurrow,
	c_adamsApple,
	l_eyelidUpper,
	c_chin,
	r_chinSide,
	r_eyesackLower,
	l_eyelidLower,
	r_eyelidUpper,
	l_chinSide,
	l_nasolabialFurrow,
	l_cheekLower,
	r_nasolabialBulge,
	r_foreheadIn,
	r_foreheadMid,
	r_eyelidLowerInner,
	l_eyelidUpperOutter,
	l_foreheadIn,
	l_underChin,
	l_masseter,
	r_cheekInner,
	r_nasolabialFurrow,
	r_eyelidUpperFurrow,
	r_foreheadOut,
	r_eyelidLower,
	r_masseter,
	r_underChin,
	l_eyesackLower,
	l_eyelidUpperInner,
	l_nasolabialBulge,
	l_eyesackUpper,
	l_eyelidLowerOutter,
	l_foreheadMid,
	c_underChin,
	r_eyelidUpperOutter,
	l_foreheadOut,
	l_cheekInner,
	r_eyelidUpperInner,
	r_eyesackUpper,
	r_eyelidLowerOutter,
	r_cheekLower,
	r_nostril,
	l_nostril,
	c_noseMid
};