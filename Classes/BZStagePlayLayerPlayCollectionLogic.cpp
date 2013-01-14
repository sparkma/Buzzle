
#include "BZStagePlayLayerPlayCollectionLogic.h"
#include "BZStageCommon.h"
#include "BZStagePlayLayerDialog.h"

#include "AString.h"

BZStagePlayLayerPlayCollectionLogic::BZStagePlayLayerPlayCollectionLogic(CAStage* pstage, CAStageLayer* playerParent) 
: BZStagePlayLayerPlayCollection(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
}

BZStagePlayLayerPlayCollectionLogic::~BZStagePlayLayerPlayCollectionLogic(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

#include "AWorld.h"
REG_CLASS(BZStagePlayLayerPlayCollectionLogic);
