#pragma once
//? Old offset
/*constexpr auto EntityList = 0x1fa0148;
constexpr auto LocalEntityId = 0x1750F5C;
constexpr auto VecOrigin = 0x12C;
constexpr auto EntityHandle = 0x500;
constexpr auto TeamNum = 0x3E4;
constexpr auto PlayerHealth = 0x3D4;
constexpr auto Recoil = 0x2014;
constexpr auto HeadPos = 0x3AF0;
constexpr auto Punch_diff = 0x2014;
constexpr auto ViewAngle = 0x20B8;
constexpr auto EntityCount = 0xC006788;
constexpr auto WeaponSpreadX = 0x1330;
constexpr auto WeaponSpreadY = 0x1340;
constexpr auto PrimaryWeaponIndex = 0x163C;
constexpr auto BoneMatrix = 0xED8;
constexpr auto Breath = 0x20A8;
constexpr auto PlayerCamPos = 0x1ACC;
constexpr auto BulletSpeed = 0x1BBC;
constexpr auto PlayerState = 0x2368;
constexpr auto TimeScale = 0x18E3D50;
constexpr auto EntityCount = 0xC010F74;
constexpr auto PlayerMaxHealth = 0x4FC;
constexpr auto PlayerShield = 0x150;
constexpr auto Spotted = 0x16C;*/

///////////////////////////////////
constexpr auto  m_bleedoutState = 0x2378;
constexpr auto  m_iHealth = 0x3E0;
constexpr auto  m_iMaxHealth = 0x500;
constexpr auto	m_shieldHealth = 0x164;
constexpr auto	m_shieldHealthMax = 0x160;
constexpr auto	m_iTeamNum = 0x3F0;
constexpr auto	m_lifeState = 0x720;
constexpr auto	m_armorType = 0x4248;
constexpr auto	m_helmetType = 0x4244;
constexpr auto	m_duckState = 0x26A0;
constexpr auto	m_fFlags = 0x8C;
constexpr auto	m_vecVelocity = 0x418;
constexpr auto	m_latestPrimaryWeapons = 0x1704;
constexpr auto	m_iWorldModelIndex = 0x1318;
constexpr auto	m_latestNonOffhandWeapons = 0x168C;
constexpr auto	EntityList = 0x1F1AC78;//0x1F96EB8;
constexpr auto  dwEntityCount = 0xBF93FF8;//0xC008AD0;
constexpr auto  m_iSignifierName = 0x508;
constexpr auto  ViewAngle = 0x21B8;
constexpr auto  m_gravityScale = 0x13CC;
constexpr auto  m_vecAimPunch = 0x2114;
constexpr auto  m_vecAbsOrigin = 0x13C;
constexpr auto  m_vecAbsVelocity = 0x130;
constexpr auto  LocalEntityId = 0x171B384;
constexpr auto  m_angNetworkAngles = 0x424;
constexpr auto  m_Collision = 0x450;
constexpr auto  m_ModelName = 0x818;
//? Collision
constexpr auto  m_vecMins = 0x10;
constexpr auto  m_vecMaxs = 0x1C;
//?
constexpr auto  m_localAngles = 0x424;
constexpr auto  sv_gravity = 0x23D47E8;
constexpr auto  BoneMatrix = 0xED8;
constexpr auto  BulletSpeed = 0x1C24;
constexpr auto  tickspeed = 0x18DB2C0;
constexpr auto  m_iName = 0x511;
constexpr auto  m_customScriptInt = 0x1308;//ItemId
constexpr auto  m_vecWeaponSpread = 0x1370;
//constexpr auto  m_nSkin = 0xE48;

constexpr auto  ForceJump = 0xC5F2298;
constexpr auto  net_droppackets = 0x01751780;
constexpr auto  m_visibilityFlags = 0x000003E8;
///////////////////////////////////