#ifndef NEO_WEAPON_PARSE_H
#define NEO_WEAPON_PARSE_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_parse.h"

enum NEOWeaponID
{
	WEAPON_NONE = 0,

	WEAPON_MP5,
	WEAPON_SUPA7,
	WEAPON_GRENADE,
	WEAPON_HL2PISTOL,
	WEAPON_TACHI,
	WEAPON_ZR68S,
	WEAPON_ZR68C,
	WEAPON_ZR68L,
	WEAPON_MX,
	WEAPON_MILSO,
	WEAPON_SRM,
	WEAPON_SRM_S,
	WEAPON_MILSO_S,
	WEAPON_MPN,
	WEAPON_PZ,
	WEAPON_JITTE,
	WEAPON_JITTESCOPED,
	WEAPON_GHOST,
	WEAPON_SMAC,
	WEAPON_M41,
	WEAPON_M41L,
	WEAPON_SMOKEGRENADE,
	WEAPON_REMOTEDET,
	WEAPON_KYLA,
	WEAPON_AA13,
	WEAPON_KNIFE,
	WEAPON_MX_SILENCED,
	WEAPON_SRS,
	WEAPON_M41S,

	WEAPON_MAX,		// number of weapons weapon index
};

class CNEOWeaponInfo : public FileWeaponInfo_t
{
public:
	DECLARE_CLASS_GAMEROOT( CNEOWeaponInfo, FileWeaponInfo_t );
	
	CNEOWeaponInfo();
	
	virtual void Parse( KeyValues *pKeyValuesData, const char *szWeaponName );

public:
	wchar_t m_wBulletCharacter;

	char m_szTeam2viewmodel[ MAX_WEAPON_STRING ];

	bool m_bDrawCrosshair;

	int m_iAimType;

	int m_iScopeStyle;

	float m_fAimFov;

	float m_fPenetration;

	float m_fVMFov;
	float m_fVMAimFov;

	float m_fVMOffsetUp;
	float m_fVMOffsetForward;
	float m_fVMOffsetRight;

	float m_fVMAimOffsetUp;
	float m_fVMAimOffsetForward;
	float m_fVMAimOffsetRight;

	float m_fVMAngleYaw;
	float m_fVMAnglePitch;
	float m_fVMAngleRoll;

	float m_fVMAimAngleYaw;
	float m_fVMAimAnglePitch;
	float m_fVMAimAngleRoll;

	char m_szAnim_prefix[ MAX_WEAPON_PREFIX ];

	int m_iDamage;
	int m_iBullets;

	float m_fCycleTime;

	float m_fTPMuzzleFlashScale;
};

extern CNEOWeaponInfo* GetNEOWeaponInfoFromHandle( WEAPON_FILE_INFO_HANDLE handle );

#endif // NEO_WEAPON_PARSE_H
