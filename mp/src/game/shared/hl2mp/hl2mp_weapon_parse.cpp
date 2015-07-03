//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include <KeyValues.h>
#include "hl2mp_weapon_parse.h"
#include "ammodef.h"

FileWeaponInfo_t* CreateWeaponInfo()
{
	return new CHL2MPSWeaponInfo;
}

CHL2MPSWeaponInfo::CHL2MPSWeaponInfo()
{
	m_iPlayerDamage = 0;

	// Viewmodel offset
	m_flVMFov = 54.0;
	m_flVMForward = 0.0;
	m_flVMRight = 0.0;
	m_flVMUp = 0.0;
	m_flVMYaw = 0.0;

	// Viewmodel aiming offset
	m_flVMAimFov = 54.0;
	m_flVMAimForward = 0.0;
	m_flVMAimRight = 0.0;
	m_flVMAimUp = 0.0;
}

void CHL2MPSWeaponInfo::Parse( KeyValues *pKeyValuesData, const char *szWeaponName )
{
	BaseClass::Parse( pKeyValuesData, szWeaponName );

	m_iPlayerDamage = pKeyValuesData->GetInt("damage", 0);

	m_flVMFov = pKeyValuesData->GetFloat("VMFov");
	m_flVMForward = pKeyValuesData->GetFloat("VMOffsetForward");
	m_flVMRight = pKeyValuesData->GetFloat("VMOffsetRight");
	m_flVMUp = pKeyValuesData->GetFloat("VMOffsetUp");
	m_flVMYaw = pKeyValuesData->GetFloat("VMAngleYaw");

	m_flVMAimFov = pKeyValuesData->GetFloat("VMAimFov");
	m_flVMAimForward = pKeyValuesData->GetFloat("VMAimOffsetForward");
	m_flVMAimRight = pKeyValuesData->GetFloat("VMAimOffsetRight");
	m_flVMAimUp = pKeyValuesData->GetFloat("VMAimOffsetUp");
}


