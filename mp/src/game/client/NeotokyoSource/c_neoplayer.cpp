#include "cbase.h"
#include "c_neoplayer.h"
#include "beamdraw.h"
#include "dlight.h"
#include "iefx.h"
#include "prediction.h"
#include "iviewrender.h"
#include "ivieweffects.h"

#if defined( CNEOPlayer )
#undef CNEOPlayer
#endif

BEGIN_RECV_TABLE_NOBASE( C_NEOPlayer, DT_NEOLocalPlayerExclusive )
	RecvPropInt( RECVINFO( m_iShotsFired ) ),
	RecvPropFloat( RECVINFO( m_fSprintNRG ) ),
	RecvPropFloat( RECVINFO( m_fThermopticNRG ) ),
	RecvPropInt( RECVINFO( m_iReinforceTimer ) ),
	RecvPropInt( RECVINFO( m_iSprint ) ),
	RecvPropFloat( RECVINFO( m_fTurnSpeed ) ),
END_RECV_TABLE()

IMPLEMENT_CLIENTCLASS_DT( C_NEOPlayer, DT_NEOPlayer, CNEOPlayer )
	RecvPropDataTable( "neolocaldata", 0, 0, &REFERENCE_RECV_TABLE( DT_NEOLocalPlayerExclusive ) ),
	RecvPropFloat( RECVINFO( m_angEyeAngles[ 0 ] ) ),
	RecvPropFloat( RECVINFO( m_angEyeAngles[ 1 ] ) ),
	RecvPropInt( RECVINFO( m_iThrowGrenadeCounter ) ),
	RecvPropEHandle( RECVINFO( m_hRagdoll ) ),
	RecvPropInt( RECVINFO( m_iClassType ) ),
	RecvPropInt( RECVINFO( m_iLives ) ),
	RecvPropInt( RECVINFO( m_iRank ) ),
	RecvPropInt( RECVINFO( m_iStar ) ),
	RecvPropInt( RECVINFO( m_iLean ) ),
	RecvPropInt( RECVINFO( m_iThermoptic ) ),
	RecvPropInt( RECVINFO( m_iNMFlash ) ),
	RecvPropInt( RECVINFO( m_iVision ) ),
	RecvPropInt( RECVINFO( m_bIsVIP ) ),
END_RECV_TABLE()

C_NEOPlayer::C_NEOPlayer() : m_iv_angEyeAngles( "C_NEOPlayer::m_iv_angEyeAngles" )
{
	m_PlayerAnimState = CreatePlayerAnimState( this, this, LEGANIM_9WAY, true );

	m_angEyeAngles.Init();

	AddVar( &m_angEyeAngles, &m_iv_angEyeAngles, LATCH_SIMULATION_VAR );

	m_fLastDeathTime = 0.f;
	m_iLives = -1;
	m_fTurnSpeed = 1.f;
	m_flUnknown2 = 1.f;
	m_pBeam = nullptr;
	m_iReinforceTimer = 0;
	m_bAnyoneInCross = false;
	m_bIsOnDeathScreen = false;
	m_iOldThermoptic = 0;
	m_iThermoptic = 0;
	m_iOldVision = 0;
	m_iVision = 0;
	m_bIsVIP = false;
	m_MobileArmor = INVALID_CLIENTENTITY_HANDLE;
	m_iRank = m_iOldRank = 0;
}

C_NEOPlayer::~C_NEOPlayer()
{
	m_PlayerAnimState->Release();
}

C_NEOPlayer* C_NEOPlayer::GetLocalNEOPlayer()
{
	return (C_NEOPlayer*) C_BasePlayer::GetLocalPlayer();
}

bool C_NEOPlayer::IsLocalNEOPlayer( void ) const
{
	return (GetLocalNEOPlayer() == this);
}

CWeaponNEOBase* C_NEOPlayer::GetActiveNEOWeapon() const
{
	return dynamic_cast<CWeaponNEOBase*>(GetActiveWeapon());
}

