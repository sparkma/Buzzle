
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

void BZStagePlayLayerDialogEndlessGameover::onUpdate()
{
	BZStageLayerCommon::onUpdate();
	if (CAMachine::getCurrentState()->getFullName() == "root.running")
	{
	}
}

void BZStagePlayLayerDialogEndlessGameover::onStateBegin(CAState* from, void* param) 
{
	GUARD_FUNCTION();

	const string& fname = from->getFullName();
	if (0) ;
	else _HANDLE_STATE(over_fadein, (void)0)
	else _HANDLE_STATE(over_running, 
	{
		_pstage->setFocus(this); 
	})
	else _HANDLE_STATE(over_fadeout, (void)0)
	else
	{
		BZStageLayerCommon::onStateBegin(from, param);
	}
}

void BZStagePlayLayerDialogEndlessGameover::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	if ("game_button_help" == btn)
	{
		this->showDialog("help", this->getZOrder() + 10.0f);
	}
	else if ("game_button_home" == btn)
	{
		doBack(_from.c_str(), btn.c_str());
	}
	else if ("game_button_restart" == btn)
	{
		doBack(_from.c_str(), btn.c_str());
	}
}

bool BZStagePlayLayerDialogEndlessGameover::onEvent(const CAEvent* pevt)
{
	bool ret = BZStageLayerCommon::onEvent(pevt);

	switch (pevt->type())
	{
	case ET_Touch:
		break;
	case ET_Command:
		break;
	}
	return ret;
}

#include "AWorld.h"
REG_CLASS(BZStagePlayLayerDialogEndlessGameover);
