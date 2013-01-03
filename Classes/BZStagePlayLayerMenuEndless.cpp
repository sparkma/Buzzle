
#include "BZStagePlayLayerMenuEndless.h"
#include "BZStageCommon.h"

#include "AWorld.h"
#include "AString.h"

BZStagePlayLayerMenuEndless::BZStagePlayLayerMenuEndless(CAStage* pstage, CAStageLayer* playerParent) : BZStageLayerCommon(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
}

BZStagePlayLayerMenuEndless::~BZStagePlayLayerMenuEndless(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStagePlayLayerMenuEndless::onUpdate()
{
	BZStageLayerCommon::onUpdate();
	if (CAMachine::getCurrentState()->getFullName() == "root.running")
	{
	}
}

void BZStagePlayLayerMenuEndless::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	//_buttonCommand = btn;
	if ("button_home" == btn)
	{
		replaceLayer("home");
	}
	else if ("button_help" == btn)
	{
		showDialog("help");
	}
	else if ("mi_newgame" == btn)
	{
		replaceLayer("play_endless");
	}
	else if ("mi_continue" == btn)
	{
		replaceLayer("play_endless");
	}
}

#include "AWorld.h"
REG_CLASS(BZStagePlayLayerMenuEndless);
