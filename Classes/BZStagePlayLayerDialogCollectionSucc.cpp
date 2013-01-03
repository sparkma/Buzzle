
#include "BZStagePlayLayerDialogCollectionSucc.h"
#include "BZStageCommon.h"

#include "AWorld.h"
#include "AString.h"

BZStagePlayLayerDialogCollectionSucc::BZStagePlayLayerDialogCollectionSucc(CAStage* pstage, CAStageLayer* playerParent) : BZStagePlayLayerDialog(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
}

BZStagePlayLayerDialogCollectionSucc::~BZStagePlayLayerDialogCollectionSucc(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStagePlayLayerDialogCollectionSucc::_onIdle()
{
	BZStageLayerCommon::_onIdle();
}

void BZStagePlayLayerDialogCollectionSucc::_onRunning()
{
	BZStageLayerCommon::_onRunning();
}

void BZStagePlayLayerDialogCollectionSucc::onUpdate()
{
	BZStageLayerCommon::onUpdate();
	if (CAMachine::getCurrentState()->getFullName() == "root.running")
	{
	}
}

void BZStagePlayLayerDialogCollectionSucc::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	if ("button_help" == btn)
	{
		this->showDialog("help", 15.0f);
	}
	else if ("button_resume" == btn)
	{
		doBack(_from.c_str());
	}
}

void BZStagePlayLayerDialogCollectionSucc::onEvent(const CAEvent* pevt)
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
REG_CLASS(BZStagePlayLayerDialogCollectionSucc);
