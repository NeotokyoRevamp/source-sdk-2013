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

	m_flVMFov = 54.0f;
	m_vecVMPosOffset = vec3_origin;
	m_angVMAngOffset.Init();

	m_flAimFov = 54.0f;
	m_vecAimPosOffset = vec3_origin;
	m_angAimAngOffset.Init();
}

void CHL2MPSWeaponInfo::Parse( KeyValues *pKeyValuesData, const char *szWeaponName )
{
	BaseClass::Parse(pKeyValuesData, szWeaponName);

	m_iPlayerDamage = pKeyValuesData->GetInt("damage", 0);

	KeyValues *pViewModel = pKeyValuesData->FindKey("ViewModelOffset");
	if (pViewModel)
	{
		m_flVMFov = pViewModel->GetFloat("fov", 0.0f);

		m_vecVMPosOffset.x = pViewModel->GetFloat("forward", 0.0f);
		m_vecVMPosOffset.y = pViewModel->GetFloat("right", 0.0f);
		m_vecVMPosOffset.z = pViewModel->GetFloat("up", 0.0f);

		m_angVMAngOffset[PITCH] = pViewModel->GetFloat("pitch", 0.0f);
		m_angVMAngOffset[YAW] = pViewModel->GetFloat("yaw", 0.0f);
		m_angVMAngOffset[ROLL] = pViewModel->GetFloat("roll", 0.0f);
	}

	KeyValues *pIronSight = pKeyValuesData->FindKey("IronSightOffset");
	if (pIronSight)
	{
		m_flAimFov = pIronSight->GetFloat("fov", 0.0f);

		m_vecAimPosOffset.x = pIronSight->GetFloat("forward", 0.0f);
		m_vecAimPosOffset.y = pIronSight->GetFloat("right", 0.0f);
		m_vecAimPosOffset.z = pIronSight->GetFloat("up", 0.0f);

		m_angAimAngOffset[PITCH] = pIronSight->GetFloat("pitch", 0.0f);
		m_angAimAngOffset[YAW] = pIronSight->GetFloat("yaw", 0.0f);
		m_angAimAngOffset[ROLL] = pIronSight->GetFloat("roll", 0.0f);
	}
}
