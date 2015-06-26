
//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Kyla revolver. Base copied from hl2mp's weapon_kyla.cpp
//
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "in_buttons.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
#else
	#include "hl2mp_player.h"
#endif

#include "weapon_hl2mpbasehlmpcombatweapon.h"

#ifdef CLIENT_DLL
#define CWeaponKyla C_WeaponKyla
#endif

//-----------------------------------------------------------------------------
// CWeaponKyla
//-----------------------------------------------------------------------------

class CWeaponKyla : public CBaseHL2MPCombatWeapon
{
	DECLARE_CLASS( CWeaponKyla, CBaseHL2MPCombatWeapon );
public:

	CWeaponKyla( void );

	void	PrimaryAttack( void );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif

private:
	
	CWeaponKyla( const CWeaponKyla & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponKyla, DT_WeaponKyla )

BEGIN_NETWORK_TABLE( CWeaponKyla, DT_WeaponKyla )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponKyla )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_kyla, CWeaponKyla );
PRECACHE_WEAPON_REGISTER( weapon_kyla );


#ifndef CLIENT_DLL
acttable_t CWeaponKyla::m_acttable[] = 
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



IMPLEMENT_ACTTABLE( CWeaponKyla );

#endif

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeaponKyla::CWeaponKyla( void )
{
	m_bReloadsSingly	= false;
	m_bFiresUnderwater	= false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponKyla::PrimaryAttack( void )
{
	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if ( !pPlayer )
	{
		return;
	}

	if ( m_iClip1 <= 0 )
	{
		if ( !m_bFireOnEmpty )
		{
			Reload();
		}
		else
		{
			WeaponSound( EMPTY );
			m_flNextPrimaryAttack = 0.15;
		}

		return;
	}

	WeaponSound( SINGLE );
	pPlayer->DoMuzzleFlash();

	SendWeaponAnim( ACT_VM_PRIMARYATTACK );
	pPlayer->SetAnimation( PLAYER_ATTACK1 );

	float f_rateOfFire = 0.35; // Kyla's RoF in Neotokyo: 0.35 seconds / 1 shot

	m_flNextPrimaryAttack = gpGlobals->curtime + f_rateOfFire;
	m_flNextSecondaryAttack = gpGlobals->curtime + f_rateOfFire;

	m_iClip1--;

	Vector vecSrc		= pPlayer->Weapon_ShootPosition();
	Vector vecAiming	= pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );	

	FireBulletsInfo_t info( 1, vecSrc, vecAiming, vec3_origin, MAX_TRACE_LENGTH, m_iPrimaryAmmoType );
	info.m_pAttacker = pPlayer;

	// Fire the bullets, and force the first shot to be perfectly accuracy
	pPlayer->FireBullets( info );

	//Disorient the player
	QAngle angles = pPlayer->GetLocalAngles();

	angles.x += random->RandomInt( -0.1, 0.1 );
	angles.y += random->RandomInt( -0.1, 0.1 );
	angles.z = 0;

#ifndef CLIENT_DLL
	pPlayer->SnapEyeAngles( angles );
#endif

	// Some view punch, somefully not too jarring.
	// It's probably better to just increase inaccuracy instead of too much of this. (Rain)
	pPlayer->ViewPunch( QAngle( -2, random->RandomFloat( -0.2, 0.2 ), 0 ) );

	if ( !m_iClip1 && pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) <= 0 )
	{
		// HEV suit - indicate out of ammo condition
		pPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 ); 
	}
}
