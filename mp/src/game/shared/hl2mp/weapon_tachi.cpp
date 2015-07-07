//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Tachi, assault pistol. Base copied from weapon_pistol.cpp
//
//=============================================================================//

#include "cbase.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
#else
	#include "hl2mp_player.h"
#endif

#include "weapon_hl2mpbase.h"

#ifdef CLIENT_DLL
#define CWeaponTachi C_WeaponTachi
#endif

class CWeaponTachi : public CWeaponHL2MPBase
{
public:
	DECLARE_CLASS( CWeaponTachi, CWeaponHL2MPBase );

	CWeaponTachi(void);

	Activity GetPrimaryAttackActivity(void);

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	
#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif
private:
	CWeaponTachi( const CWeaponTachi & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponTachi, DT_WeaponTachi )

BEGIN_NETWORK_TABLE( CWeaponTachi, DT_WeaponTachi )
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( CWeaponTachi )
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( weapon_tachi, CWeaponTachi );
PRECACHE_WEAPON_REGISTER( weapon_tachi );

#ifndef CLIENT_DLL
acttable_t CWeaponTachi::m_acttable[] = 
{
	{ ACT_HL2MP_IDLE,					ACT_HL2MP_IDLE_PISTOL,					false },
	{ ACT_HL2MP_RUN,					ACT_HL2MP_RUN_PISTOL,					false },
	{ ACT_HL2MP_IDLE_CROUCH,			ACT_HL2MP_IDLE_CROUCH_PISTOL,			false },
	{ ACT_HL2MP_WALK_CROUCH,			ACT_HL2MP_WALK_CROUCH_PISTOL,			false },
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_PISTOL,	false },
	{ ACT_HL2MP_GESTURE_RELOAD,			ACT_HL2MP_GESTURE_RELOAD_PISTOL,		false },
	{ ACT_HL2MP_JUMP,					ACT_HL2MP_JUMP_PISTOL,					false },
	{ ACT_RANGE_ATTACK1,				ACT_RANGE_ATTACK_PISTOL,				false },
};

IMPLEMENT_ACTTABLE( CWeaponTachi );
#endif

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeaponTachi::CWeaponTachi( void )
{
	m_iFireMode = FM_SEMI; //TODO: Switch to auto with +special
	m_bFiresUnderwater	= true;
}

Activity CWeaponTachi::GetPrimaryAttackActivity(void)
{
	if (m_nShotsFired < 2)
		return ACT_VM_PRIMARYATTACK; 

	if (m_nShotsFired < 3)
		return ACT_VM_RECOIL1;

	if (m_nShotsFired < 4)
		return ACT_VM_RECOIL2;

	return ACT_VM_RECOIL3;
}