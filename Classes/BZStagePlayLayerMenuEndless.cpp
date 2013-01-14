
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

void BZStagePlayLayerMenuEndless::_goBack()
{
	replaceLayer("home");
}

void BZStagePlayLayerMenuEndless::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	//_buttonCommand = btn;
	if ("button_home" == btn)
	{
		_goBack();
	}
	else if ("button_help" == btn)
	{
		showDialog("help");
	}
	else if ("mi_newgame" == btn)
	{
		CAWorld::sharedWorld().gameenv().setString("mode", "endless");
		CAWorld::sharedWorld().gameenv().setString("how", "newgame");
		replaceLayer("play_endless");
	}
	else if ("mi_continue" == btn)
	{
		CAWorld::sharedWorld().gameenv().setString("mode", "endless");
		CAWorld::sharedWorld().gameenv().setString("how", "continue");
		replaceLayer("play_endless");
	}
}

void BZStagePlayLayerMenuEndless::onEvent(const CAEvent* pevt)
{
	BZStageLayerCommon::onEvent(pevt);

	switch (pevt->type())
	{
	case ET_Touch:
		break;
	case ET_Key:
		{
			CAEventKey* pek = (CAEventKey*)pevt;
			if (KE_Back == pek->key() || KE_Menu == pek->key())
			{
				_goBack();
			}
		}
		break;
	case ET_Command:
		break;
	case ET_Gesture:
		break;
	}
}

#include "AWorld.h"
REG_CLASS(BZStagePlayLayerMenuEndless);
