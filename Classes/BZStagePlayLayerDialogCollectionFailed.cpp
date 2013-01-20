
#include "BZStagePlayLayerDialogCollectionFailed.h"
#include "BZStageCommon.h"

#include "AWorld.h"
#include "AString.h"

BZStagePlayLayerDialogCollectionFailed::BZStagePlayLayerDialogCollectionFailed(CAStage* pstage, CAStageLayer* playerParent) : BZStagePlayLayerDialog(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
}

BZStagePlayLayerDialogCollectionFailed::~BZStagePlayLayerDialogCollectionFailed(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStagePlayLayerDialogCollectionFailed::_onIdle()
{
	BZStageLayerCommon::_onIdle();
}

void BZStagePlayLayerDialogCollectionFailed::_onRunning()
{
	BZStageLayerCommon::_onRunning();
}

void BZStagePlayLayerDialogCollectionFailed::onUpdate()
{
	BZStageLayerCommon::onUpdate();
	if (CAMachine::getCurrentState()->getFullName() == "root.running")
	{
	}
}

void BZStagePlayLayerDialogCollectionFailed::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	if ("button_help" == btn)
	{
		this->showDialog("help", 15.0f);
	}
	else if ("button_resume" == btn)
	{
		doBack(_from.c_str(), btn.c_str());
	}
}

void BZStagePlayLayerDialogCollectionFailed::onEvent(const CAEvent* pevt)
{
	BZStageLayerCommon::onEvent(pevt);

	switch (pevt->type())
	{
	case ET_Touch:
		break;
	case ET_Command:
		break;
	}
}

#include "AWorld.h"
REG_CLASS(BZStagePlayLayerDialogCollectionFailed);
