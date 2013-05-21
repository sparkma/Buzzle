#if !defined(_STAGE_PLAY_LAYER_ENDLESS_LOGIC_H_)
#define _STAGE_PLAY_LAYER_ENDLESS_LOGIC_H_

#include "BZStagePlayLayerPlayEndless.h"
#include "BZGroupNumber.h"
#include "BZGameClassic.h"
#include "BZStageLayerCommon.h"

class BZStagePlayLayerPlayEndless :
	public BZStageLayerCommon
{
protected:
	string	_state;
	int		_nScore;
	BZGroupNumber* _score;
	int		_nLevel;

	string _debug_log;

	float	_oldLevelPercent;
	BZGroupNumber* _level;
	void _updateScoreAndLevel();
	
	BZGameClassic* _pgame;
	void _initGame(int restartlevel);

	void _onFadein();
	void _onRunning();
	void _onFadeout();
	virtual void _onHome();
	virtual void _onRestart();
	virtual void _onResume();
	//virtual void _onGameOver();

	virtual void _onButtonCommand(CASprite* pbutton);
public:
	BZStagePlayLayerPlayEndless(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerPlayEndless(void);

	virtual void onStateBegin(CAState* from, void* param);
	virtual string debuglog();

	virtual void onUpdate();
	virtual bool onEvent(const CAEvent* pevt);
	virtual void onExit();
};

#endif //_STAGE_PLAY_LAYER_ENDLESS_LOGIC_H_