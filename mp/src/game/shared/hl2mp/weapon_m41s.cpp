//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: M41S semi automatic silenced rifle
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
#define CWeaponM41S C_WeaponM41S
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CWeaponM41S : public CHL2MPMachineGun
{
public:
	DECLARE_CLASS( CWeaponM41S, CHL2MPMachineGun );

	CWeaponM41S();

	virtual bool	Reload( void );


	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	void SecondaryAttack(void);
	void AddViewKick( void );
	bool Holster(CBaseCombatWeapon *pSwitchingTo = NULL);
	void Drop(const Vector &vecVelocity);

	float	GetFireRate( void ) { return 0.15f; }
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
	CWeaponM41S( const CWeaponM41S & );

	void	ToggleZoom( void );
	void	CancelZoom( void );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponM41S, DT_WeaponM41S )

BEGIN_NETWORK_TABLE( CWeaponM41S, DT_WeaponM41S )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponM41S )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_m41s, CWeaponM41S );
PRECACHE_WEAPON_REGISTER(weapon_m41s);

#ifndef CLIENT_DLL
acttable_t	CWeaponM41S::m_acttable[] = 
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

IMPLEMENT_ACTTABLE(CWeaponM41S);
#endif

//=========================================================
CWeaponM41S::CWeaponM41S( )
{

}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Activity
//-----------------------------------------------------------------------------
Activity CWeaponM41S::GetPrimaryAttackActivity( void )
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
void CWeaponM41S::SecondaryAttack(void)
{
	ToggleZoom();
}

void CWeaponM41S::ToggleZoom( void )
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
}

void CWeaponM41S::CancelZoom( void )
{
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer == NULL)
		return;

	if (pPlayer->GetFOV() != pPlayer->GetDefaultFOV())
	{
		pPlayer->SetFOV(pPlayer, pPlayer->GetDefaultFOV(), 0.2f);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponM41S::AddViewKick( void )
{
	#define	EASY_DAMPEN			0.5f
	#define	MAX_VERTICAL_KICK	7.5f	//Degrees
	#define	SLIDE_LIMIT			1.0f	//Seconds
	
	//Get the view kick
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if ( pPlayer == NULL )
		return;

	DoMachineGunKick( pPlayer, EASY_DAMPEN, MAX_VERTICAL_KICK, 5.0f, SLIDE_LIMIT );
}

// Disable scope when releading
bool CWeaponM41S::Reload( void )
{
	if ( BaseClass::Reload() )
	{
		CancelZoom();

		return true;
	}

	return false;
}

// Disable scope when switching weapons
bool CWeaponM41S::Holster(CBaseCombatWeapon *pSwitchingTo)
{
	CancelZoom();

	return BaseClass::Holster(pSwitchingTo);
}

void CWeaponM41S::Drop(const Vector &vecVelocity)
{
	CancelZoom();

	BaseClass::Drop(vecVelocity);
}