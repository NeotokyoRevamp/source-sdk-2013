#include "cbase.h"
#include "c_neoplayer.h"
#include "beamdraw.h"
#include "dlight.h"
#include "iefx.h"
#include "prediction.h"
#include "iviewrender.h"
#include "ivieweffects.h"
#include "materialsystem\imaterialvar.h"
#include "convar.h"
#include "view_scene.h"
#include "neo_gamerules.h"

ConVar cl_nt_ragdoll_lifetime( "cl_nt_ragdoll_lifetime", "60", FCVAR_REPLICATED );

void UpdateThermopticMaterial( IMaterial* material, float a2 );
IMaterial* GetThermopticMaterial();

#if defined( CNEOPlayer )
#undef CNEOPlayer
#endif

class C_NEORagdoll : public C_BaseAnimatingOverlay
{
public:
	DECLARE_CLASS( C_NEORagdoll, C_BaseAnimatingOverlay );
	DECLARE_CLIENTCLASS();

	C_NEORagdoll();
	~C_NEORagdoll();

	virtual void OnDataChanged( DataUpdateType_t type );

	IRagdoll* GetIRagdoll() const;

	virtual bool AddRagdollToFadeQueue() { return true; }

	virtual void ImpactTrace( trace_t *pTrace, int iDamageType, const char *pCustomImpactName );
	virtual int DrawModel( int flags );
	virtual void ClientThink();

private:
	C_NEORagdoll( const C_NEORagdoll & );

	void Interp_Copy( C_BaseAnimatingOverlay *pSourceEntity );
	void CreateNEORagdoll();

private:
	EHANDLE	m_hPlayer;
	CNetworkVector( m_vecRagdollVelocity );
	CNetworkVector( m_vecRagdollOrigin );
	float m_flRagdollSinkStart; // The time when it spawned, took the name from CSS source
};

IMPLEMENT_CLIENTCLASS_DT_NOBASE( C_NEORagdoll, DT_NEORagdoll, CNEORagdoll )
	RecvPropVector( RECVINFO( m_vecRagdollOrigin ) ),
	RecvPropEHandle( RECVINFO( m_hPlayer ) ),
	RecvPropInt( RECVINFO( m_nModelIndex ) ),
	RecvPropInt( RECVINFO( m_nForceBone ) ),
	RecvPropVector( RECVINFO( m_vecForce ) ),
	RecvPropVector( RECVINFO( m_vecRagdollVelocity ) )
END_RECV_TABLE()

C_NEORagdoll::C_NEORagdoll()
{
}

C_NEORagdoll::~C_NEORagdoll()
{
}

int C_NEORagdoll::DrawModel( int flags )
{
	C_NEOPlayer *pPlayer = dynamic_cast<C_NEOPlayer*>(m_hPlayer.Get());

	if ( pPlayer && (pPlayer->IsLocalNEOPlayer() || pPlayer->m_bIsOnDeathScreen) )
		return 0;

	if ( pPlayer->m_iVision == 3 ) // Thermal vision
	{
		IMaterial* matThermal = g_pMaterialSystem->FindMaterial( "dev/thermal", "Other textures" );

		if ( g_pMaterialSystemHardwareConfig->SupportsPixelShaders_2_0() )
		{
			bool found = false;

			IMaterialVar* matVar = matThermal->FindVar( "$eyevec", &found );

			if ( found )
			{
				Vector forward;
				pPlayer->GetVectors( &forward, nullptr, nullptr );

				matVar->SetVecValue( forward.x, forward.y, forward.z );
			}
		}

		modelrender->ForcedMaterialOverride( matThermal );

		int result = InternalDrawModel( flags );

		modelrender->ForcedMaterialOverride( nullptr );

		return result;
	}

	return BaseClass::DrawModel( flags );
}

void C_NEORagdoll::ClientThink()
{
	if ( (m_flRagdollSinkStart + cl_nt_ragdoll_lifetime.GetFloat()) < gpGlobals->curtime )
	{
		C_NEOPlayer* localPlayer = C_NEOPlayer::GetLocalNEOPlayer();

		if ( !localPlayer )
			return;

		if ( (GetAbsOrigin() - localPlayer->GetAbsOrigin()).Length() > 600.f )
		{
			Vector origin, min, max;

			origin = m_pRagdoll->GetRagdollOrigin();
			m_pRagdoll->GetRagdollBounds( min, max );

			if ( engine->IsBoxInViewCluster( origin + min, origin + max ) )
				engine->CullBox( origin + min, origin + max );
		}
	}
}

