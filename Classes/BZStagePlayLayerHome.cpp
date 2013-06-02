
#include "BZStagePlayLayerHome.h"
#include "BZStageCommon.h"
#include "BZSpriteButton.h"

#include "AWorld.h"
#include "AString.h"
#include "AUserData.h"

BZStagePlayLayerHome::BZStagePlayLayerHome(CAStage* pstage, CAStageLayer* playerParent) : BZStageLayerCommon(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
	//_bFirstRun = true;
}

BZStagePlayLayerHome::~BZStagePlayLayerHome(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStagePlayLayerHome::_onIdle()
{
	BZStageLayerCommon::_onIdle();
	//if (_bFirstRun) 
	//{ 
	//	this->setConditionResult("root.idle@user.show", true); 
	//	_bFirstRun = false; 
	//}
}

void BZStagePlayLayerHome::_onRunning()
{
	BZStageLayerCommon::_onRunning();
	postGameEvent("ui", "home");
	BZSpriteButton* pbutton = (BZSpriteButton*)this->_getNamedSprite("game_button_audio", 0);
	if (pbutton) pbutton->setState(_pstage->isAudioMute() ? "off_home" : "on_home");
}

void BZStagePlayLayerHome::onStateBegin(CAState* from, void* param) 
{
	GUARD_FUNCTION();
	BZStageLayerCommon::onStateBegin(from, param);
}

void BZStagePlayLayerHome::_handleModeCommand(const char* mode, const char* diff)
{
	CAWorld::sharedWorld()->gameenv().setString("mode", mode);
	CAWorld::sharedWorld()->gameenv().setString("difficulty", diff); //_difficulty);

	string layer = "game_";
	layer += mode;
	layer += "level";
	replaceLayer(layer.c_str());
}

void BZStagePlayLayerHome::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();

	const char* mode = "endless";
	if ("home_menu_easy" == btn)
	{
		_handleModeCommand(mode, "easy");
	}
	if ("home_menu_normal" == btn)
	{
		_handleModeCommand(mode, "normal");
	}
	if ("home_menu_hard" == btn)
	{
		_handleModeCommand(mode, "hard");
	}
	if ("game_button_audio" == btn)
	{
		bool mute = _pstage->isAudioMute();
		pbutton->setState(mute ? "on_home" : "off_home");
		_pstage->enableAudio(mute);
	}
}

bool BZStagePlayLayerHome::onEvent(const CAEvent* pevt)
{
	bool ret = BZStageLayerCommon::onEvent(pevt);

	switch (pevt->type())
	{
	case ET_Key:
		{
			CAEventKey* pek = (CAEventKey*)pevt;
			if (KE_Back == pek->key())// || KE_Menu == pek->key())
			{
				string fname = this->getCurrentState()->getLeafState()->getFullName();
				if (CAString::startWith(fname, "root.running"))
				{
					postGameEvent("ui", "exit");
				}
			}
		}
		break;
	}
	return ret;
}

#include "AWorld.h"
REG_CLASS(BZStagePlayLayerHome);
