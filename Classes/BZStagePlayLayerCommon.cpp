
#include "BZStagePlayLayerCommon.h"

BZStagePlayLayerCommon::BZStagePlayLayerCommon(CAStage* pstage, CAStageLayer* playerParent) : CAStageLayer(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
}

BZStagePlayLayerCommon::~BZStagePlayLayerCommon(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}
