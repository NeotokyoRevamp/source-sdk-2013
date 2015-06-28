//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Neotokyo knife. Copies base code of the crowbar.
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "weapon_hl2mpbasehlmpcombatweapon.h"
#include "weapon_hl2mpbasebasebludgeon.h"

#if defined( CLIENT_DLL )
	#include "c_hl2mp_player.h"
#else
	#include "hl2mp_player.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define	KNIFE_RANGE	 48.0f + 3.0f // Neotokyo range is 48 units ish. However, we need about 3 units more for the NT reach, possibly because viewmodel positioning? (Rain)
#define	KNIFE_REFIRE 0.534f // Neotokyo refire rate is ~0.534 seconds
#define KNIFE_DAMAGE 25.0f // Assault class base damage of 25hp

#ifdef CLIENT_DLL
#define CWeaponKnife C_WeaponKnife
#endif

//-----------------------------------------------------------------------------
// CWeaponKnife
//-----------------------------------------------------------------------------

class CWeaponKnife : public CBaseHL2MPBludgeonWeapon
{
public:
	DECLARE_CLASS(CWeaponKnife, CBaseHL2MPBludgeonWeapon);

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif

	CWeaponKnife();

	float		GetRange(void) { return	KNIFE_RANGE; }
	float		GetFireRate(void) { return	KNIFE_REFIRE; }
	float		GetDamageForActivity(Activity hitActivity) { return KNIFE_DAMAGE; }
	void		PrimaryAttack(void);
	void		SecondaryAttack(void)	{ return; }
	
	void		Drop(const Vector &vecVelocity);

	CWeaponKnife(const CWeaponKnife &);
};

//-----------------------------------------------------------------------------
// CWeaponKnife
//-----------------------------------------------------------------------------

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponKnife, DT_WeaponKnife )

BEGIN_NETWORK_TABLE( CWeaponKnife, DT_WeaponKnife )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponKnife )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_knife, CWeaponKnife );
PRECACHE_WEAPON_REGISTER( weapon_knife );

#ifndef CLIENT_DLL

// TODO: Animations dun goof'd, except the idle animation. (Rain)
acttable_t	CWeaponKnife::m_acttable[] = 
{
	{ ACT_RANGE_ATTACK1,				ACT_RANGE_ATTACK_SLAM, true },
	{ ACT_HL2MP_IDLE,					ACT_HL2MP_IDLE_MELEE,					false },
	{ ACT_HL2MP_RUN,					ACT_HL2MP_RUN_MELEE,					false },
	{ ACT_HL2MP_IDLE_CROUCH,			ACT_HL2MP_IDLE_CROUCH_MELEE,			false },
	{ ACT_HL2MP_WALK_CROUCH,			ACT_HL2MP_WALK_CROUCH_MELEE,			false },
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_MELEE,	false },
	{ ACT_HL2MP_GESTURE_RELOAD,			ACT_HL2MP_GESTURE_RELOAD_MELEE,			false },
	{ ACT_HL2MP_JUMP,					ACT_HL2MP_JUMP_MELEE,					false },
};

IMPLEMENT_ACTTABLE(CWeaponKnife);

#endif

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CWeaponKnife::CWeaponKnife( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: Removes weapon when it's dropped
//-----------------------------------------------------------------------------
void CWeaponKnife::Drop( const Vector &vecVelocity )
{
#ifndef CLIENT_DLL
	UTIL_Remove( this );
#endif
}

void CWeaponKnife::PrimaryAttack()
{
	BaseClass::PrimaryAttack();

	CBasePlayer *pOwner = ToBasePlayer(GetOwner());
	if (!pOwner)
		return;

	// Knife model doesn't have HIT or MISS animation so we have to show attack animation manually
	SendWeaponAnim(ACT_VM_PRIMARYATTACK);

	//Setup our next attack times
	m_flNextPrimaryAttack = gpGlobals->curtime + GetFireRate();
	m_flNextSecondaryAttack = gpGlobals->curtime + SequenceDuration();
}