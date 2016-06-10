//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include <KeyValues.h>
#include "neo_weapon_parse.h"

FileWeaponInfo_t* CreateWeaponInfo()
{
	return new CNEOWeaponInfo;
}


CNEOWeaponInfo::CNEOWeaponInfo()
{
}


void CNEOWeaponInfo::Parse( KeyValues *pKeyValuesData, const char *szWeaponName )
{
	BaseClass::Parse( pKeyValuesData, szWeaponName );

	// These instructions below were in FileWeaponInfo_t::Parse. I moved these here because that's where they belong (and because I couldn't access the new members). -Ochii
	wcsncpy( &m_wBulletCharacter, pKeyValuesData->GetWString( "BulletCharacter" ), 1 );

	m_bDrawCrosshair = ( pKeyValuesData->GetInt( "DrawCrosshair", 0 ) != 0 ) ? true : false;

	m_iAimType = pKeyValuesData->GetInt( "AimType", 1 );
	m_iScopeStyle = pKeyValuesData->GetInt( "ScopeStyle", 0 );	

	m_fVMFov = pKeyValuesData->GetInt( "VMFov", 54.f );
	m_fVMAimFov = pKeyValuesData->GetInt( "VMAimFov", 54.f );
	m_fAimFov = pKeyValuesData->GetInt( "AimFov", 54.f );

	m_fVMOffsetUp = pKeyValuesData->GetInt( "VMOffsetUp", 0.f );
	m_fVMOffsetForward = pKeyValuesData->GetInt( "VMOffsetForward", 0.f );
	m_fVMOffsetRight = pKeyValuesData->GetInt( "VMOffsetRight", 0.f );

	m_fVMAimOffsetUp = pKeyValuesData->GetInt( "VMAimOffsetUp", 0.f );
	m_fVMAimOffsetForward = pKeyValuesData->GetInt( "VMAimOffsetForward", 0.f );
	m_fVMAimOffsetRight = pKeyValuesData->GetInt( "VMAimOffsetRight", 0.f );

	m_fVMAngleYaw = pKeyValuesData->GetInt( "VMAngleYaw", 0.f );
	m_fVMAnglePitch = pKeyValuesData->GetInt( "VMAnglePitch", 0.f );
	m_fVMAngleRoll = pKeyValuesData->GetInt( "VMAngleRoll", 0.f );

	m_fVMAimAngleYaw = pKeyValuesData->GetInt( "VMAimAngleYaw", 0.f );
	m_fVMAimAnglePitch = pKeyValuesData->GetInt( "VMAimAnglePitch", 0.f );
	m_fVMAimAngleRoll = pKeyValuesData->GetInt( "VMAimAngleRoll", 0.f );

	m_fPenetration = pKeyValuesData->GetInt( "Penetration", 1.f );

	// These were always in here
	Q_strncpy( m_szAnim_prefix, pKeyValuesData->GetString( "anim_prefix" ), MAX_WEAPON_PREFIX );

	m_iDamage = pKeyValuesData->GetInt( "Damage", 42 );
	m_iBullets = pKeyValuesData->GetInt( "Bullets", 1 );

	m_fCycleTime = pKeyValuesData->GetInt( "CycleTime", 0.15f );
	m_fTPMuzzleFlashScale = pKeyValuesData->GetInt( "TPMuzzleFlashScale", 1.0f );
}

// Check the header for more info -Ochii
/*virtual const char* CNEOWeaponInfo::GetClassName()
{
	return "CNEOWeaponInfo";
}*/