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
	m_flCycleTime = 0.0; // Rate of fire

	m_flVMFov = 54.0f; // Viewmodel fov without sights
	m_flAimFov = 54.0f; //  Aimed viewmodel FOV with ironsigths
	m_flZoomFov = 54.0f; //	Aimed viewmodel FOV with classic zoom

	m_iAimType = 0; // 0 - no aiming, 1 - ironsights, 2 - scope
	m_flAimZoom = 75.0f; // Game fov for zoom effect

	// Default viewmodel offset
	m_vecVMPosOffset = vec3_origin;
	m_angVMAngOffset.Init();

	// Iron sight offset
	m_vecAimPosOffset = vec3_origin;
	m_angAimAngOffset.Init();

	// Classic sight offset
	m_vecZoomPosOffset = vec3_origin;
	m_angZoomAngOffset.Init();
}

void CHL2MPSWeaponInfo::Parse( KeyValues *pKeyValuesData, const char *szWeaponName )
{
	BaseClass::Parse(pKeyValuesData, szWeaponName);

	m_iPlayerDamage = pKeyValuesData->GetInt("damage", 0);
	m_flCycleTime = pKeyValuesData->GetFloat("cycletime", 0.0f);

	KeyValues *pViewModel = pKeyValuesData->FindKey("ViewModelData");
	if (pViewModel)
	{
		m_flVMFov = pViewModel->GetFloat("fov", 54.0f);

		m_vecVMPosOffset.x = pViewModel->GetFloat("forward", 0.0f);
		m_vecVMPosOffset.y = pViewModel->GetFloat("right", 0.0f);
		m_vecVMPosOffset.z = pViewModel->GetFloat("up", 0.0f);

		m_angVMAngOffset[PITCH] = pViewModel->GetFloat("pitch", 0.0f);
		m_angVMAngOffset[YAW] = pViewModel->GetFloat("yaw", 0.0f);
		m_angVMAngOffset[ROLL] = pViewModel->GetFloat("roll", 0.0f);
	}

	KeyValues *pIronSight = pKeyValuesData->FindKey("IronSightData");
	if (pIronSight)
	{
		m_iAimType = pIronSight->GetInt("type", 0);
		m_flAimZoom = pIronSight->GetFloat("aimfov", 75.0f);

		m_flAimFov = pIronSight->GetFloat("fov", 54.0f);

		m_vecAimPosOffset.x = pIronSight->GetFloat("forward", 0.0f);
		m_vecAimPosOffset.y = pIronSight->GetFloat("right", 0.0f);
		m_vecAimPosOffset.z = pIronSight->GetFloat("up", 0.0f);

		m_angAimAngOffset[PITCH] = pIronSight->GetFloat("pitch", 0.0f);
		m_angAimAngOffset[YAW] = pIronSight->GetFloat("yaw", 0.0f);
		m_angAimAngOffset[ROLL] = pIronSight->GetFloat("roll", 0.0f);
	}

	KeyValues *pZoom = pKeyValuesData->FindKey("ZoomData");
	if (pZoom)
	{
		m_flZoomFov = pZoom->GetFloat("fov", 54.0f);

		m_vecZoomPosOffset.x = pZoom->GetFloat("forward", 0.0f);
		m_vecZoomPosOffset.y = pZoom->GetFloat("right", 0.0f);
		m_vecZoomPosOffset.z = pZoom->GetFloat("up", 0.0f);

		m_angZoomAngOffset[PITCH] = pZoom->GetFloat("pitch", 0.0f);
		m_angZoomAngOffset[YAW] = pZoom->GetFloat("yaw", 0.0f);
		m_angZoomAngOffset[ROLL] = pZoom->GetFloat("roll", 0.0f);
	}
}
