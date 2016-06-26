#include "cbase.h"
#include "neo_gamerules.h"

static CViewVectors g_NEOViewVectors(
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

BEGIN_NETWORK_TABLE_NOBASE( CNEOGameRules, DT_NEOGameRules )
	RecvPropInt( RECVINFO( m_iGameType ) ),
	RecvPropFloat( RECVINFO( m_fRoundTimeLeft ) ),
	RecvPropInt( RECVINFO( m_iRoundNumber ) ),
	RecvPropInt( RECVINFO( m_iGameState ) ),
	RecvPropInt( RECVINFO( m_iGameHud ) ),
	RecvPropInt( RECVINFO( m_iMVP ) ),
	RecvPropInt( RECVINFO( m_iAttackingTeam ) ),
	RecvPropBool( RECVINFO( m_bFreezePeriod ) )
END_NETWORK_TABLE()

bool CNEOGameRules::ShouldCollide( int collisionGroup0, int collisionGroup1 )
{
	if ( collisionGroup0 > collisionGroup1 )
	{
		// swap so that lowest is always first
		::V_swap( collisionGroup0, collisionGroup1 );
	}

	if ( collisionGroup1 == COLLISION_GROUP_PROJECTILE && (collisionGroup0 == COLLISION_GROUP_PLAYER || collisionGroup0 == COLLISION_GROUP_PLAYER_MOVEMENT) )
		return false;

	if ( collisionGroup0 == COLLISION_GROUP_PLAYER && collisionGroup1 == COLLISION_GROUP_PLAYER_MOVEMENT )
		return false;

	if ( collisionGroup1 == COLLISION_GROUP_WEAPON )
		return false;

	return BaseClass::ShouldCollide( collisionGroup0, collisionGroup1 );
}

const CViewVectors* CNEOGameRules::GetViewVectors() const
{
	return &g_NEOViewVectors;
}

// I'm not sure what this is all about since the values seem to be 0 all the time
void CNEOGameRules::LevelShutdown()
{
	DevMsg( "Client CNEOGameRules::LevelShutdown\n" );

	if ( m_iUnknown2 < 0 )
		m_iUnknown4 = m_iUnknown;

	else
	{
		delete m_iUnknown;

		m_iUnknown1 = 0;
		m_iUnknown4 = nullptr;
	}
}