//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: AA13 automatic shotgun
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
#define CWeaponAA13 C_WeaponAA13
#endif

extern ConVar sk_aa13_auto_reload_time;
extern ConVar sk_aa13_plr_num_shotgun_pellets;

class CWeaponAA13 : public CBaseHL2MPCombatWeapon
{
public:
	DECLARE_CLASS( CWeaponAA13, CBaseHL2MPCombatWeapon );

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

public:
	virtual const Vector& GetBulletSpread( void )
	{
		static Vector cone = VECTOR_CONE_10DEGREES;
		return cone;
	}

	bool Reload( void );
	void FillClip( void );
	void CheckHolsterReload( void );
//	void WeaponIdle( void );
	void PrimaryAttack( void );
	void DryFire( void );
	virtual float GetFireRate( void ) { return 0.7; };

#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif

	CWeaponAA13(void);

private:
	CWeaponAA13( const CWeaponAA13 & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponAA13, DT_WeaponAA13 )

BEGIN_NETWORK_TABLE( CWeaponAA13, DT_WeaponAA13 )
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( CWeaponAA13 )
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( weapon_aa13, CWeaponAA13 );
PRECACHE_WEAPON_REGISTER(weapon_aa13);

#ifndef CLIENT_DLL
acttable_t	CWeaponAA13::m_acttable[] = 
{
	{ ACT_HL2MP_IDLE,					ACT_HL2MP_IDLE_SHOTGUN,					false },
	{ ACT_HL2MP_RUN,					ACT_HL2MP_RUN_SHOTGUN,					false },
	{ ACT_HL2MP_IDLE_CROUCH,			ACT_HL2MP_IDLE_CROUCH_SHOTGUN,			false },
	{ ACT_HL2MP_WALK_CROUCH,			ACT_HL2MP_WALK_CROUCH_SHOTGUN,			false },
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SHOTGUN,	false },
	{ ACT_HL2MP_GESTURE_RELOAD,			ACT_HL2MP_GESTURE_RELOAD_SHOTGUN,		false },
	{ ACT_HL2MP_JUMP,					ACT_HL2MP_JUMP_SHOTGUN,					false },
	{ ACT_RANGE_ATTACK1,				ACT_RANGE_ATTACK_SHOTGUN,				false },
};

IMPLEMENT_ACTTABLE(CWeaponAA13);

#endif

//-----------------------------------------------------------------------------
// Purpose: Mag style reload, not individual shells
// Input  :
// Output :
//-----------------------------------------------------------------------------
bool CWeaponAA13::Reload( void )
{
	return BaseClass::Reload();
}

//-----------------------------------------------------------------------------
// Purpose: 
//
//
//-----------------------------------------------------------------------------
void CWeaponAA13::DryFire( void )
{
	WeaponSound(EMPTY);
	SendWeaponAnim( ACT_VM_DRYFIRE );
	
	m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();
}

//-----------------------------------------------------------------------------
// Purpose: 
//
//
//-----------------------------------------------------------------------------
void CWeaponAA13::PrimaryAttack( void )
{
	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if (!pPlayer)
	{
		return;
	}

	// MUST call sound before removing a round from the clip of a CMachineGun
	WeaponSound(SINGLE);

	pPlayer->DoMuzzleFlash();

	SendWeaponAnim( ACT_VM_PRIMARYATTACK );

	// Don't fire again until fire animation has completed
	m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();
	m_iClip1 -= 1;

	// player "shoot" animation
	pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector	vecSrc		= pPlayer->Weapon_ShootPosition( );
	Vector	vecAiming	= pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );	

	FireBulletsInfo_t info( 7, vecSrc, vecAiming, GetBulletSpread(), MAX_TRACE_LENGTH, m_iPrimaryAmmoType );
	info.m_pAttacker = pPlayer;

	// Fire the bullets, and force the first shot to be perfectly accuracy
	pPlayer->FireBullets( info );
	
	QAngle punch;
	punch.Init( SharedRandomFloat( "shotgunpax", -2, -1 ), SharedRandomFloat( "shotgunpay", -2, 2 ), 0 );
	pPlayer->ViewPunch( punch );

	if (!m_iClip1 && pPlayer->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
	{
		// HEV suit - indicate out of ammo condition
		pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0); 
	}
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeaponAA13::CWeaponAA13( void )
{
	m_bReloadsSingly = false;

	m_fMinRange1		= 0.0;
	m_fMaxRange1		= 500;
	m_fMinRange2		= 0.0;
	m_fMaxRange2		= 200;
}
