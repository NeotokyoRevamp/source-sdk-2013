#ifndef BASEHL2MPVIEWMODEL_SHARED_H
#define BASEHL2MPVIEWMODEL_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#include "predicted_viewmodel.h"

#if defined( CLIENT_DLL )
#define CBaseHL2MPViewModel C_BaseHL2MPViewModel
#define CBaseHL2MPCombatWeapon C_BaseHL2MPCombatWeapon
#endif

class CBaseHL2MPViewModel : public CPredictedViewModel
{
	DECLARE_CLASS(CBaseHL2MPViewModel, CPredictedViewModel);
public:

	DECLARE_NETWORKCLASS();

	CBaseHL2MPViewModel(void);
	~CBaseHL2MPViewModel(void);

	void CalcViewModelView(CBasePlayer *owner, const Vector& eyePosition, const QAngle& eyeAngles);

	bool ShouldFlipViewModel();
};

#endif // BASEHL2MPVIEWMODEL_SHARED_H