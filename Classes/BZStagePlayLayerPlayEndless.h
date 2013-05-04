#if !defined(_STAGE_PLAY_LAYER_ENDLESS_H_)
#define _STAGE_PLAY_LAYER_ENDLESS_H_

#include "BZStageLayerCommon.h"

class BZStagePlayLayerPlayEndless :
	public BZStageLayerCommon
{
protected:
	virtual void _onResume();
	virtual void _onButtonCommand(CASprite* pbutton);
	void _pause();

	virtual void _onHome();
	virtual void _onRestart();
	virtual void _onGameOver();
public:
	BZStagePlayLayerPlayEndless(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerPlayEndless(void);

	virtual void onStateBegin(CAState* from, void* param);
	virtual bool onEvent(const CAEvent* pevt);
};

#endif //_STAGE_PLAY_LAYER_ENDLESS_H_