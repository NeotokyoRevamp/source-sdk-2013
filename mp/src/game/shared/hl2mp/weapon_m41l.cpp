//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: M41L sniper rifle
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
#define CWeaponM41L C_WeaponM41L
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CWeaponM41L : public CHL2MPMachineGun
{
public:
	DECLARE_CLASS( CWeaponM41L, CHL2MPMachineGun );

	CWeaponM41L();

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	void AddViewKick( void );

	float	GetFireRate( void ) { return 1.0f; }
	Activity	GetPrimaryAttackActivity( void );

	virtual const Vector& GetBulletSpread( void )
	{
		static const Vector cone = VECTOR_CONE_1DEGREES;
		return cone;
	}

#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif
	
private:
	CWeaponM41L( const CWeaponM41L & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponM41L, DT_WeaponM41L )

BEGIN_NETWORK_TABLE( CWeaponM41L, DT_WeaponM41L )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponM41L )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_m41l, CWeaponM41L );
PRECACHE_WEAPON_REGISTER(weapon_m41l);

#ifndef CLIENT_DLL
acttable_t	CWeaponM41L::m_acttable[] = 
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

IMPLEMENT_ACTTABLE(CWeaponM41L);
#endif

//=========================================================
CWeaponM41L::CWeaponM41L( )
{
	m_iFireMode = FM_SEMI;
	m_bScope = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Activity
//-----------------------------------------------------------------------------
Activity CWeaponM41L::GetPrimaryAttackActivity( void )
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
void CWeaponM41L::AddViewKick( void )
{
	#define	EASY_DAMPEN			0.5f
	#define	MAX_VERTICAL_KICK	30.0f	//Degrees
	#define	SLIDE_LIMIT			2.0f	//Seconds
	
	//Get the view kick
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if ( pPlayer == NULL )
		return;

	DoMachineGunKick( pPlayer, EASY_DAMPEN, MAX_VERTICAL_KICK, 1.0f, SLIDE_LIMIT );
}