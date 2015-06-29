//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: ZR68L sniper rifle
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
#define CWeaponZRLong C_WeaponZRLong
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CWeaponZRLong : public CHL2MPMachineGun
{
public:
	DECLARE_CLASS( CWeaponZRLong, CHL2MPMachineGun );

	CWeaponZRLong();

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	void	SecondaryAttack(void);
	void	AddViewKick( void );

	int		GetMinBurst() { return 2; }
	int		GetMaxBurst() { return 5; }

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
	CWeaponZRLong( const CWeaponZRLong & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponZRLong, DT_WeaponZRLong )

BEGIN_NETWORK_TABLE( CWeaponZRLong, DT_WeaponZRLong )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponZRLong )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_zr68l, CWeaponZRLong );
PRECACHE_WEAPON_REGISTER(weapon_zr68l);

#ifndef CLIENT_DLL
acttable_t	CWeaponZRLong::m_acttable[] = 
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

IMPLEMENT_ACTTABLE(CWeaponZRLong);
#endif

//=========================================================
CWeaponZRLong::CWeaponZRLong( )
{

}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Activity
//-----------------------------------------------------------------------------
Activity CWeaponZRLong::GetPrimaryAttackActivity( void )
{
	if ( m_nShotsFired < 2 )
		return ACT_VM_PRIMARYATTACK;

	if ( m_nShotsFired < 3 )
		return ACT_VM_RECOIL1;
	
	if ( m_nShotsFired < 4 )
		return ACT_VM_RECOIL2;

	return ACT_VM_RECOIL3;
}

// Zoom view by changing FOV
void CWeaponZRLong::SecondaryAttack(void)
{
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer == NULL)
		return;

	if (pPlayer->GetFOV() == pPlayer->GetDefaultFOV())
	{
		pPlayer->SetFOV(pPlayer, 40, 0.2f);
	}
	else
	{
		pPlayer->SetFOV(pPlayer, pPlayer->GetDefaultFOV(), 0.2f);
	}

	m_flNextSecondaryAttack = gpGlobals->curtime + 0.5f;

	// TODO: Show scope
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponZRLong::AddViewKick( void )
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