//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Jitte submachinegun
//
//=============================================================================//

#include "cbase.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
#else
	#include "hl2mp_player.h"
#endif

#include "weapon_hl2mpbase.h"
#include "weapon_hl2mpbase_machinegun.h"

#ifdef CLIENT_DLL
#define CWeaponJitte C_WeaponJitte
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CWeaponJitte : public CHL2MPMachineGun
{
public:
	DECLARE_CLASS( CWeaponJitte, CHL2MPMachineGun );

	CWeaponJitte();

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	void	AddViewKick( void );

	Activity	GetPrimaryAttackActivity( void );

#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif
	
private:
	CWeaponJitte( const CWeaponJitte & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponJitte, DT_WeaponJitte )

BEGIN_NETWORK_TABLE( CWeaponJitte, DT_WeaponJitte )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponJitte )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_jitte, CWeaponJitte );
PRECACHE_WEAPON_REGISTER(weapon_jitte);

#ifndef CLIENT_DLL
acttable_t	CWeaponJitte::m_acttable[] = 
{
	{ ACT_HL2MP_IDLE,					ACT_HL2MP_IDLE_SMG1,					false },
	{ ACT_HL2MP_RUN,					ACT_HL2MP_RUN_SMG1,						false },
	{ ACT_HL2MP_IDLE_CROUCH,			ACT_HL2MP_IDLE_CROUCH_SMG1,				false },
	{ ACT_HL2MP_WALK_CROUCH,			ACT_HL2MP_WALK_CROUCH_SMG1,				false },
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SMG1,	false },
	{ ACT_HL2MP_GESTURE_RELOAD,			ACT_HL2MP_GESTURE_RELOAD_SMG1,			false },
	{ ACT_HL2MP_JUMP,					ACT_HL2MP_JUMP_SMG1,					false },
	{ ACT_RANGE_ATTACK1,				ACT_RANGE_ATTACK_SMG1,					false },
};

IMPLEMENT_ACTTABLE(CWeaponJitte);
#endif

//=========================================================
CWeaponJitte::CWeaponJitte( )
{

}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Activity
//-----------------------------------------------------------------------------
Activity CWeaponJitte::GetPrimaryAttackActivity( void )
{
	if ( m_nShotsFired < 2 )
		return ACT_VM_PRIMARYATTACK;

	if ( m_nShotsFired < 3 )
		return ACT_VM_RECOIL1;
	
	if ( m_nShotsFired < 4 )
		return ACT_VM_RECOIL2;

	return ACT_VM_RECOIL3;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponJitte::AddViewKick( void )
{
	#define	EASY_DAMPEN			0.5f
	#define	MAX_VERTICAL_KICK	1.0f	//Degrees
	#define	SLIDE_LIMIT			2.0f	//Seconds
	
	//Get the view kick
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if ( pPlayer == NULL )
		return;

	DoMachineGunKick( pPlayer, EASY_DAMPEN, MAX_VERTICAL_KICK, m_fFireDuration, SLIDE_LIMIT );
}