#include "cbase.h"
#include "gamemovement.h"
#include "neo_shareddefs.h"
#include "in_buttons.h"
#include "movevars_shared.h"

#ifdef CLIENT_DLL
#include "c_neoplayer.h"
#endif

#define NEOGAMEMOVEMENT_JUMP_HEIGHT			36.0f


class CNEOGameMovement : public CGameMovement
{
public:
	DECLARE_CLASS( CNEOGameMovement, CGameMovement );

	CNEOGameMovement();

	virtual void	ProcessMovement( CBasePlayer *pPlayer, CMoveData *pMove );

	virtual Vector	GetPlayerMins( bool ducked ) const;
	virtual Vector	GetPlayerMaxs( bool ducked ) const;
	virtual Vector	GetPlayerViewOffset( bool ducked ) const;

	virtual Vector	GetPlayerMins( void ) const; // uses local player
	virtual Vector	GetPlayerMaxs( void ) const; // uses local player

	virtual bool	CanAccelerate();

	virtual void	WalkMove();

	virtual bool	CheckJumpButton();

	virtual void	CategorizePosition();

	virtual void	HandleDuckingSpeedCrop();

private:
	CViewVectors m_ViewVectors[ CLASS_MAX - 1 ]; // This may or may not have to do with classes, I'm not sure anymore. WHAT THE FUCK RADI8
};

// Expose our interface.
static CNEOGameMovement g_GameMovement;
IGameMovement *g_pGameMovement = (IGameMovement *) &g_GameMovement;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CGameMovement, IGameMovement, INTERFACENAME_GAMEMOVEMENT, g_GameMovement );


CNEOGameMovement::CNEOGameMovement()
{
	m_ViewVectors[ CLASS_NONE ] = CViewVectors(
		Vector( 0, 0, 64 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 72 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 36 ),
		Vector( 0, 0, 28 ),

		Vector( -10, -10, -10 ),
		Vector( 10, 10, 10 ),

		Vector( 0, 0, 14 )
		);

	m_ViewVectors[ CLASS_RECON ] = CViewVectors(
		Vector( 0, 0, 57 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 64 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 46 ),
		Vector( 0, 0, 41 ),

		Vector( -10, -10, -10 ),
		Vector( 10, 10, 10 ),

		Vector( 0, 0, 14 )
		);

	m_ViewVectors[ CLASS_ASSAULT ] = CViewVectors(
		Vector( 0, 0, 57.5 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 65 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 48 ),
		Vector( 0, 0, 41 ),

		Vector( -10, -10, -10 ),
		Vector( 10, 10, 10 ),

		Vector( 0, 0, 14 )
		);

	m_ViewVectors[ CLASS_SUPPORT ] = CViewVectors(
		Vector( 0, 0, 60 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 70 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 59 ),
		Vector( 0, 0, 50 ),

		Vector( -10, -10, -10 ),
		Vector( 10, 10, 10 ),

		Vector( 0, 0, 14 )
		);

	m_ViewVectors[ 4 ] = CViewVectors(
		Vector( 0, 0, 57.5 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 65 ),

		Vector( -16, -16, 0 ),
		Vector( 16, 16, 48 ),
		Vector( 0, 0, 41 ),

		Vector( -10, -10, -10 ),
		Vector( 10, 10, 10 ),

		Vector( 0, 0, 14 )
		);
}

void CNEOGameMovement::ProcessMovement( CBasePlayer *pPlayer, CMoveData *pMove )
{
	BaseClass::ProcessMovement( dynamic_cast<C_NEOPlayer*>(pPlayer), pMove );
}

Vector CNEOGameMovement::GetPlayerMins( bool ducked ) const
{
	C_NEOPlayer* neoPlayer = dynamic_cast<C_NEOPlayer*>(player);

	if ( neoPlayer && neoPlayer->m_iClassType >= CLASS_NONE && neoPlayer->m_iClassType < CLASS_MAX )
		return ducked ? NEO_VEC_DUCK_HULL_MIN_SCALED( neoPlayer->m_iClassType, player ) : NEO_VEC_HULL_MIN_SCALED( neoPlayer->m_iClassType, player ); // source sdk 2006 doesn't scales, watch out
	else
		return ducked ? VEC_DUCK_HULL_MIN_SCALED( player ) : VEC_HULL_MIN_SCALED( player ); // same about scaling in here
}

