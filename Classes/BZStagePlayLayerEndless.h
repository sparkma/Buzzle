#if !defined(_STAGE_PLAY_LAYER_ENDLESS_H_)
#define _STAGE_PLAY_LAYER_ENDLESS_H_

#include "BZStageLayerCommon.h"

class BZStagePlayLayerEndless :
	public BZStageLayerCommon
{
protected:
	virtual void _onResume();
	virtual void _onButtonCommand(CASprite* pbutton);
public:
	BZStagePlayLayerEndless(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerEndless(void);
};

#endif //_STAGE_PLAY_LAYER_ENDLESS_H_