void C_NEOPlayer::UpdateThermoptic()
{
	if ( m_iThermoptic != m_iOldThermoptic )
	{
		if ( m_iThermoptic == 1 )
		{
			EmitSound( "NeoPlayer.ThermOpticOn" );

			dlight_t* light = effects->CL_AllocDlight( LIGHT_INDEX_TE_DYNAMIC + index );

			light->origin = GetAbsOrigin();
			light->radius = 96.f;
			light->color.r = 64;
			light->color.b = 64;
			light->color.b = 255;
			light->color.exponent = 10;
			light->die = gpGlobals->curtime + 0.5f;
			light->decay = 192.f;
		}

		else
			EmitSound( "NeoPlayer.ThermOpticOff" );
	}

	Vector vel;
	EstimateAbsVelocity( vel );

	float speed = vel.Length2D();

	if ( speed <= 20.f )
		m_flUnknown -= (gpGlobals->curtime - m_fLastThinkTime) * 1.1f;
	else
		m_flUnknown = speed * 0.004 * (gpGlobals->curtime - m_fLastThinkTime) * m_flUnknown;

	if ( m_flUnknown < 0.f )
		m_flUnknown = 0.f;

	if ( m_flUnknown > 1.f )
		m_flUnknown = 1.f;

	if ( !m_iThermoptic || gpGlobals->curtime >= (m_flUnknown2 + 1.f) )
	{
		m_nRenderFX = kRenderFxNone;
		m_iOldThermoptic = m_iThermoptic;
	}

	else
	{
		m_nRenderFX = kRenderFxDistort;
		m_iOldThermoptic = 1;
		m_flUnknown = 1.f - m_flUnknown;
	}
}

void C_NEOPlayer::UpdateGeiger()
{
	if ( m_iRank <= m_iOldRank )
		EmitSound( "Geiger.BeepLow" );
	else
		EmitSound( "Geiger.BeepHigh" );

	m_iOldRank = m_iRank;
}

void C_NEOPlayer::UpdateVision()
{
	if ( !IsLocalNEOPlayer() )
		return;

	//dword_243ED4F8 = m_iVision;

	if ( m_iVision != m_iOldVision )
	{
		if ( m_iVision )
			EmitSound( "NeoPlayer.VisionOn" );
		else
			EmitSound( "NeoPlayer.VisionOff" );
	}

	m_iOldVision = m_iVision;
}

void C_NEOPlayer::UpdateInCross()
{
	if ( !IsLocalNEOPlayer() )
		return;

	m_iInCrossIndex = 0;
	m_bAnyoneInCross = false;

	Vector forward;
	GetEyeVectors( &forward, nullptr, nullptr );

	Vector eyes = EyePosition();

	Vector start = eyes + forward * 10.f;
	Vector end = eyes + forward * 1500.f;

	trace_t trace;
	UTIL_TraceLine( start, end, CONTENTS_LADDER | CONTENTS_MOVEABLE | CONTENTS_GRATE | CONTENTS_AUX | CONTENTS_WINDOW | CONTENTS_SOLID, this, 0, &trace );

	if ( !trace.startsolid && trace.DidHitNonWorldEntity() && trace.m_pEnt )
	{
		if ( trace.m_pEnt != this && trace.m_pEnt->IsPlayer() && trace.m_pEnt->GetTeamNumber() == this->GetTeamNumber() )
		{
			m_iInCrossIndex = trace.m_pEnt->entindex();
			m_bAnyoneInCross = true;
		}
	}
}

void C_NEOPlayer::UpdateSomething( float a1 )
{
	if ( m_fTurnSpeed == m_flUnknown4 )
		return;

	if ( m_fTurnSpeed > m_flUnknown4 )
	{
		m_flUnknown4 += a1;

		if ( m_fTurnSpeed > m_flUnknown4 )
			m_flUnknown4 = m_fTurnSpeed;
	}

	else
	{
		m_flUnknown4 -= a1;

		if ( m_fTurnSpeed < m_flUnknown4 )
			m_flUnknown4 = m_fTurnSpeed;
	}
}

