//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: MXs assault rifle
//
//=============================================================================//

#include "cbase.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
#else
	#include "hl2mp_player.h"
#endif

#include "weapon_hl2mpbase.h"

#ifdef CLIENT_DLL
#define CWeaponMXs C_WeaponMXs
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CWeaponMXs : public CWeaponHL2MPBase
{
public:
	DECLARE_CLASS( CWeaponMXs, CWeaponHL2MPBase );

	CWeaponMXs();

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	void	AddViewKick( void );

#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif
	
private:
	CWeaponMXs( const CWeaponMXs & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponMXs, DT_WeaponMXs )

BEGIN_NETWORK_TABLE( CWeaponMXs, DT_WeaponMXs )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponMXs )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_mxs, CWeaponMXs );
PRECACHE_WEAPON_REGISTER(weapon_mxs);

#ifndef CLIENT_DLL
acttable_t	CWeaponMXs::m_acttable[] = 
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

IMPLEMENT_ACTTABLE(CWeaponMXs);
#endif

//=========================================================
CWeaponMXs::CWeaponMXs( )
{

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponMXs::AddViewKick( void )
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