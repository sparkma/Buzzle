
#include "BZStagePlayLayerDialogCollectionPause.h"
#include "BZStageCommon.h"

#include "AWorld.h"
#include "AString.h"

BZStagePlayLayerDialogCollectionPause::BZStagePlayLayerDialogCollectionPause(CAStage* pstage, CAStageLayer* playerParent) : BZStagePlayLayerDialog(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
}

BZStagePlayLayerDialogCollectionPause::~BZStagePlayLayerDialogCollectionPause(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStagePlayLayerDialogCollectionPause::_onIdle()
{
	BZStageLayerCommon::_onIdle();
}

void BZStagePlayLayerDialogCollectionPause::_onRunning()
{
	BZStageLayerCommon::_onRunning();
}

void BZStagePlayLayerDialogCollectionPause::onUpdate()
{
	BZStageLayerCommon::onUpdate();
	if (CAMachine::getCurrentState()->getFullName() == "root.running")
	{
	}
}

void BZStagePlayLayerDialogCollectionPause::_onButtonCommand(CASprite* pbutton)
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
	else if ("button_home" == btn)
	{
		doBack(_from.c_str(), btn.c_str());
	}
	else if ("button_select_level" == btn)
	{
		doBack(_from.c_str(), btn.c_str());
	}
	else if ("button_restart" == btn)
	{
		doBack(_from.c_str(), btn.c_str());
	}
}

void BZStagePlayLayerDialogCollectionPause::onEvent(const CAEvent* pevt)
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
REG_CLASS(BZStagePlayLayerDialogCollectionPause);
