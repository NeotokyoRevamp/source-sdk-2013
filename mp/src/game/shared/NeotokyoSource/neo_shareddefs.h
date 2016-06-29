#ifndef NEO_SHAREDDEFS_H
#define NEO_SHAREDDEFS_H

#ifdef _WIN32
#pragma once
#endif

#define PANEL_LOADOUT			"loadout"
#define PANEL_LOADOUT_DEV		"loadout_dev"


#define NEO_VEC_VIEW_SCALED( classType, player )				( m_ViewVectors[ classType ].m_vView * player->GetModelScale() )
#define NEO_VEC_HULL_MIN_SCALED( classType, player )			( m_ViewVectors[ classType ].m_vHullMin * player->GetModelScale() )
#define NEO_VEC_HULL_MAX_SCALED( classType, player )			( m_ViewVectors[ classType ].m_vHullMax * player->GetModelScale() )

#define NEO_VEC_DUCK_HULL_MIN_SCALED( classType, player )		( m_ViewVectors[ classType ].m_vDuckHullMin * player->GetModelScale() )
#define NEO_VEC_DUCK_HULL_MAX_SCALED( classType, player )		( m_ViewVectors[ classType ].m_vDuckHullMax * player->GetModelScale() )
#define NEO_VEC_DUCK_VIEW_SCALED( classType, player )			( m_ViewVectors[ classType ].m_vDuckView * player->GetModelScale() )

#define NEO_VEC_OBS_HULL_MIN_SCALED( classType, player )		( m_ViewVectors[ classType ].m_vObsHullMin * player->GetModelScale() )
#define NEO_VEC_OBS_HULL_MAX_SCALED( classType, player )		( m_ViewVectors[ classType ].m_vObsHullMax * player->GetModelScale() )

#define NEO_VEC_DEAD_VIEWHEIGHT_SCALED( classType, player )	( m_ViewVectors[ classType ].m_vDeadViewHeight * player->GetModelScale() )


// From https://github.com/softashell/sourcemod-nt-include

#define TEAM_JINRAI		2
#define	TEAM_NSF		3
#define TEAM_MAXCOUNT	4

#define VISION_NONE		0
#define VISION_NIGHT	2
#define VISION_THERMAL 	3
#define VISION_MOTION	4

#define CLASS_NONE		0
#define CLASS_RECON		1
#define CLASS_ASSAULT	2
#define CLASS_SUPPORT	3
#define CLASS_MAX		6 // Maybe the max, because of CNEOGameMovement::GetPlayerMins

#define IN_AIM			(1 << 23)
#define IN_LEANL		(1 << 24)
#define IN_LEANR		(1 << 25)
#define IN_SPRINT		(1 << 26)
#define IN_THERMOPTIC	(1 << 27)
#define IN_VISION		(1 << 28)
#define IN_TOSS			(1 << 29)

#endif // NEO_SHAREDDEFS_H