// Turns out these are the same as in C_HL2MPRagdoll except CreateNEORagdoll, so let's paste them
void C_NEORagdoll::Interp_Copy( C_BaseAnimatingOverlay *pSourceEntity )
{
	if ( !pSourceEntity )
		return;

	VarMapping_t *pSrc = pSourceEntity->GetVarMapping();
	VarMapping_t *pDest = GetVarMapping();

	// Find all the VarMapEntry_t's that represent the same variable.
	for ( int i = 0; i < pDest->m_Entries.Count(); i++ )
	{
		VarMapEntry_t *pDestEntry = &pDest->m_Entries[ i ];
		const char *pszName = pDestEntry->watcher->GetDebugName();
		for ( int j = 0; j < pSrc->m_Entries.Count(); j++ )
		{
			VarMapEntry_t *pSrcEntry = &pSrc->m_Entries[ j ];
			if ( !Q_strcmp( pSrcEntry->watcher->GetDebugName(), pszName ) )
			{
				pDestEntry->watcher->Copy( pSrcEntry->watcher );
				break;
			}
		}
	}
}

void C_NEORagdoll::ImpactTrace( trace_t *pTrace, int iDamageType, const char *pCustomImpactName )
{
	IPhysicsObject *pPhysicsObject = VPhysicsGetObject();

	if ( !pPhysicsObject )
		return;

	Vector dir = pTrace->endpos - pTrace->startpos;

	if ( iDamageType == DMG_BLAST )
	{
		dir *= 4000;  // adjust impact strenght

		// apply force at object mass center
		pPhysicsObject->ApplyForceCenter( dir );
	}
	else
	{
		Vector hitpos;

		VectorMA( pTrace->startpos, pTrace->fraction, dir, hitpos );
		VectorNormalize( dir );

		dir *= 4000;  // adjust impact strenght

		// apply force where we hit it
		pPhysicsObject->ApplyForceOffset( dir, hitpos );

		// Blood spray!
		//		FX_CS_BloodSpray( hitpos, dir, 10 );
	}

	m_pRagdoll->ResetRagdollSleepAfterTime();
}

void C_NEORagdoll::CreateNEORagdoll()
{
	// First, initialize all our data. If we have the player's entity on our client,
	// then we can make ourselves start out exactly where the player is.
	C_NEOPlayer *pPlayer = dynamic_cast<C_NEOPlayer*>(m_hPlayer.Get());

	if ( pPlayer && !pPlayer->IsDormant() )
	{
		pPlayer->SnatchModelInstance( this );

		VarMapping_t *varMap = GetVarMapping();

		// Copy all the interpolated vars from the player entity.
		// The entity uses the interpolated history to get bone velocity.
		bool bRemotePlayer = (pPlayer != C_BasePlayer::GetLocalPlayer());
		if ( bRemotePlayer )
		{
			Interp_Copy( pPlayer );

			SetAbsAngles( pPlayer->GetRenderAngles() );
			GetRotationInterpolator().Reset();

			m_flAnimTime = pPlayer->m_flAnimTime;
			SetSequence( pPlayer->GetSequence() );
			m_flPlaybackRate = pPlayer->GetPlaybackRate();
		}
		else
		{
			// This is the local player, so set them in a default
			// pose and slam their velocity, angles and origin
			SetAbsOrigin( m_vecRagdollOrigin );

			SetAbsAngles( pPlayer->GetRenderAngles() );

			SetAbsVelocity( m_vecRagdollVelocity );

			int iSeq = LookupSequence( "walk_lower" );
			if ( iSeq == -1 )
			{
				Assert( false );	// missing walk_lower?
				iSeq = 0;
			}

			SetSequence( iSeq );	// walk_lower, basic pose
			SetCycle( 0.0 );

			Interp_Reset( varMap );

			pPlayer->m_bIsOnDeathScreen = true; // Only these were added
			pPlayer->m_fRagdollCreationTime = gpGlobals->curtime;
		}
	}
	else
	{
		// overwrite network origin so later interpolation will
		// use this position
		SetNetworkOrigin( m_vecRagdollOrigin );

		SetAbsOrigin( m_vecRagdollOrigin );
		SetAbsVelocity( m_vecRagdollVelocity );

		Interp_Reset( GetVarMapping() );
	}

	SetModelByIndex( GetModelIndex() );

	// Make us a ragdoll..
	m_nRenderFX = kRenderFxRagdoll;

	matrix3x4_t boneDelta0[ MAXSTUDIOBONES ];
	matrix3x4_t boneDelta1[ MAXSTUDIOBONES ];
	matrix3x4_t currentBones[ MAXSTUDIOBONES ];
	const float boneDt = 0.05f;

	if ( pPlayer && !pPlayer->IsDormant() )
	{
		pPlayer->GetRagdollInitBoneArrays( boneDelta0, boneDelta1, currentBones, boneDt );
	}
	else
	{
		GetRagdollInitBoneArrays( boneDelta0, boneDelta1, currentBones, boneDt );
	}

	InitAsClientRagdoll( boneDelta0, boneDelta1, currentBones, boneDt );
}

