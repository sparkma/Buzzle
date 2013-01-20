
#include "BZStagePlayLayerDialogEndlessPause.h"
#include "BZStageCommon.h"

#include "AWorld.h"
#include "AString.h"

BZStagePlayLayerDialogEndlessPause::BZStagePlayLayerDialogEndlessPause(CAStage* pstage, CAStageLayer* playerParent) : BZStagePlayLayerDialog(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
}

BZStagePlayLayerDialogEndlessPause::~BZStagePlayLayerDialogEndlessPause(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStagePlayLayerDialogEndlessPause::_onIdle()
{
	BZStageLayerCommon::_onIdle();
}

void BZStagePlayLayerDialogEndlessPause::_onRunning()
{
	BZStageLayerCommon::_onRunning();
}

void BZStagePlayLayerDialogEndlessPause::onUpdate()
{
	BZStageLayerCommon::onUpdate();
	if (CAMachine::getCurrentState()->getFullName() == "root.running")
	{
	}
}

void BZStagePlayLayerDialogEndlessPause::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	if ("button_help" == btn)
	{
		this->showDialog("help", 65.0f);
	}
	else if ("button_resume" == btn)
	{
		doBack(_from.c_str(), btn.c_str());
	}
	else if ("button_home" == btn)
	{
		doBack(_from.c_str(), btn.c_str());
	}
	else if ("button_save_quit" == btn)
	{
		doBack(_from.c_str(), btn.c_str());
	}
	else if ("button_restart" == btn)
	{
		doBack(_from.c_str(), btn.c_str());
	}
}

void BZStagePlayLayerDialogEndlessPause::onEvent(const CAEvent* pevt)
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
REG_CLASS(BZStagePlayLayerDialogEndlessPause);
