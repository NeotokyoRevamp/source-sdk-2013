//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Knife stuff. Base copied from weapon_crowbar.h
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#ifndef HL2MP_WEAPON_KNIFE_H
#define HL2MP_WEAPON_KNIFE_H
#pragma once


#include "weapon_hl2mpbasehlmpcombatweapon.h"
#include "weapon_hl2mpbasebasebludgeon.h"


#ifdef CLIENT_DLL
#define CWeaponKnife C_WeaponKnife
#endif

//-----------------------------------------------------------------------------
// CWeaponKnife
//-----------------------------------------------------------------------------

class CWeaponKnife : public CBaseHL2MPBludgeonWeapon
{
public:
	DECLARE_CLASS( CWeaponKnife, CBaseHL2MPBludgeonWeapon );

	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif

	CWeaponKnife();

	float		GetRange( void );
	float		GetFireRate( void );
	float		GetDamageForActivity( Activity hitActivity );
	void		SecondaryAttack( void )	{	return;	}

	void		Drop( const Vector &vecVelocity );


	// Animation event
#ifndef CLIENT_DLL
	virtual void Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );
	void HandleAnimEventMeleeHit( animevent_t *pEvent, CBaseCombatCharacter *pOperator );
#endif

	CWeaponKnife( const CWeaponKnife & );

private:
		
};


#endif // HL2MP_WEAPON_KNIFE_H

