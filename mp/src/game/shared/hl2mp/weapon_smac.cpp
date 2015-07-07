//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: SMAC machine pistol(?)
//
// TODO: Fix reload animation
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
#define CWeaponSMAC C_WeaponSMAC
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CWeaponSMAC : public CHL2MPMachineGun
{
public:
	DECLARE_CLASS( CWeaponSMAC, CHL2MPMachineGun );

	CWeaponSMAC();

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	void	AddViewKick( void );

	Activity	GetPrimaryAttackActivity( void );

	virtual const Vector& GetBulletSpread( void )
	{
		static const Vector cone = VECTOR_CONE_5DEGREES;
		return cone;
	}

#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif
	
private:
	CWeaponSMAC( const CWeaponSMAC & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponSMAC, DT_WeaponSMAC )

BEGIN_NETWORK_TABLE( CWeaponSMAC, DT_WeaponSMAC )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponSMAC )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_smac, CWeaponSMAC );
PRECACHE_WEAPON_REGISTER(weapon_smac);

#ifndef CLIENT_DLL
acttable_t	CWeaponSMAC::m_acttable[] = 
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

IMPLEMENT_ACTTABLE(CWeaponSMAC);
#endif

//=========================================================
CWeaponSMAC::CWeaponSMAC( )
{

}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Activity
//-----------------------------------------------------------------------------
Activity CWeaponSMAC::GetPrimaryAttackActivity( void )
{
	if ( m_nShotsFired < 2 )
		return ACT_VM_PRIMARYATTACK;

	if ( m_nShotsFired < 3 )
		return ACT_VM_RECOIL1;
	
	if ( m_nShotsFired < 4 )
		return ACT_VM_RECOIL2;

	return ACT_VM_RECOIL3;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponSMAC::AddViewKick( void )
{
	#define	EASY_DAMPEN			0.5f
	#define	MAX_VERTICAL_KICK	1.0f	//Degrees
	#define	SLIDE_LIMIT			2.0f	//Seconds
	
	//Get the view kick
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if ( pPlayer == NULL )
		return;

	DoMachineGunKick( pPlayer, EASY_DAMPEN, MAX_VERTICAL_KICK, m_fFireDuration, SLIDE_LIMIT );
}