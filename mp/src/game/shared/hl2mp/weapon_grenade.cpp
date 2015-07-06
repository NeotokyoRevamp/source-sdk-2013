#include "cbase.h"
#include "weapon_hl2mpbase.h"

#if defined( CLIENT_DLL )
#include "c_hl2mp_player.h"
#else
#include "hl2mp_player.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef CLIENT_DLL
#define CWeaponGrenade C_WeaponGrenade
#endif

class CWeaponGrenade : public CWeaponHL2MPBase
{
public:
	DECLARE_CLASS(CWeaponGrenade, CWeaponHL2MPBase);

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif

	CWeaponGrenade();

	void SecondaryAttack(void) { return; }

	CWeaponGrenade(const CWeaponGrenade &);
};

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponGrenade, DT_WeaponGrenade)

BEGIN_NETWORK_TABLE(CWeaponGrenade, DT_WeaponGrenade)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CWeaponGrenade)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(weapon_grenade, CWeaponGrenade);
PRECACHE_WEAPON_REGISTER(weapon_grenade);

#ifndef CLIENT_DLL

acttable_t	CWeaponGrenade::m_acttable[] =
{
	{ ACT_HL2MP_IDLE, ACT_HL2MP_IDLE_GRENADE, false },
	{ ACT_HL2MP_RUN, ACT_HL2MP_RUN_GRENADE, false },
	{ ACT_HL2MP_IDLE_CROUCH, ACT_HL2MP_IDLE_CROUCH_GRENADE, false },
	{ ACT_HL2MP_WALK_CROUCH, ACT_HL2MP_WALK_CROUCH_GRENADE, false },
	{ ACT_HL2MP_JUMP, ACT_HL2MP_JUMP_GRENADE, false },
};

IMPLEMENT_ACTTABLE(CWeaponGrenade);

#endif

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CWeaponGrenade::CWeaponGrenade(void)
{

}