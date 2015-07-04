#include "cbase.h"
#include "in_buttons.h"
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
	void ItemPostFrame(void);
	void WeaponIdle(void);
	void Drop(const Vector &vecVelocity);

	bool Holster(CBaseCombatWeapon *pSwitchingTo);
	bool Deploy(void);
	
	DECLARE_CLASS(CWeaponDetpack, CWeaponHL2MPBase);

	CWeaponDetpack();

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CNetworkVar(bool, m_bArming);
	CNetworkVar(bool, m_bDetonatorArmed);

#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
	DECLARE_DATADESC();
#endif

private:
	CWeaponDetpack(const CWeaponDetpack &);

	void StartDropDetpack(void);
	void DropDetpack(void);
	void DetonateDetpack(void);
};

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponDetpack, DT_WeaponDetpack)

BEGIN_NETWORK_TABLE(CWeaponDetpack, DT_WeaponDetpack)
#ifdef CLIENT_DLL
RecvPropBool(RECVINFO(m_bArming)),
RecvPropBool(RECVINFO(m_bDetonatorArmed)),
#else
SendPropBool(SENDINFO(m_bArming)),
SendPropBool(SENDINFO(m_bDetonatorArmed)),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CWeaponDetpack)
DEFINE_PRED_FIELD(m_bArming, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE),
DEFINE_PRED_FIELD(m_bDetonatorArmed, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE),
END_PREDICTION_DATA()
#endif 

LINK_ENTITY_TO_CLASS(weapon_detpack, CWeaponDetpack);
PRECACHE_WEAPON_REGISTER(weapon_detpack);

#ifndef CLIENT_DLL

BEGIN_DATADESC(CWeaponDetpack)
DEFINE_FIELD(m_bArming, FIELD_BOOLEAN),
DEFINE_FIELD(m_bDetonatorArmed, FIELD_BOOLEAN),
END_DATADESC()

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
	m_bArming = false;
	m_bDetonatorArmed = false;
}

void CWeaponDetpack::PrimaryAttack(void)
{
	// Only the player fires this way so we can cast
	CBasePlayer *pOwner = ToBasePlayer(GetOwner());

	if (!pOwner || m_bArming)
		return;

	// player "shoot" animation
	pOwner->SetAnimation(PLAYER_ATTACK1);

	if (m_bDetonatorArmed)
	{
		return DetonateDetpack();
	}
	else 
	{
		return StartDropDetpack();
	}
}

void CWeaponDetpack::StartDropDetpack(void)
{
	if (m_bDetonatorArmed || m_bArming)
		return;

	m_bArming = true;

	WeaponSound(SINGLE);
	SendWeaponAnim(ACT_VM_PRIMARYATTACK);

	m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();
}

void CWeaponDetpack::DropDetpack(void)
{
	if (!m_bArming || m_bDetonatorArmed)
		return;

	m_bArming = false;
	m_bDetonatorArmed = true;

	SendWeaponAnim(ACT_VM_THROW);

	// TODO: Actually create grenade_detpack

	m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();
}

void CWeaponDetpack::DetonateDetpack(void)
{
	if (!m_bDetonatorArmed)
		return;

	SendWeaponAnim(ACT_VM_PRIMARYATTACK_DEPLOYED);

	// TODO: Detonate grenade_detpack
	WeaponSound(SPECIAL1); // Placeholder sound for the much needed explosion sound!

	m_bDetonatorArmed = false;
	m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();
}

void CWeaponDetpack::ItemPostFrame(void)
{
	CBasePlayer *pOwner = ToBasePlayer(GetOwner());
	if (!pOwner)
		return;

	if (m_flNextPrimaryAttack <= gpGlobals->curtime)
	{
		if (m_bArming)
		{
			// Detpack is ready to be dropped
			DropDetpack();
		}
		else if ((pOwner->m_nButtons & IN_ATTACK))
		{
			PrimaryAttack();
		}
		else
		{
			// Nothing is happening
			WeaponIdle();
		}
	}
}

void CWeaponDetpack::WeaponIdle(void)
{
	if (m_bDetonatorArmed)
	{
		SendWeaponAnim(ACT_VM_IDLE_DEPLOYED);
	}
	else
	{
		SendWeaponAnim(ACT_VM_IDLE);
	}
}

void CWeaponDetpack::Drop(const Vector &vecVelocity)
{
	m_bArming = false;

	// Stop beeping when dropping
	StopWeaponSound(SINGLE);

	BaseClass::Drop(vecVelocity);
}

bool CWeaponDetpack::Holster(CBaseCombatWeapon *pSwitchingTo)
{
	m_bArming = false;

	// Stop beeping when switching away
	StopWeaponSound(SINGLE);

	return BaseClass::Holster(pSwitchingTo);
}

bool CWeaponDetpack::Deploy(void)
{
	m_bArming = false;

	Activity iActivity;

	if (m_bDetonatorArmed)
		iActivity = ACT_VM_DRAW_DEPLOYED;
	else
		iActivity = ACT_VM_DRAW;

	return DefaultDeploy((char*)GetViewModel(), (char*)GetWorldModel(), iActivity, (char*)GetAnimPrefix());
}