void C_NEOPlayer::AddEntity()
{
	BaseClass::AddEntity();

	if ( !IsLocalNEOPlayer() )
		return;

	if ( IsEffectActive( EF_BRIGHTLIGHT ) )
	{
		matrix3x4_t matrix;
		QAngle eyeAngles = m_angEyeAngles;

		if ( !GetAttachment( LookupAttachment( "muzzle_flash" ), matrix ) )
			return;

		Vector start, end;

		AngleVectors( eyeAngles, &start );

		end = end * 200.f;

		trace_t trace;
		UTIL_TraceLine( start, end, CONTENTS_HITBOX | CONTENTS_DEBRIS | CONTENTS_MONSTER | CONTENTS_MOVEABLE | CONTENTS_WINDOW | CONTENTS_SOLID, this, 0, &trace );

		if ( m_pBeam )
		{
			beams->CreateBeamCirclePoints( TE_BEAMPOINTS, trace.startpos, trace.endpos, -1, -1, 3.f, 0.5f, 8.f, 35.f, 300.f, 0.f, 60.f, 0.f, 0, 0.f, 255.f, 255.f, 255.f );
			dlight_t* light = effects->CL_AllocDlight( 0 );

			light->origin = trace.startpos;
			light->radius = 50.f;
			light->color.r = 200;
			light->color.g = 200;
			light->color.b = 200;
			light->die = gpGlobals->curtime + 0.1f;
		}

		else
		{
			BeamInfo_t beamInfo;

			beamInfo.m_nType = TE_BEAMPOINTS;
			beamInfo.m_vecStart = trace.startpos;
			beamInfo.m_vecEnd = trace.endpos;
			beamInfo.m_nModelIndex = -1;
			beamInfo.m_pszModelName = "sprites/glow01.vmt";
			beamInfo.m_nHaloIndex = -1;
			beamInfo.m_pszHaloName = "sprites/glow01.vmt";
			beamInfo.m_flHaloScale = 3.f;
			beamInfo.m_flLife = 0.5f;
			beamInfo.m_flWidth = 8.f;
			beamInfo.m_flEndWidth = 35.f;
			beamInfo.m_flFadeLength = 300.f;
			beamInfo.m_flAmplitude = 0.f;
			beamInfo.m_flBrightness = 60.f;
			beamInfo.m_flSpeed = 0.f;
			beamInfo.m_nStartFrame = 0;
			beamInfo.m_flFrameRate = 0.f;
			beamInfo.m_flRed = 255.f;
			beamInfo.m_flGreen = 255.f;
			beamInfo.m_flBlue = 255.f;
			beamInfo.m_bRenderable = true;
			beamInfo.m_nSegments = 8;
			beamInfo.m_nFlags = FBEAM_HALOBEAM | FBEAM_FOREVER | FBEAM_NOTILE | FBEAM_ONLYNOISEONCE;

			m_pBeam = beams->CreateBeamRingPoint( beamInfo );
		}
	}

	else if ( m_pBeam )
	{
		m_pBeam->flags = 0;
		m_pBeam->die = gpGlobals->curtime - 1.f;
		m_pBeam = nullptr;
	}
}

Vector C_NEOPlayer::EyePosition()
{
	// This has mobile armor checks, and since I've never seen it ingame idgaf about it, or should I?
	return BaseClass::EyePosition();
}

C_BaseAnimating* C_NEOPlayer::BecomeRagdollOnClient( bool bCopyEntity /*= true*/ )
{
	return nullptr;
}

void C_NEOPlayer::UpdateClientSideAnimation()
{
	if ( this == GetLocalNEOPlayer() )
		m_PlayerAnimState->Update( EyeAngles()[ YAW ], m_angEyeAngles[ PITCH ] );
	else
		m_PlayerAnimState->Update( m_angEyeAngles[ YAW ], m_angEyeAngles[ PITCH ] );

	C_BaseAnimating::UpdateClientSideAnimation();
}

void C_NEOPlayer::DoMuzzleFlash()
{
	C_BaseAnimating::DoMuzzleFlash();
	//*(bool*) (this + 0x1044) = true;
}

void C_NEOPlayer::ProcessMuzzleFlashEvent()
{
	Vector vector;
	QAngle angles;

	bool bFoundAttachment = GetAttachment( 1, vector, angles );

	if ( bFoundAttachment )
	{
		dlight_t *el = effects->CL_AllocDlight( LIGHT_INDEX_MUZZLEFLASH + index );
		el->origin = vector;
		el->radius = RandomFloat( 32.f, 64.f );
		el->decay = el->radius / 0.05f;
		el->die = gpGlobals->curtime + 0.05f;
		el->color.r = 255;
		el->color.g = 192;
		el->color.b = 64;
		el->color.exponent = 5;
	}

	CWeaponNEOBase* activeWeapon = GetActiveNEOWeapon();

	if ( activeWeapon )
		FX_MuzzleEffect( activeWeapon->GetAbsOrigin(), activeWeapon->GetAbsAngles(), 1.f, 0 ); // It doesn't use GetAbsAngles, I'll change this later
}

