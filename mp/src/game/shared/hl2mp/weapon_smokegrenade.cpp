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
#define CWeaponSmokeGrenade C_WeaponSmokeGrenade
#endif

class CWeaponSmokeGrenade : public CWeaponHL2MPBase
{
public:
	DECLARE_CLASS(CWeaponSmokeGrenade, CWeaponHL2MPBase);

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif

	CWeaponSmokeGrenade();

	void SecondaryAttack(void) { return; }

	CWeaponSmokeGrenade(const CWeaponSmokeGrenade &);
};

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponSmokeGrenade, DT_WeaponSmokeGrenade)

BEGIN_NETWORK_TABLE(CWeaponSmokeGrenade, DT_WeaponSmokeGrenade)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CWeaponSmokeGrenade)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(weapon_smokegrenade, CWeaponSmokeGrenade);
PRECACHE_WEAPON_REGISTER(weapon_smokegrenade);

#ifndef CLIENT_DLL

acttable_t	CWeaponSmokeGrenade::m_acttable[] =
{
	{ ACT_HL2MP_IDLE, ACT_HL2MP_IDLE_GRENADE, false },
	{ ACT_HL2MP_RUN, ACT_HL2MP_RUN_GRENADE, false },
	{ ACT_HL2MP_IDLE_CROUCH, ACT_HL2MP_IDLE_CROUCH_GRENADE, false },
	{ ACT_HL2MP_WALK_CROUCH, ACT_HL2MP_WALK_CROUCH_GRENADE, false },
	{ ACT_HL2MP_JUMP, ACT_HL2MP_JUMP_GRENADE, false },
};

IMPLEMENT_ACTTABLE(CWeaponSmokeGrenade);

#endif

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CWeaponSmokeGrenade::CWeaponSmokeGrenade(void)
{

}