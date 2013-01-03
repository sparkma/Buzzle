#if !defined(_STAGE_PLAY_LAYER_COLLECTION_H_)
#define _STAGE_PLAY_LAYER_COLLECTION_H_

#include "BZStageLayerCommon.h"

class BZStagePlayLayerCollection :
	public BZStageLayerCommon
{
protected:
	virtual void _onResume();
	virtual void _onButtonCommand(CASprite* pbutton);
public:
	BZStagePlayLayerCollection(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerCollection(void);
};

#endif //_STAGE_PLAY_LAYER_COLLECTION_H_