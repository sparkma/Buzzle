#if !defined(_STAGE_TEST_LAYER_TEST_H_)
#define _STAGE_TEST_LAYER_TEST_H_

#include "AStageLayer.h"

class BZStageTestLayerTest :
	public CAStageLayer
{
public:
	BZStageTestLayerTest(CAStage* pstage, CAStageLayer* playerParent);
	virtual ~BZStageTestLayerTest(void);
};

#endif //_STAGE_TEST_LAYER_TEST_H_