Vector CNEOGameMovement::GetPlayerMaxs( bool ducked ) const
{
	C_NEOPlayer* neoPlayer = dynamic_cast<C_NEOPlayer*>(player);

	if ( neoPlayer && neoPlayer->m_iClassType >= CLASS_NONE && neoPlayer->m_iClassType < CLASS_MAX )
		return ducked ? NEO_VEC_DUCK_HULL_MAX_SCALED( neoPlayer->m_iClassType, player ) : NEO_VEC_HULL_MAX_SCALED( neoPlayer->m_iClassType, player );
	else
		return ducked ? VEC_DUCK_HULL_MAX_SCALED( player ) : VEC_HULL_MAX_SCALED( player );
}

Vector CNEOGameMovement::GetPlayerViewOffset( bool ducked ) const
{
	C_NEOPlayer* neoPlayer = dynamic_cast<C_NEOPlayer*>(player);

	if ( neoPlayer && neoPlayer->m_iClassType >= CLASS_NONE && neoPlayer->m_iClassType < CLASS_MAX )
		return ducked ? NEO_VEC_DUCK_VIEW_SCALED( neoPlayer->m_iClassType, player ) : NEO_VEC_VIEW_SCALED( neoPlayer->m_iClassType, player );
	else
		return ducked ? VEC_DUCK_VIEW_SCALED( player ) : VEC_VIEW_SCALED( player );
}

Vector CNEOGameMovement::GetPlayerMins( void ) const
{
	if ( player->IsObserver() )
	{
		return VEC_OBS_HULL_MIN_SCALED( player );
	}
	else
	{
		C_NEOPlayer* neoPlayer = dynamic_cast<C_NEOPlayer*>(player);

		if ( neoPlayer && neoPlayer->m_iClassType >= CLASS_NONE && neoPlayer->m_iClassType < CLASS_MAX )
			return player->m_Local.m_bDucked ? NEO_VEC_DUCK_HULL_MIN_SCALED( neoPlayer->m_iClassType, player ) : NEO_VEC_HULL_MIN_SCALED( neoPlayer->m_iClassType, player );
		else
			return player->m_Local.m_bDucked ? VEC_DUCK_HULL_MIN_SCALED( player ) : VEC_HULL_MIN_SCALED( player );
	}
}

Vector CNEOGameMovement::GetPlayerMaxs( void ) const
{
	if ( player->IsObserver() )
	{
		return VEC_OBS_HULL_MAX_SCALED( player );
	}
	else
	{
		C_NEOPlayer* neoPlayer = dynamic_cast<C_NEOPlayer*>(player);

		if ( neoPlayer && neoPlayer->m_iClassType >= CLASS_NONE && neoPlayer->m_iClassType < CLASS_MAX )
			return player->m_Local.m_bDucked ? NEO_VEC_DUCK_HULL_MAX_SCALED( neoPlayer->m_iClassType, player ) : NEO_VEC_HULL_MAX_SCALED( neoPlayer->m_iClassType, player );
		else
			return player->m_Local.m_bDucked ? VEC_DUCK_HULL_MAX_SCALED( player ) : VEC_HULL_MAX_SCALED( player );
	}
}

bool CNEOGameMovement::CanAccelerate()
{
	if ( player->GetObserverMode() == OBS_MODE_CHASE )
		return true;

	C_NEOPlayer* neoPlayer = dynamic_cast<C_NEOPlayer*>(player);

	if ( neoPlayer->CanMove() )
		return false;
	
	return BaseClass::CanAccelerate();
}

void CNEOGameMovement::WalkMove()
{
	if ( mv->m_flForwardMove < 0.f )
		mv->m_flForwardMove *= 0.75f;

	BaseClass::WalkMove();
}

