
#include "BZStagePlayLayerGamePlayPause.h"
#include "AWorld.h"
#include "AStage.h"
#include "AString.h"
#include "BZSpriteCommon.h"
#include "BZSpriteButton.h"

BZStagePlayLayerGamePlayPause::BZStagePlayLayerGamePlayPause(CAStage* pstage, CAStageLayer* playerParent) : CAStageLayer(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
	_NullGetters();
	_psprSound = null;
	_psprHelp = null;
	_pmenu = null;
}

BZStagePlayLayerGamePlayPause::~BZStagePlayLayerGamePlayPause(void)
{
	if (null != _pmenu)
	{
		_pmenu->release();
		_pmenu = null;
	}

	_Trace("%s destroyed", __FUNCTION__);
}

bool BZStagePlayLayerGamePlayPause::checkCondition(CAState* from, const CATransition& trans)
{
	//root.idle:	inner condition
	//root.running:	button event
	if (CAMachine::checkCondition(from, trans))
		return true;

	string fname = from->getFullName();
	if (CAString::startWith(fname, "root.fade"))
	{
		const char* pszPose = "fadein";
		if (fname == "root.fadein")
		{
		}
		else if (fname == "root.fadeout")
		{
			pszPose = "fadeout";
		}
		else
		{
			_Assert(false);
		}

		bool result = false;
		_Assert(null != _pause_ui_back());
		if (_pause_ui_back()->isAnimationDone())
		{
			result = true;
		}
		/*
		else
		{
			float timeout = trans.timeout;
			if (timeout > 0.00001f && this->getTimeNow() - from->getTimeEnter() > timeout)
			{
				result = true;
			}
		}
		*/
		return result;
	}

	if (CAMachine::checkCondition(from, trans))
		return true;

	return false;
}

void BZStagePlayLayerGamePlayPause::onBarClicked(const string& id)
{
	_selectedMenuBar = id;
}

void BZStagePlayLayerGamePlayPause::_handleMenuMessage(const string& _id)
{
	string id = _id;

	if (id == "restart")
	{
		this->setConditionResult("root.running@user.restart", true);
	}
	else if (id == "save_quit")
	{
		this->setConditionResult("root.running@user.save_quit", true);
	}
	else if (id == "resume")
	{
		this->setConditionResult("root.running@user.resume", true);
	}
}

void BZStagePlayLayerGamePlayPause::_addMenuBar(const char* lab)
{
	float menu_size = _settings.getFloat("menu_size");
	int   menu_cols = _settings.getInteger("menu_cols");
	float menu_zord = _settings.getFloat("menu_zorder");
	float menu_labs = _settings.getFloat("menu_lablescale");

	{
		string key;
		key = "menu_"; key += lab; 

		CCPoint pos = _settings.getPoint((key + "_lefttop").c_str());
		//string id = _settings.getString((key + "_id").c_str());
		string bubbletype = _settings.getString((key + "_type").c_str());

		string id = lab;
		string label = key;

		_pmenu->addBar(id.c_str(), label.c_str(), menu_labs, bubbletype.c_str(), 
			pos, menu_cols, menu_size, menu_zord);
	}
 }

void BZStagePlayLayerGamePlayPause::onStateBegin(CAState* from, void* param) 
{
	GUARD_FUNCTION();

	const string& fname = from->getFullName();
	if (0) ;
	else if (CAString::startWith(fname, "root.idle"))		//_onStateBeginIdle(from);
	{
		_NullGetters();
		this->removeAllTimelines();
	}
	else if (CAString::startWith(fname, "root.create"))
	{
		this->activeAllTimelines();
	}
	else if (CAString::startWith(fname, "root.fadein"))		//_onStateBeginFadein(from);
	{
		_InitGetters();

		CASprite* psprsControls[] =
		{
			//_label_pause_title(),
			_pause_ui_back(),
			null,
		};
		_setSpritesState(STATE_Fadein, psprsControls);

		CCPoint pos;
		float zp;
		//button sound on/off
		_psprSound = new BZSpriteButton(this, "pause_ui_button_sound");
		_psprSound->setVisible(false);
		bool bMute = stage()->isSoundMute() || stage()->isMusicMute();
		_psprSound->setState(bMute ? "off" : "on");
		pos = _settings.getPoint("button_sound_pos");
		CAWorld::percent2view(pos);
		zp = _settings.getFloat("button_sound_z");
		_psprSound->setZOrder(zp);
		_psprSound->setPos(pos);
		this->addSprite(_psprSound);

		//button help
		_psprHelp = new BZSpriteButton(this, "pause_ui_button_help");
		_psprHelp->setVisible(false);
		_psprHelp->setState("stand");
		pos = _settings.getPoint("button_help_pos");
		CAWorld::percent2view(pos);
		zp = _settings.getFloat("button_help_z");
		_psprHelp->setZOrder(zp);
		_psprHelp->setPos(pos);
		this->addSprite(_psprHelp);
	}
	else if (CAString::startWith(fname, "root.running"))	//_onStateBeginRunning(from);
	{
		_psprSound->setVisible(true);
		_psprHelp->setVisible(true);

		if (null != _pmenu) _pmenu->release();
		_pmenu = new BZGameMenu(this, this);
		_addMenuBar("resume");
		_addMenuBar("restart");
		_addMenuBar("save_quit");
	}
	else if (CAString::startWith(fname, "root.onresume"))	//_onStateBeginOnResume(from);
	{
		_Assert(this->_playerParent);
		this->_playerParent->onEvent(new CAEventCommand(this, EVENT_ONRESUME));
	}
	else if (CAString::startWith(fname, "root.onrestart"))	//_onStateBeginOnRestart(from);
	{
		_Assert(this->_playerParent);
		this->_playerParent->onEvent(new CAEventCommand(this, EVENT_ONRESTART));
	}
	else if (CAString::startWith(fname, "root.onsave_quit"))	//_onStateBeginOnRestart(from);
	{
		_Assert(this->_playerParent);
		this->_playerParent->onEvent(new CAEventCommand(this, EVENT_ONSAVE_QUIT));
	}
	else if (CAString::startWith(fname, "root.fadeout"))	//_onStateBeginFadeout(from);
	{
		CASprite* psprsControls[] =
		{
			//_label_pause_title(),
			_pause_ui_back(),
			null,
		};
		_setSpritesState(STATE_Fadeout, psprsControls);
		this->removeSprite(_psprSound);
		_psprSound = null;
		this->removeSprite(_psprHelp);
		_psprHelp = null;
		if (null != _pmenu)
		{
			_pmenu->release();
			_pmenu = null;
		}
	}
	else if (CAString::startWith(fname, "root.clean"))
	{
		//_pause_ui_back()->setVisible(false);
		//this->removeAllTimelines();
	}
	else ;
};

