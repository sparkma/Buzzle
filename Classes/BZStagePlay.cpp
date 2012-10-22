
#include "BZStagePlay.h"

BZStagePlay::BZStagePlay(const char* name) : CAStage(name)
{
}

BZStagePlay::~BZStagePlay(void)
{
}

void BZStagePlay::onEnter()
{
	CAStage::onEnter();
	//this->setScale(0.5f);
}