bool CNEOGameMovement::CheckJumpButton()
{
	C_NEOPlayer* neoPlayer = dynamic_cast<C_NEOPlayer*>(player);

	if ( !neoPlayer->CanMove() )
		return false;

	bool bIsRecon = false, bIsSprintingAsRecon = false;
	float fClassVelModifier = 0.f;

	if ( neoPlayer )
	{
		if ( neoPlayer->m_iClassType == CLASS_RECON )
		{
			bIsRecon = true;
			fClassVelModifier = 1.5f;

			if ( neoPlayer->m_iSprint == 1 )
				bIsSprintingAsRecon = true;
		}

		else if ( neoPlayer->m_iClassType == 5 ) // LEAKED CODE ABOUT A NEW CLASS!!!!!!1111 #dramaalert
			fClassVelModifier = 1.5f;
	}

	if ( player->pl.deadflag )
	{
		mv->m_nOldButtons |= IN_JUMP;	// don't jump again until released
		return false;
	}

	// See if we are waterjumping.  If so, decrement count and return.
	if ( player->m_flWaterJumpTime )
	{
		player->m_flWaterJumpTime -= gpGlobals->frametime;
		if ( player->m_flWaterJumpTime < 0 )
			player->m_flWaterJumpTime = 0;

		return false;
	}

	// If we are in the water most of the way...
	if ( player->GetWaterLevel() >= 2 )
	{
		// swimming, not jumping
		SetGroundEntity( NULL );

		if ( player->GetWaterType() == CONTENTS_WATER )    // We move up a certain amount
			mv->m_vecVelocity[ 2 ] = 100;
		else if ( player->GetWaterType() == CONTENTS_SLIME )
			mv->m_vecVelocity[ 2 ] = 80;

		// play swiming sound
		if ( player->m_flSwimSoundTime <= 0 )
		{
			// Don't play sound again for 1 second
			player->m_flSwimSoundTime = 1000;
			PlaySwimSound();
		}

		return false;
	}

	// No more effect
	if ( player->GetGroundEntity() == NULL )
	{
		mv->m_nOldButtons |= IN_JUMP;
		return false;		// in air, so no effect
	}

	// Don't allow jumping when the player is in a stasis field.
	if ( player->m_Local.m_bSlowMovement )
		return false;

	if ( mv->m_nOldButtons & IN_JUMP )
		return false;		// don't pogo stick

	// Cannot jump will in the unduck transition.
	if ( player->m_Local.m_bDucking && (player->GetFlags() & FL_DUCKING) )
		return false;

	// Still updating the eye position.
	if ( player->m_Local.m_flDuckJumpTime > 0.0f )
		return false;


	// In the air now.
	SetGroundEntity( NULL );

	player->PlayStepSound( (Vector &) mv->GetAbsOrigin(), player->m_pSurfaceData, 1.0, true );

	MoveHelper()->PlayerSetAnimation( PLAYER_JUMP );

	float flGroundFactor = 1.0f;
	if ( player->m_pSurfaceData )
	{
		flGroundFactor = player->m_pSurfaceData->game.jumpFactor;
	}

	float flMul = sqrt( 2 * GetCurrentGravity() * NEOGAMEMOVEMENT_JUMP_HEIGHT * fClassVelModifier );

	// Acclerate upward
	// If we are ducking...
	float startz = mv->m_vecVelocity[ 2 ];
	if ( (player->m_Local.m_bDucking) || (player->GetFlags() & FL_DUCKING) )
	{
		// d = 0.5 * g * t^2		- distance traveled with linear accel
		// t = sqrt(2.0 * 45 / g)	- how long to fall 45 units
		// v = g * t				- velocity at the end (just invert it to jump up that high)
		// v = g * sqrt(2.0 * 45 / g )
		// v^2 = g * g * 2.0 * 45 / g
		// v = sqrt( g * 2.0 * 45 )
		mv->m_vecVelocity[ 2 ] = flGroundFactor * flMul;  // 2 * gravity * height
	}
	else
	{
		mv->m_vecVelocity[ 2 ] += flGroundFactor * flMul;  // 2 * gravity * height
	}

	if ( neoPlayer && bIsRecon && bIsSprintingAsRecon && neoPlayer->CanSpeedBoost() )
	{
		Vector vecForward;

		AngleVectors( mv->m_vecViewAngles, &vecForward );
		vecForward.z = 0;
		VectorNormalize( vecForward );

		vecForward *= mv->m_flForwardMove;

		VectorAdd( vecForward, mv->m_vecVelocity, mv->m_vecVelocity );
	}

	FinishGravity();

	mv->m_outJumpVel.z += mv->m_vecVelocity[ 2 ] - startz;
	mv->m_outStepHeight += 0.15f;

	OnJump( mv->m_outJumpVel.z );

	// Set jump time.
	if ( gpGlobals->maxClients == 1 )
	{
		player->m_Local.m_flJumpTime = GAMEMOVEMENT_JUMP_TIME;
		player->m_Local.m_bInDuckJump = true;
	}

	// Flag that we jumped.
	mv->m_nOldButtons |= IN_JUMP;	// don't jump again until released
	return true;
}

void CNEOGameMovement::CategorizePosition()
{
	// There should be a mobile armor check in here
	BaseClass::CategorizePosition();
}

void CNEOGameMovement::HandleDuckingSpeedCrop( void )
{
}