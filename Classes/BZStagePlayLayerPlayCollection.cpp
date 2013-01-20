
#include "BZStagePlayLayerPlayCollection.h"
#include "BZStageCommon.h"
#include "BZStagePlayLayerDialog.h"

#include "AString.h"

BZStagePlayLayerPlayCollection::BZStagePlayLayerPlayCollection(CAStage* pstage, CAStageLayer* playerParent) : BZStageLayerCommon(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
}

BZStagePlayLayerPlayCollection::~BZStagePlayLayerPlayCollection(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStagePlayLayerPlayCollection::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	if ("button_pause" == btn)
	{
		this->showDialog("collection_pause");
		//pstage->pushLayer(this, "collection_pause");
	}
}

void BZStagePlayLayerPlayCollection::_onResume()
{
	BZStageLayerCommon::_onResume();

	const string& result = this->_dialogResult;
	if (result == "button_home")
	{
		this->replaceLayer("home");
	}
	else if (result == "button_select_level")
	{
		this->replaceLayer("menu_collection");
	}
	else if (result == "button_restart")
	{
	}
	else if (result == "button_resume")
	{
		//do nothing
	}
}

//#include "AWorld.h"
//REG_CLASS(BZStagePlayLayerPlayCollection);
