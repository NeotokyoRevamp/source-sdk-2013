#include "cbase.h"
#include "weapon_hl2mpbase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef CLIENT_DLL
#define CWeaponDetpack C_WeaponDetpack
#endif

class CWeaponDetpack : public CWeaponHL2MPBase
{
public:
	void PrimaryAttack(void);

	DECLARE_CLASS(CWeaponDetpack, CWeaponHL2MPBase);

	CWeaponDetpack();

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CNetworkVar(bool, m_bDetonatorArmed);

#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif

private:
	CWeaponDetpack(const CWeaponDetpack &);
};

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponDetpack, DT_WeaponDetpack)

BEGIN_NETWORK_TABLE(CWeaponDetpack, DT_WeaponDetpack)
#ifdef CLIENT_DLL
RecvPropBool(RECVINFO(m_bDetonatorArmed)),
#else
SendPropBool(SENDINFO(m_bDetonatorArmed)),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CWeaponDetpack)

END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(weapon_detpack, CWeaponDetpack);
PRECACHE_WEAPON_REGISTER(weapon_detpack);

#ifndef CLIENT_DLL
acttable_t	CWeaponDetpack::m_acttable[] =
{
	{ ACT_RANGE_ATTACK1, ACT_RANGE_ATTACK_SLAM, true },
	{ ACT_HL2MP_IDLE, ACT_HL2MP_IDLE_SLAM, false },
	{ ACT_HL2MP_RUN, ACT_HL2MP_RUN_SLAM, false },
	{ ACT_HL2MP_IDLE_CROUCH, ACT_HL2MP_IDLE_CROUCH_SLAM, false },
	{ ACT_HL2MP_WALK_CROUCH, ACT_HL2MP_WALK_CROUCH_SLAM, false },
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK, ACT_HL2MP_GESTURE_RANGE_ATTACK_SLAM, false },
	{ ACT_HL2MP_GESTURE_RELOAD, ACT_HL2MP_GESTURE_RELOAD_SLAM, false },
	{ ACT_HL2MP_JUMP, ACT_HL2MP_JUMP_SLAM, false },
};

IMPLEMENT_ACTTABLE(CWeaponDetpack);
#endif

CWeaponDetpack::CWeaponDetpack()
{

}

void CWeaponDetpack::PrimaryAttack(void)
{

}
