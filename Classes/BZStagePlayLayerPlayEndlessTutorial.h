#if !defined(_STAGE_PLAY_LAYER_ENDLESS_TUTORIAL_H_)
#define _STAGE_PLAY_LAYER_ENDLESS_TUTORIAL_H_

#include "BZStagePlayLayerPlayEndlessTutorial.h"
#include "BZGroupNumber.h"
#include "BZGameClassic.h"
#include "BZStageLayerCommon.h"

class BZStagePlayLayerPlayEndlessTutorial :
	public BZStageLayerCommon
{
protected:
	string	_state;

	string _debug_log;

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
	BZStagePlayLayerPlayEndlessTutorial(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerPlayEndlessTutorial(void);

	virtual void onStateBegin(CAState* from, void* param);
	virtual string debuglog();

	virtual void onUpdate();
	virtual bool onEvent(const CAEvent* pevt);
	virtual void onExit();
};

#endif //_STAGE_PLAY_LAYER_ENDLESS_TUTORIAL_H_