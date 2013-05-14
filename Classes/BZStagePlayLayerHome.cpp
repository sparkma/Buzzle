
#include "BZStagePlayLayerHome.h"
#include "BZStageCommon.h"
#include "BZSpriteButton.h"

#include "AWorld.h"
#include "AString.h"
#include "AUserData.h"

BZStagePlayLayerHome::BZStagePlayLayerHome(CAStage* pstage, CAStageLayer* playerParent) : BZStageLayerCommon(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
	_bFirstRun = true;
}

BZStagePlayLayerHome::~BZStagePlayLayerHome(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStagePlayLayerHome::onStateBegin(CAState* from, void* param) 
{
	GUARD_FUNCTION();

	const string& fname = from->getFullName();
	if (0) ;
	else _HANDLE_STATE(idle, if (_bFirstRun) { this->setConditionResult("root.idle@user.show", true); _bFirstRun = false; })
	else _HANDLE_STATE(mode_fadein, (void)0)
	else _HANDLE_STATE(mode_running, 
	{
		_pstage->musicResetPlaylist(_musics, _pcount);
		_pstage->setFocus(this); 
		BZSpriteButton* pbutton = (BZSpriteButton*)this->_getNamedSprite("game_button_audio", 0);
		if (pbutton) pbutton->setState(_pstage->isAudioMute() ? "off_home" : "on_home");
	})
	else _HANDLE_STATE(mode_fadeout, (void)0)
	else _HANDLE_STATE(load_fadein, (void)0)
	else _HANDLE_STATE(load_running, _pstage->setFocus(this);)
	else _HANDLE_STATE(load_fadeout, (void)0)
	else
	{
		BZStageLayerCommon::onStateBegin(from, param);
	}
}

void BZStagePlayLayerHome::_beginPlay(const char* mode, bool newgame)
{
	CAWorld::sharedWorld().gameenv().setString("mode", mode);
	CAWorld::sharedWorld().gameenv().setString("difficulty", _difficulty);
	CAWorld::sharedWorld().gameenv().setBool("newgame", newgame);
	string layer = "game_";
	layer += mode;
	replaceLayer(layer.c_str());
}

void BZStagePlayLayerHome::_handleModeCommand(const char* diff)
{
	string str;
	str = "mode_";
	str += diff;
	str += "_level";

	_difficulty = diff;

	int level = CAUserData::sharedUserData().getInteger(str.c_str(), 0);
#if 0
	if (level <= 0)
	{
		this->setConditionResult("root.mode_running@user.finished", true);
		_beginPlay("endless", true);
	}
	else
#endif
	{
		this->setConditionResult("root.mode_running@user.load", true);
	}
}

void BZStagePlayLayerHome::_handleLoadCommand(bool newgame)
{
	this->setConditionResult("root.load_running@user.finished", true);
	_beginPlay("endless", newgame);
}

void BZStagePlayLayerHome::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	if ("home_menu_easy" == btn)
	{
		_handleModeCommand("easy");
	}
	if ("home_menu_normal" == btn)
	{
		_handleModeCommand("normal");
	}
	if ("home_menu_hard" == btn)
	{
		_handleModeCommand("hard");
	}
	if ("home_menu_newgame" == btn)
	{
		_handleLoadCommand(true);
	}
	if ("home_menu_continue" == btn)
	{
		_handleLoadCommand(false);
	}
	if ("home_menu_back" == btn)
	{
		this->setConditionResult("root.load_running@user.back", true);
	}
	if ("game_button_audio" == btn)
	{
		bool mute = _pstage->isAudioMute();
		pbutton->setState(mute ? "on_home" : "off_home");
		_pstage->enableAudio(mute);
	}
}

#include "AWorld.h"
REG_CLASS(BZStagePlayLayerHome);
