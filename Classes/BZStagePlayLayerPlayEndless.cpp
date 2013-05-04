
#include "BZStagePlayLayerPlayEndless.h"
#include "BZStageCommon.h"
#include "BZStagePlayLayerDialog.h"
#include "BZSpriteButton.h"

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
	this->showDialog("dialog_paused", 30.0f);
}

void BZStagePlayLayerPlayEndless::onStateBegin(CAState* from, void* param) 
{
	GUARD_FUNCTION();

	const string& fname = from->getFullName();
	if (0) ;
	else _HANDLE_STATE(running, 
	{
		BZSpriteButton* pbutton = (BZSpriteButton*)this->_getNamedSprite("game_button_audio", 0);
		if (pbutton) pbutton->setState(_pstage->isAudioMute() ? "off_game" : "on_game");
		BZStageLayerCommon::onStateBegin(from, param);
	})
	else
	{
		BZStageLayerCommon::onStateBegin(from, param);
	}
}


void BZStagePlayLayerPlayEndless::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	if ("game_button_pause" == btn)
	{
		_pause();
	}
	else if ("game_button_audio" == btn)
	{
		bool mute = _pstage->isAudioMute();
		pbutton->setState(mute ? "on_game" : "off_game");
		_pstage->enableAudio(mute);
	}
}

void BZStagePlayLayerPlayEndless::_onHome()
{
	this->replaceLayer("home");
}

void BZStagePlayLayerPlayEndless::_onRestart()
{
}

void BZStagePlayLayerPlayEndless::_onGameOver()
{
	this->showDialog("dialog_gameover", 30.0f);
}

void BZStagePlayLayerPlayEndless::_onResume()
{
	GUARD_FUNCTION();

	BZStageLayerCommon::_onResume();

	//BZStagePlayLayerDialog* pdlg = (BZStagePlayLayerDialog*)_pstage->getSubLayer("endless_pause");
	//_Assert(pdlg);
	//const string& result = pdlg->getResult();
	const string& result = this->_dialogResult;

	if (result == "game_button_home")
	{
		_onHome();
	}
	else if (result == "game_button_restart")
	{
		_onRestart();
		string signal = this->getCurrentState()->getFullName() + "@user.resumeback";
		this->setConditionResult(signal.c_str(), true);
	}
	else if (result == "game_button_resume")
	{
		string signal = this->getCurrentState()->getFullName() + "@user.resumeback";
		this->setConditionResult(signal.c_str(), true);
	}
}

bool BZStagePlayLayerPlayEndless::onEvent(const CAEvent* pevt)
{
	bool ret = BZStageLayerCommon::onEvent(pevt);

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
	return ret;
}

//#include "AWorld.h"
//REG_CLASS(BZStagePlayLayerPlayEndless);
