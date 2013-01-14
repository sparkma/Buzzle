#if !defined(_STAGE_PLAY_LAYER_ENDLESS_LOGIC_H_)
#define _STAGE_PLAY_LAYER_ENDLESS_LOGIC_H_

#include "BZStagePlayLayerPlayEndless.h"
#include "BZGroupNumber.h"
#include "BZGame.h"

class BZStagePlayLayerPlayEndlessLogic :
	public BZStagePlayLayerPlayEndless
{
protected:
	string	_state;
	int		_nScore;
	BZGroupNumber* _score;
	int		_nLevel;
	BZGroupNumber* _level;
	void _updateScoreAndLevel();
	
	BZGame* _pgame;
	void _initGame();

	void _onFadein();
	void _onRunning();
	void _onFadeout();
	virtual void _onHome();
	virtual void _onSaveQuit();
	virtual void _onRestart();
public:
	BZStagePlayLayerPlayEndlessLogic(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerPlayEndlessLogic(void);

	virtual void onUpdate();
	virtual void onEvent(const CAEvent* pevt);
	virtual void onExit();
};

#endif //_STAGE_PLAY_LAYER_ENDLESS_LOGIC_H_