void C_NEOPlayer::CalcView( Vector &eyeOrigin, QAngle &eyeAngles, float &zNear, float &zFar, float &fov )
{
	C_BaseEntity* ragdoll = m_hRagdoll.Get();

	if ( m_lifeState != LIFE_ALIVE && ragdoll && m_bIsOnDeathScreen )
	{
		int attachment = ragdoll->LookupAttachment( "eyes" );

		if ( !attachment )
			return;

		Vector origin;
		QAngle angles;

		if ( !ragdoll->GetAttachment( attachment, origin, angles ) )
			return;

		Vector forward;
		AngleVectors( angles, &forward );

		Vector start;

		start = forward;
		start.z += 64.f;

		trace_t trace;
		UTIL_TraceHull( start, forward, -Vector( 12, 12, 12 ), Vector( 12, 12, 12 ), CONTENTS_MOVEABLE | CONTENTS_GRATE | CONTENTS_AUX | CONTENTS_WINDOW | CONTENTS_SOLID, this, 0, &trace );

		if ( trace.fraction < 1.f )
		{
			// I'm not sure what they do in here yet 
		}

		if ( gpGlobals->curtime >= (m_fLastDeathTime + 10.f) )
			m_bIsOnDeathScreen = false;
	}

	// The original game has a check for mobile armor in here

	else
		BaseClass::CalcView( eyeOrigin, eyeAngles, zNear, zFar, fov );
}

bool C_NEOPlayer::CreateMove( float flInputSampleTime, CUserCmd *pCmd )
{
	// More mobile armor checks
	return BaseClass::CreateMove( flInputSampleTime, pCmd );
}

IRagdoll* C_NEOPlayer::GetRepresentativeRagdoll() const
{
	C_BaseEntity /*C_NEORagdoll*/ *pRagdoll = /*(C_NEORagdoll*)*/ m_hRagdoll.Get();

	if ( m_hRagdoll.Get() )
		return pRagdoll->GetIRagdoll();
	else
		return nullptr;
}

void C_NEOPlayer::PreThink()
{
	BaseClass::PreThink();

	UpdateThermoptic();
	UpdateGeiger();
	UpdateVision();
	UpdateInCross();
	UpdateSomething( gpGlobals->curtime - m_fLastThinkTime );

	m_fLastThinkTime = gpGlobals->curtime;
}

float C_NEOPlayer::GetFOV()
{
	CWeaponNEOBase* activeWeapon = GetActiveNEOWeapon();

	if ( activeWeapon )
		activeWeapon->m_fFov;
	else
		return 75.f;
}

float C_NEOPlayer::GetMinFOV() const
{
	return 1.f;
}

// This is mostly copy paste from C_BasePlayer::CalcPlayerView
void C_NEOPlayer::CalcPlayerView( Vector& eyeOrigin, QAngle& eyeAngles, float& fov )
{
	if ( !prediction->InPrediction() )
	{
		// FIXME: Move into prediction
		view->DriftPitch();
	}

	VectorCopy( EyePosition(), eyeOrigin );
	VectorCopy( EyeAngles(), eyeAngles );

	if ( !prediction->InPrediction() )
	{
		SmoothViewOnStairs( eyeOrigin );
	}

	// Snack off the origin before bob + water offset are applied
	Vector vecBaseEyePosition = eyeOrigin;

	CalcViewRoll( eyeAngles );

	// Apply punch angle
	VectorAdd( eyeAngles, m_Local.m_vecPunchAngle, eyeAngles );

	if ( !prediction->InPrediction() )
	{
		// Shake it up baby!
		vieweffects->CalcShake();
		vieweffects->ApplyShake( eyeOrigin, eyeAngles, 1.0 );
	}

	// Apply a smoothing offset to smooth out prediction errors.
	Vector vSmoothOffset;
	GetPredictionErrorSmoothingVector( vSmoothOffset );
	eyeOrigin += vSmoothOffset;
	m_flObserverChaseDistance = 0.0;

	// This is what they've added
	switch ( m_iUnknown )
	{
		case 0:
			eyeAngles.z = 0.f;
			break;

		case 1:
			eyeAngles.z = m_flUnknown3 * 20.f - 20.f;
			break;

		case 2:
			eyeAngles.z = -20.f;
			break;

		case 3:
			eyeAngles.z = -20.f * m_flUnknown3;
			break;

		case 4:
			eyeAngles.z = m_flUnknown3;
			break;

		case 5:
			eyeAngles.z = 20.f;
			break;

		case 6:
			eyeAngles.z = 20.f * m_flUnknown3;
			break;
	}

	// calc current FOV
	fov = GetFOV();
}

void C_NEOPlayer::NEO_MuzzleFlash()
{
	if ( m_iOldNMFlash != m_iNMFlash )
	{
		m_iOldNMFlash = m_iNMFlash;
		DoMuzzleFlash();
	}
}

void C_NEOPlayer::GetEyeVectors( Vector *forward, Vector *right, Vector *up )
{
	// This was supposed to have mobile armor checks in here
	AngleVectors( EyeAngles(), forward, right, up );
}