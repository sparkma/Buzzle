#if !defined(_STAGE_PLAY_LAYER_COLLECTION_LOGIC_H_)
#define _STAGE_PLAY_LAYER_COLLECTION_LOGIC_H_

#include "BZStagePlayLayerPlayCollection.h"

class BZStagePlayLayerPlayCollectionLogic :
	public BZStagePlayLayerPlayCollection
{
protected:
public:
	BZStagePlayLayerPlayCollectionLogic(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerPlayCollectionLogic(void);
};

#endif //_STAGE_PLAY_LAYER_COLLECTION_LOGIC_H_