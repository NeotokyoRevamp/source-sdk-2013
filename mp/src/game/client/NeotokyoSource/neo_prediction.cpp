#include "cbase.h"
#include "prediction.h"
#include "c_neoplayer.h"
#include "igamemovement.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static CMoveData g_MoveData;
CMoveData *g_pMoveData = &g_MoveData;

class CNEOPrediction : public CPrediction
{
	DECLARE_CLASS( CNEOPrediction, CPrediction );

public:
	virtual void	SetupMove( C_BasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move );
	virtual void	FinishMove( C_BasePlayer *player, CUserCmd *ucmd, CMoveData *move );
};

void CNEOPrediction::SetupMove( C_BasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move )
{
	BaseClass::SetupMove( player, ucmd, pHelper, move );
}

void CNEOPrediction::FinishMove( C_BasePlayer *player, CUserCmd *ucmd, CMoveData *move )
{
	BaseClass::FinishMove( player, ucmd, move );
}

// Expose interface to engine
static CPrediction g_Prediction;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CPrediction, IPrediction, VCLIENT_PREDICTION_INTERFACE_VERSION, g_Prediction );

CPrediction *prediction = &g_Prediction;

