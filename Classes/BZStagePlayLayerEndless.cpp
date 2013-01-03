
#include "BZStagePlayLayerEndless.h"
#include "BZStageCommon.h"
#include "BZStagePlayLayerDialog.h"

#include "AString.h"

BZStagePlayLayerEndless::BZStagePlayLayerEndless(CAStage* pstage, CAStageLayer* playerParent) : BZStageLayerCommon(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
}

BZStagePlayLayerEndless::~BZStagePlayLayerEndless(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStagePlayLayerEndless::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	if ("button_pause" == btn)
	{
		this->showDialog("endless_pause");
	}
}

void BZStagePlayLayerEndless::_onResume()
{
	BZStageLayerCommon::_onResume();

	BZStagePlayLayerDialog* pdlg = (BZStagePlayLayerDialog*)_pstage->getSubLayer("endless_pause");
	_Assert(pdlg);
	const string& result = pdlg->getResult();
	if (result == "button_home")
	{
		this->replaceLayer("home");
	}
	else if (result == "button_save_quit")
	{
		this->replaceLayer("menu_endless");
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
REG_CLASS(BZStagePlayLayerEndless);
