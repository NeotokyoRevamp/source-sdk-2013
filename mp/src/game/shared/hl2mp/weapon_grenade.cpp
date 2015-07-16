#include "cbase.h"
#include "weapon_hl2mpbase.h"
#include "in_buttons.h"

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

	void PrimaryAttack(void);
	void SecondaryAttack(void) { return; }
	void ItemPostFrame(void);

	bool Deploy(void);

private:
	CNetworkVar(bool, m_bPinPulled);
	CNetworkVar(bool, m_bThrownGrenade);

	void ThrowGrenade(void);
	void CreateGrenade(void);

	CWeaponGrenade(const CWeaponGrenade &);
};

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponGrenade, DT_WeaponGrenade)

BEGIN_NETWORK_TABLE(CWeaponGrenade, DT_WeaponGrenade)
#ifdef CLIENT_DLL
RecvPropInt(RECVINFO(m_bPinPulled)),
RecvPropInt(RECVINFO(m_bThrownGrenade)),
#else
SendPropInt(SENDINFO(m_bPinPulled)),
SendPropInt(SENDINFO(m_bThrownGrenade)),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CWeaponGrenade)
DEFINE_PRED_FIELD(m_bPinPulled, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE),
DEFINE_PRED_FIELD(m_bThrownGrenade, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE),
END_PREDICTION_DATA()
#endif

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
	m_bPinPulled = false;
	m_bThrownGrenade = false;
}

void CWeaponGrenade::PrimaryAttack(void)
{
	// Only the player fires this way so we can cast
	CBasePlayer *pOwner = ToBasePlayer(GetOwner());

	if (!pOwner || m_bPinPulled)
		return;

	SendWeaponAnim(ACT_VM_PULLPIN);

	m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();

	m_bPinPulled = true;
}

void CWeaponGrenade::ItemPostFrame(void)
{
	CBasePlayer *pOwner = ToBasePlayer(GetOwner());
	if (!pOwner)
		return;

	if (m_flNextPrimaryAttack <= gpGlobals->curtime)
	{
		// Throw animation ended and grenade is thrown
		if (m_bThrownGrenade)
		{
			// Remove weapon
			#ifndef CLIENT_DLL
			UTIL_Remove(this);
			#endif

			// Switch to another weapon
			pOwner->SwitchToNextBestWeapon(this);
			return;
		}
		// Holding +attack
		else if ((pOwner->m_nButtons & IN_ATTACK))
		{
			// Pin is not pulled yet
			if (!m_bPinPulled)
			{
				// Show to pin pull animation
				PrimaryAttack();
			}
			else
			{
				// Grenade has no pin pulled idle animation so we just hide the viewmodel here
				SetWeaponVisible(false);
			}

			return;
		}
		// -attack and pin is pulled
		else if (!(pOwner->m_nButtons & IN_ATTACK) && m_bPinPulled)
		{
			ThrowGrenade();
			return;
		}
		else
		{
			// Nothing is happening
			WeaponIdle();
		}
	}

	BaseClass::ItemPostFrame();
}

// Reset the state when picking up weapon or switching to it
bool CWeaponGrenade::Deploy()
{
	m_bPinPulled = false;

	return BaseClass::Deploy();
}

// Spawn the grenade projectile and show throw animations
void CWeaponGrenade::ThrowGrenade(void)
{
	CBasePlayer *pOwner = ToBasePlayer(GetOwner());
	if (!pOwner)
		return;

	// Throw animation for player model
	//pOwner->SetAnimation(PLAYER_ATTACK1);

	// Show view model again
	SetWeaponVisible(true);

	// Show the throw animation
	SendWeaponAnim(ACT_VM_THROW);

	CreateGrenade();

	m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();

	m_bPinPulled = false;
	m_bThrownGrenade = true;
}

void CWeaponGrenade::CreateGrenade(void)
{
	CBasePlayer *pOwner = ToBasePlayer(GetOwner());
	if (!pOwner)
		return;

#ifndef CLIENT_DLL
	// TODO: Create projectile
#endif
}