#include "cbase.h"
#include <KeyValues.h>
#include "neo_weapon_parse.h"

CNEOWeaponInfo* GetNEOWeaponInfoFromHandle( WEAPON_FILE_INFO_HANDLE handle )
{
	return static_cast< CNEOWeaponInfo* >(GetFileWeaponInfoFromHandle( handle ));
}

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

	m_fVMFov = pKeyValuesData->GetFloat( "VMFov", 54.f );
	m_fVMAimFov = pKeyValuesData->GetFloat( "VMAimFov", 54.f );
	m_fAimFov = pKeyValuesData->GetFloat( "AimFov", 54.f );

	m_fVMOffsetUp = pKeyValuesData->GetFloat( "VMOffsetUp", 0.f );
	m_fVMOffsetForward = pKeyValuesData->GetFloat( "VMOffsetForward", 0.f );
	m_fVMOffsetRight = pKeyValuesData->GetFloat( "VMOffsetRight", 0.f );

	m_fVMAimOffsetUp = pKeyValuesData->GetFloat( "VMAimOffsetUp", 0.f );
	m_fVMAimOffsetForward = pKeyValuesData->GetFloat( "VMAimOffsetForward", 0.f );
	m_fVMAimOffsetRight = pKeyValuesData->GetFloat( "VMAimOffsetRight", 0.f );

	m_fVMAngleYaw = pKeyValuesData->GetFloat( "VMAngleYaw", 0.f );
	m_fVMAnglePitch = pKeyValuesData->GetFloat( "VMAnglePitch", 0.f );
	m_fVMAngleRoll = pKeyValuesData->GetFloat( "VMAngleRoll", 0.f );

	m_fVMAimAngleYaw = pKeyValuesData->GetFloat( "VMAimAngleYaw", 0.f );
	m_fVMAimAnglePitch = pKeyValuesData->GetFloat( "VMAimAnglePitch", 0.f );
	m_fVMAimAngleRoll = pKeyValuesData->GetFloat( "VMAimAngleRoll", 0.f );

	m_fPenetration = pKeyValuesData->GetFloat( "Penetration", 1.f );

	// These were always in here
	Q_strncpy( m_szAnim_prefix, pKeyValuesData->GetString( "anim_prefix" ), MAX_WEAPON_PREFIX );

	m_iDamage = pKeyValuesData->GetInt( "Damage", 42 );
	m_iBullets = pKeyValuesData->GetInt( "Bullets", 1 );

	m_fCycleTime = pKeyValuesData->GetFloat( "CycleTime", 0.15f );
	m_fTPMuzzleFlashScale = pKeyValuesData->GetFloat( "TPMuzzleFlashScale", 1.0f );
}