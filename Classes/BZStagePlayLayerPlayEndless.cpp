
#include "BZStagePlayLayerPlayEndless.h"
#include "BZStageCommon.h"
#include "BZStagePlayLayerDialog.h"

#include "AString.h"

BZStagePlayLayerPlayEndless::BZStagePlayLayerPlayEndless(CAStage* pstage, CAStageLayer* playerParent) : BZStageLayerCommon(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
}

BZStagePlayLayerPlayEndless::~BZStagePlayLayerPlayEndless(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStagePlayLayerPlayEndless::_pause()
{
	this->showDialog("endless_pause", 30.0f);
}

void BZStagePlayLayerPlayEndless::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	if ("button_pause" == btn)
	{
		_pause();
	}
}

void BZStagePlayLayerPlayEndless::_onHome()
{
	this->replaceLayer("home");
}

void BZStagePlayLayerPlayEndless::_onSaveQuit()
{
	this->replaceLayer("menu_endless");
}

void BZStagePlayLayerPlayEndless::_onRestart()
{
}

void BZStagePlayLayerPlayEndless::_onGameOver()
{
	this->showDialog("endless_gameover", 30.0f);
}

void BZStagePlayLayerPlayEndless::_onResume()
{
	GUARD_FUNCTION();

	BZStageLayerCommon::_onResume();

	BZStagePlayLayerDialog* pdlg = (BZStagePlayLayerDialog*)_pstage->getSubLayer("endless_pause");
	_Assert(pdlg);
	const string& result = pdlg->getResult();
	if (result == "button_home")
	{
		_onHome();
	}
	else if (result == "button_save_quit")
	{
		_onSaveQuit();
	}
	else if (result == "button_restart")
	{
		_onRestart();
	}
	else if (result == "button_resume")
	{
		//do nothing
	}
}

void BZStagePlayLayerPlayEndless::onEvent(const CAEvent* pevt)
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
				_pause();
			}
		}
		break;
	case ET_Command:
		break;
	}
}

//#include "AWorld.h"
//REG_CLASS(BZStagePlayLayerPlayEndless);