void BZStagePlayLayerGamePlayPause::onStateEnd(CAState* from, void* param) 
{
	const string& fname = from->getFullName();
	if (0) ;
	else if (CAString::startWith(fname, "root.idle"))	
	{
	}
	else if (CAString::startWith(fname, "root.fadein"))	
	{
	}
	else if (CAString::startWith(fname, "root.running"))
	{
	}
	else if (CAString::startWith(fname, "root.onresume"))
	{
	}
	else if (CAString::startWith(fname, "root.onrestart"))	
	{
	}
	else if (CAString::startWith(fname, "root.onsave_quit"))	
	{
	}
	else if (CAString::startWith(fname, "root.fadeout"))
	{
	}
	else ;
};

void BZStagePlayLayerGamePlayPause::show(bool s)
{
	if (s)
	{
		this->navigateToState(getTimeNow(), this->getCurrentState(), "root.idle");
		this->setConditionResult("root.idle@user.show", true);
	}
	else
	{
		this->navigateToState(getTimeNow(), this->getCurrentState(), "root.fadeout");
	}
}

void BZStagePlayLayerGamePlayPause::onEnter()
{
	GUARD_FUNCTION();

	CAStageLayer::onEnter();
}

static int _goNear(int cur, int to, int range, int step)
{
	if (cur == to) return cur;
	if ((cur - to < 0 && to - cur > (range >> 1)) ||
		(cur - to > 0 && cur - to < (range >> 1)))
	{
		cur += range - step;
	}
	else
	{
		cur += step;
	}
	cur %= range;

	return cur;
}

void BZStagePlayLayerGamePlayPause::onUpdate() 
{
	CAStageLayer::onUpdate();
	if (_pmenu)
	{
		_pmenu->onUpdate();
	}
	if (_selectedMenuBar.size() > 0)
	{
		this->_handleMenuMessage(_selectedMenuBar);
		_selectedMenuBar = "";
	}
};

void BZStagePlayLayerGamePlayPause::onExit()
{
	CAStageLayer::onExit();
}

void BZStagePlayLayerGamePlayPause::onEvent(CAEvent* pevt)
{
	CAStageLayer::onEvent(pevt);

	switch (pevt->type())
	{
	case ET_Touch:
		{
			CAEventTouch* ptouch = (CAEventTouch*)pevt;
			if (null != _pmenu)
			{
				_pmenu->onEvent(pevt);
			}
			switch (ptouch->state())
			{
			case kTouchStateGrabbed:
				{
				}
				break;
			case kTouchStateUngrabbed:
				{
				}
				break;
			}
		}
		break;
	case ET_Command:
		{
			CAEventCommand* pec = (CAEventCommand*)pevt;
			if (pec->command() == "onClick")
			{
				_Assert(pec->getSenderType() == ST_Sprite);
				CASprite* pspr = (CASprite*)pec->sender();
				string name;
				name = pspr->getModName();
				if (name == "pause_ui_button_sound" && null != _psprSound)
				{
					if (stage()->isSoundMute() || stage()->isMusicMute())
					{
						stage()->enableSound(true);
						stage()->enableMusic(true);
						_psprSound->setState("on");
					}
					else
					{
						stage()->enableSound(false);
						stage()->enableMusic(false);
						_psprSound->setState("off");
					}
				}
				else if (name == "pause_ui_button_help" && null != _psprHelp)
				{
					int n = 0;
				}
			}
		}
		break;
	}
}