void C_NEORagdoll::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( type == DATA_UPDATE_CREATED )
	{
		CreateNEORagdoll();

		AngularImpulse angularVel( 0, 0, 0 );

		m_pPhysicsObject->AddVelocity( &m_vecRagdollVelocity.Get(), &angularVel );
	}
}

IRagdoll* C_NEORagdoll::GetIRagdoll() const
{
	return m_pRagdoll;
}

BEGIN_RECV_TABLE_NOBASE( C_NEOPlayer, DT_NEOLocalPlayerExclusive )
	RecvPropInt( RECVINFO( m_iShotsFired ) ),
	RecvPropFloat( RECVINFO( m_fSprintNRG ) ),
	RecvPropFloat( RECVINFO( m_fThermopticNRG ) ),
	RecvPropInt( RECVINFO( m_iReinforceTimer ) ),
	RecvPropInt( RECVINFO( m_iSprint ) ),
	RecvPropFloat( RECVINFO( m_fTurnSpeed ) )
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
	RecvPropInt( RECVINFO( m_bIsVIP ) )
END_RECV_TABLE()

C_NEOPlayer::C_NEOPlayer() : m_iv_angEyeAngles( "C_NEOPlayer::m_iv_angEyeAngles" )
{
	m_PlayerAnimState = CreatePlayerAnimState( this, this, LEGANIM_9WAY, true );

	m_angEyeAngles.Init();

	AddVar( &m_angEyeAngles, &m_iv_angEyeAngles, LATCH_SIMULATION_VAR );

	m_fRagdollCreationTime = 0.f;
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

bool C_NEOPlayer::CanMove()
{
	return (GetObserverMode() == OBS_MODE_NONE && !NEOGameRules()->IsInFreezePeriod());
}

bool C_NEOPlayer::CanSpeedBoost()
{
	return m_fSprintNRG > 33.f;
}

CWeaponNEOBase* C_NEOPlayer::GetActiveNEOWeapon() const
{
	return dynamic_cast< CWeaponNEOBase* >( GetActiveWeapon() );
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

	//dword_243ED4F8 = m_iVision; // This is used somewhere in CViewRender to draw the nightvision effect

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

void C_NEOPlayer::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( updateType == DATA_UPDATE_CREATED )
		SetNextClientThink( -1293.f );

	UpdateVisibility();
}

void C_NEOPlayer::PostDataUpdate( DataUpdateType_t updateType )
{
	SetNetworkAngles( GetLocalAngles() );

	BaseClass::PostDataUpdate( updateType );
}

const QAngle& C_NEOPlayer::GetRenderAngles()
{
	if ( IsRagdoll() )
		return vec3_angle;

	else
		return m_PlayerAnimState->GetRenderAngles();
}

int C_NEOPlayer::DrawModel( int flags )
{
	C_NEOPlayer* localPlayer = C_NEOPlayer::GetLocalNEOPlayer();

	int result = -1;

	if ( localPlayer )
	{
		if ( GetTeamNumber() == localPlayer->GetTeamNumber() )
			m_bUnknown = true;

		if ( m_iOldVision != 3 )
		{
			if ( m_bUnknown2 )
			{
				m_bUnknown2 = false;

				dlight_t* light = effects->CL_AllocDlight( LIGHT_INDEX_TE_DYNAMIC + index );

				light->origin = GetAbsOrigin();
				light->radius = 96.f;
				light->decay = 192.f;
				light->color.r = 64;
				light->color.g = 64;
				light->color.b = 255;
				light->color.exponent = 10;
				light->die = gpGlobals->curtime + 0.1f;

				return 0;
			}

			if ( !g_pMaterialSystemHardwareConfig->SupportsPixelShaders_2_0() )
				return 0;

			UpdateRefractTexture();

			IMaterial* thermopticMaterial = GetThermopticMaterial();

			UpdateThermopticMaterial( thermopticMaterial, m_flUnknown );

			modelrender->ForcedMaterialOverride( thermopticMaterial );
			result = BaseClass::InternalDrawModel( flags );
			modelrender->ForcedMaterialOverride( nullptr );
		}		

		else
		{
			if ( !g_pMaterialSystemHardwareConfig->SupportsPixelShaders_2_0() )
				return BaseClass::DrawModel( flags );

			IMaterial* matThermal = g_pMaterialSystem->FindMaterial( "dev/thermal", TEXTURE_GROUP_OTHER );

			if ( IsErrorMaterial( matThermal ) )
			{
				DevMsg( SPEW_MESSAGE, "Fuck me...\n" ); // Their message, not mine kek
				BaseClass::DrawModel( flags );
			}

			bool found = false;
			IMaterialVar* matVar = matThermal->FindVar( "$eyevec", &found );

			if ( found )
			{
				Vector forward;
				GetVectors( &forward, nullptr, nullptr );
				matVar->SetVecValue( forward.x, forward.y, forward.z, 3.f );
			}

			modelrender->ForcedMaterialOverride( matThermal );
			result = BaseClass::InternalDrawModel( flags );
			modelrender->ForcedMaterialOverride( nullptr );
		}

		if ( m_iThermoptic == 1 )
		{
			if ( !g_pMaterialSystemHardwareConfig->SupportsPixelShaders_2_0() )
				return BaseClass::DrawModel( flags );

			IMaterial* matMotion = g_pMaterialSystem->FindMaterial( "dev/motion", TEXTURE_GROUP_OTHER );

			if ( IsErrorMaterial( matMotion ) )
			{
				DevMsg( SPEW_MESSAGE, "Fuck me...\n" );
				BaseClass::DrawModel( flags );
			}

			float velocity = localPlayer->GetLocalVelocity().Length() / 75.f;

			if ( velocity > 4.f )
				velocity = 4.f;

			bool found = false;
			IMaterialVar* matVar = matMotion->FindVar( "$eyevec", &found );

			if ( found )
			{
				Vector forward;
				GetVectors( &forward, nullptr, nullptr );
				matVar->SetVecValue( forward.x, forward.y, forward.z, velocity );
			}

			modelrender->ForcedMaterialOverride( matMotion );
			result = BaseClass::InternalDrawModel( flags );
			modelrender->ForcedMaterialOverride( nullptr );
		}

		if ( m_iVision == 3 ) // Thermal vision
		{
			if ( !g_pMaterialSystemHardwareConfig->SupportsPixelShaders_2_0() )
				return BaseClass::DrawModel( flags );

			IMaterial* matThermal = g_pMaterialSystem->FindMaterial( "dev/vm_thermal", "Other textures" );

			if ( IsErrorMaterial( matThermal ) )
			{
				DevMsg( SPEW_MESSAGE, "Fuck me...\n" );
				BaseClass::DrawModel( flags );
			}

			bool found = false;
			IMaterialVar* matVar = matThermal->FindVar( "$eyevec", &found );

			if ( found )
			{
				Vector forward;
				GetVectors( &forward, nullptr, nullptr );
				matVar->SetVecValue( forward.x, forward.y, forward.z, 3.f );
			}

			modelrender->ForcedMaterialOverride( matThermal );
			result = BaseClass::InternalDrawModel( flags );
			modelrender->ForcedMaterialOverride( nullptr );
		}

		if ( m_bUnknown2 )
			m_bUnknown2 = false;

		if ( result >= 0 )
			return result;
	}

	return BaseClass::DrawModel( flags );
}

ShadowType_t C_NEOPlayer::ShadowCastType()
{
	if ( !IsVisible() )
		return SHADOWS_NONE;

	return m_iThermoptic != 1 ? SHADOWS_RENDER_TO_TEXTURE_DYNAMIC : SHADOWS_NONE;
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
	m_bUnknown2 = true;
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
			forward = trace.endpos;

		if ( gpGlobals->curtime >= (m_fRagdollCreationTime + 10.f) )
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
	C_NEORagdoll *pRagdoll = (C_NEORagdoll*) m_hRagdoll.Get();

	if ( pRagdoll )
		return pRagdoll->GetIRagdoll();

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
		return activeWeapon->m_fFov;
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

CWeaponNEOBase* C_NEOPlayer::NEOAnim_GetActiveWeapon()
{
	return GetActiveNEOWeapon();
}

bool C_NEOPlayer::NEOAnim_CanMove()
{
	return true;
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