
#include "BZStagePlayLayerCollection.h"
#include "BZStageCommon.h"
#include "BZStagePlayLayerDialog.h"

#include "AString.h"

BZStagePlayLayerCollection::BZStagePlayLayerCollection(CAStage* pstage, CAStageLayer* playerParent) : BZStageLayerCommon(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
}

BZStagePlayLayerCollection::~BZStagePlayLayerCollection(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStagePlayLayerCollection::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	if ("button_pause" == btn)
	{
		this->showDialog("collection_pause");
		//pstage->pushLayer(this, "collection_pause");
	}
}

void BZStagePlayLayerCollection::_onResume()
{
	BZStageLayerCommon::_onResume();

	BZStagePlayLayerDialog* pdlg = (BZStagePlayLayerDialog*)_pstage->getSubLayer("collection_pause");
	_Assert(pdlg);
	const string& result = pdlg->getResult();
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

#include "AWorld.h"
REG_CLASS(BZStagePlayLayerCollection);
