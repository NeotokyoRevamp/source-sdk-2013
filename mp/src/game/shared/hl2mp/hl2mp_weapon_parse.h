//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef HL2MP_WEAPON_PARSE_H
#define HL2MP_WEAPON_PARSE_H
#ifdef _WIN32
#pragma once
#endif


#include "weapon_parse.h"
#include "networkvar.h"


//--------------------------------------------------------------------------------------------------------
class CHL2MPSWeaponInfo : public FileWeaponInfo_t
{
public:
	DECLARE_CLASS_GAMEROOT( CHL2MPSWeaponInfo, FileWeaponInfo_t );
	
	CHL2MPSWeaponInfo();
	
	virtual void Parse( ::KeyValues *pKeyValuesData, const char *szWeaponName );

public:

	int m_iPlayerDamage;
	float m_flCycleTime;
	
	float m_flRecoilPitch;
	float m_flRecoilYaw;
	Vector m_vecSpread;

	float	m_flVMFov;
	Vector	m_vecVMPosOffset;
	QAngle	m_angVMAngOffset;

	int		m_iAimType;
	float	m_flAimZoom;
	float	m_flAimFov;
	Vector	m_vecAimPosOffset;
	QAngle	m_angAimAngOffset;
	
	float	m_flZoomFov;
	Vector	m_vecZoomPosOffset;
	QAngle	m_angZoomAngOffset;
};


#endif // HL2MP_WEAPON_PARSE_H
