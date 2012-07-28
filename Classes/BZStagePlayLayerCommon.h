#if !defined(_STAGE_PLAY_LAYER_COMMON_H_)
#define _STAGE_PLAY_LAYER_COMMON_H_

#include "AStageLayer.h"

class BZStagePlayLayerCommon :
	public CAStageLayer
{
public:
	BZStagePlayLayerCommon(CAStage* pstage, CAStageLayer* playerParent);
	virtual ~BZStagePlayLayerCommon(void);
};

#endif //_STAGE_PLAY_LAYER_COMMON_H_