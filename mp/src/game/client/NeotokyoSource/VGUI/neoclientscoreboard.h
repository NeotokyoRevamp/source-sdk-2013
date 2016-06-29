#ifndef NEOCLIENTSCOREBOARDDIALOG_H
#define NEOCLIENTSCOREBOARDDIALOG_H

#ifdef _WIN32
#pragma once
#endif

#include <clientscoreboarddialog.h>

class CNEOClientScoreBoardDialog : public CClientScoreBoardDialog
{
	DECLARE_CLASS_SIMPLE( CNEOClientScoreBoardDialog, CClientScoreBoardDialog );

public:
	CNEOClientScoreBoardDialog( IViewPort *pViewPort );
	~CNEOClientScoreBoardDialog();

	virtual bool GetPlayerScoreInfo( int playerIndex, KeyValues *outPlayerInfo );
	virtual void InitScoreboardSections();
	virtual void UpdateTeamInfo();
	virtual void UpdatePlayerInfo();
	virtual void AddHeader();
	virtual void AddSection( int teamType, int teamNumber );

	virtual void UpdateServerInfo( KeyValues* kv );

	void SetLabelColorWhite( const char* labelName );

private:
	enum
	{
		NEO_NAME_WIDTH = 180,
		NEO_RANK_WIDTH = 75,
		NEO_CLASS_WIDTH = 50,
		NEO_STAR_WIDTH = 50,
		NEO_FRAGS_WIDTH = 40,
		NEO_DEATH_WIDTH = 40,
		NEO_PING_WIDTH = 40,
		NEO_ALIVE_WIDTH = 40
	};
};

#endif // NEOCLIENTSCOREBOARDDIALOG_H