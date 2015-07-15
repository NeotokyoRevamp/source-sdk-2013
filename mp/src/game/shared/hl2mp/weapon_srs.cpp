#include "cbase.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
#else
	#include "hl2mp_player.h"
#endif

#include "weapon_hl2mpbase.h"

#ifdef CLIENT_DLL
#define CWeaponSRS C_WeaponSRS
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CWeaponSRS : public CWeaponHL2MPBase
{
public:
	DECLARE_CLASS(CWeaponSRS, CWeaponHL2MPBase);

	CWeaponSRS();

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	void AddViewKick(void);
	void ItemPostFrame(void);

	Activity	GetPrimaryAttackActivity(void);

#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif
	
private:
	CNetworkVar(bool, m_bBoltPull);

	void BoltPull(void);

	CWeaponSRS(const CWeaponSRS &);
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponSRS, DT_WeaponSRS )

BEGIN_NETWORK_TABLE( CWeaponSRS, DT_WeaponSRS )
#ifdef CLIENT_DLL
RecvPropInt(RECVINFO(m_bBoltPull)),
#else
SendPropInt(SENDINFO(m_bBoltPull)),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( CWeaponSRS )
DEFINE_PRED_FIELD(m_bBoltPull, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE),
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( weapon_srs, CWeaponSRS );
PRECACHE_WEAPON_REGISTER(weapon_srs);

#ifndef CLIENT_DLL
acttable_t	CWeaponSRS::m_acttable[] = 
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

IMPLEMENT_ACTTABLE(CWeaponSRS);
#endif

CWeaponSRS::CWeaponSRS( )
{
	m_iFireMode = FM_SEMI;

	m_bBoltPull = false;
}

Activity CWeaponSRS::GetPrimaryAttackActivity( void )
{
	// Called by PrimaryAttack to get recoil animation so we can just set boltpull here
	m_bBoltPull = true;

	return ACT_VM_RECOIL2;
}

void CWeaponSRS::BoltPull(void)
{
	SendWeaponAnim(ACT_VM_PULLBACK);
	WeaponSound(SPECIAL1);

	m_flNextPrimaryAttack = gpGlobals->curtime + (GetFireRate() - 0.05);

	m_bBoltPull = false;
}

void CWeaponSRS::ItemPostFrame(void)
{
	// Start bolt animation 0.05 seconds after shooting
	if (m_flNextPrimaryAttack <= (gpGlobals->curtime + 0.95) && m_bBoltPull)
	{
		BoltPull();

		// Do nothing else this frame
		return;
	}

	BaseClass::ItemPostFrame();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponSRS::AddViewKick( void )
{
	#define	EASY_DAMPEN			0.5f
	#define	MAX_VERTICAL_KICK	30.0f	//Degrees
	#define	SLIDE_LIMIT			2.0f	//Seconds
	
	//Get the view kick
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if ( pPlayer == NULL )
		return;

	DoMachineGunKick( pPlayer, EASY_DAMPEN, MAX_VERTICAL_KICK, 5.0f, SLIDE_LIMIT );
}