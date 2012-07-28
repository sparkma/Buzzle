
#include "BZStageTestLayerTest.h"

BZStageTestLayerTest::BZStageTestLayerTest(CAStage* pstage, CAStageLayer* playerParent) : CAStageLayer(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
}

BZStageTestLayerTest::~BZStageTestLayerTest(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}
