#if !defined(_STAGE_PLAY_LAYER_HOME_H_)
#define _STAGE_PLAY_LAYER_HOME_H_

#include "BZStageLayerCommon.h"

class BZStagePlayLayerHome :
	public BZStageLayerCommon
{
protected:
	virtual void _onButtonCommand(CASprite* pbutton);
public:
	BZStagePlayLayerHome(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerHome(void);
};

#endif //_STAGE_PLAY_LAYER_HOME_H_