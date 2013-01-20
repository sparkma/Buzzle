
#include "BZStagePlayLayerDialogEndlessGameover.h"
#include "BZStageCommon.h"

#include "AWorld.h"
#include "AString.h"

BZStagePlayLayerDialogEndlessGameover::BZStagePlayLayerDialogEndlessGameover(CAStage* pstage, CAStageLayer* playerParent) : BZStagePlayLayerDialog(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
}

BZStagePlayLayerDialogEndlessGameover::~BZStagePlayLayerDialogEndlessGameover(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStagePlayLayerDialogEndlessGameover::_onIdle()
{
	BZStageLayerCommon::_onIdle();
}

void BZStagePlayLayerDialogEndlessGameover::_onRunning()
{
	BZStageLayerCommon::_onRunning();
}

void BZStagePlayLayerDialogEndlessGameover::onUpdate()
{
	BZStageLayerCommon::onUpdate();
	if (CAMachine::getCurrentState()->getFullName() == "root.running")
	{
	}
}

void BZStagePlayLayerDialogEndlessGameover::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	if ("button_help" == btn)
	{
		this->showDialog("help", 65.0f);
	}
	else if ("button_resume" == btn)
	{
		doBack(_from.c_str());
	}
}

void BZStagePlayLayerDialogEndlessGameover::onEvent(const CAEvent* pevt)
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
REG_CLASS(BZStagePlayLayerDialogEndlessGameover);
