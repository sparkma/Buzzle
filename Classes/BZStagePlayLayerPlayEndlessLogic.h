#if !defined(_STAGE_PLAY_LAYER_ENDLESS_LOGIC_H_)
#define _STAGE_PLAY_LAYER_ENDLESS_LOGIC_H_

#include "BZStagePlayLayerPlayEndless.h"
#include "BZGroupNumber.h"
#include "BZGameClassic.h"

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
	
	BZGameClassic* _pgame;
	void _initGame(bool restart);

	void _onFadein();
	void _onRunning();
	void _onFadeout();
	virtual void _onHome();
	virtual void _onRestart();
	virtual void _onResume();
	//virtual void _onGameOver();

	virtual void _onButtonCommand(CASprite* pbutton);
public:
	BZStagePlayLayerPlayEndlessLogic(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerPlayEndlessLogic(void);

	virtual void onUpdate();
	virtual bool onEvent(const CAEvent* pevt);
	virtual void onExit();
};

#endif //_STAGE_PLAY_LAYER_ENDLESS_LOGIC_H_