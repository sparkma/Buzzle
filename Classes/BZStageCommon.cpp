
#include "BZStageCommon.h"
#include "AWorld.h"
#include "BZStageLayerCommon.h"

BZStageCommon::BZStageCommon(const char* name) : CAStage(name)
{
}

BZStageCommon::~BZStageCommon(void)
{
}

/*
void BZStageCommon::move2Layer(BZStageLayerCommon* playerFrom, const char* layerTo)
{
	BZStageLayerCommon* playerTo = (BZStageLayerCommon*)this->getSubLayer(layerTo);
	playerFrom->hide();
	playerTo->setFrom(playerFrom->name());
	playerTo->show();
}

void BZStageCommon::back2Layer(BZStageLayerCommon* playerFrom, const char* layerTo)
{
	BZStageLayerCommon* playerTo = (BZStageLayerCommon*)this->getSubLayer(layerTo);
	playerFrom->hide();
	playerTo->show();
}

void BZStageCommon::pushLayer(BZStageLayerCommon* playerFrom, const char* layerTo)
{
	BZStageLayerCommon* playerTo = (BZStageLayerCommon*)this->getSubLayer(layerTo);
	playerFrom->pause();
	playerTo->show();
}

void BZStageCommon::popLayer(BZStageLayerCommon* playerFrom, const char* layerTo)
{
	BZStageLayerCommon* playerTo = (BZStageLayerCommon*)this->getSubLayer(layerTo);
	playerFrom->pause();
	playerTo->show();
}
*/

REG_CLASS(BZStageCommon);
