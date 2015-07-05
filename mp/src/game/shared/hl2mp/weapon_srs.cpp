//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: SRS sniper rifle
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
#define CWeaponSRS C_WeaponSRS
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CWeaponSRS : public CHL2MPMachineGun
{
public:
	DECLARE_CLASS( CWeaponSRS, CHL2MPMachineGun );

	CWeaponSRS();

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	void AddViewKick( void );

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
	CWeaponSRS( const CWeaponSRS & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponSRS, DT_WeaponSRS )

BEGIN_NETWORK_TABLE( CWeaponSRS, DT_WeaponSRS )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponSRS )
END_PREDICTION_DATA()

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

//=========================================================
CWeaponSRS::CWeaponSRS( )
{
	m_iFireMode = FM_SEMI;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Activity
//-----------------------------------------------------------------------------
Activity CWeaponSRS::GetPrimaryAttackActivity( void )
{
	return ACT_VM_RECOIL2;

	// TODO: Bolt action animation (ACT_VM_PULLBACK) and sound ("Weapon_SRS.Charge")
	// And animations (acttable?)
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