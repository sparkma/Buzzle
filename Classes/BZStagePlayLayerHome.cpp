
#include "BZStagePlayLayerHome.h"
#include "BZStageCommon.h"

#include "AString.h"

BZStagePlayLayerHome::BZStagePlayLayerHome(CAStage* pstage, CAStageLayer* playerParent) : BZStageLayerCommon(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
}

BZStagePlayLayerHome::~BZStagePlayLayerHome(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStagePlayLayerHome::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	if ("button_help" == btn)
	{
		showDialog("help");
	}
	else if ("mi_collection" == btn)
	{
		replaceLayer("menu_collection");
	}
	else if ("mi_endless" == btn)
	{
		replaceLayer("menu_endless");
	}
}

#include "AWorld.h"
REG_CLASS(BZStagePlayLayerHome);
