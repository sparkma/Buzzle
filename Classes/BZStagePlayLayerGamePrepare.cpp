
#include "BZStagePlayLayerGamePrepare.h"
#include "AWorld.h"
#include "AStage.h"
#include "AString.h"
#include "BZSpriteCommon.h"
#include "BZSpriteButton.h"

#include "BZGameMenu.h"

BZStagePlayLayerGamePrepare::BZStagePlayLayerGamePrepare(CAStage* pstage, CAStageLayer* playerParent) : CAStageLayer(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
	_NullGetters();
	_pmenu = null;
}

BZStagePlayLayerGamePrepare::~BZStagePlayLayerGamePrepare(void)
{
	_pmenu->release();
	_pmenu = null;

	_Trace("%s destroyed", __FUNCTION__);
}

string BZStagePlayLayerGamePrepare::debuglog() 
{ 
	char sz[256];
	sprintf(sz, "sprs=%d, state=%s", this->_getNamedSpritesCount(), 
		this->getCurrentState()->getLeafState()->getFullName().c_str());
	return sz;
}

bool BZStagePlayLayerGamePrepare::checkCondition(CAState* from, const CATransition& trans)
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
		float timeout = trans.timeout;
		if (timeout > 0 && this->getTimeNow() - from->getTimeEnter() > timeout)
		{
			result = true;
		}
		else
		{
			//check 3 controls pose
			CASprite* psprs[] = 
			{
				null,
			};
			result = _checkSpritePoseFinished(pszPose, psprs);
		}
		return result;
	}

	if (CAMachine::checkCondition(from, trans))
		return true;

	return false;
}


void BZStagePlayLayerGamePrepare::_findNumberSprites(const char* prefix, CASprite** ppsprs, int size)
{
	int i, len = strlen(prefix);
	int count = _getNamedSpritesCount(prefix);
	_Assert(count == size);

	for (i = 0; i < count; i++)
	{
		CASprite* pspr = _getNamedSprite(prefix, i);
		string gname = pspr->getGroupName();
		char c = gname[gname.length() - 1];
		_Assert(c >= '0' && c <= '9');
		int index = c - '0';
		_Assert (index >= 0 && index < size);
		{
			ppsprs[index] = pspr;
		}
	}
}

void BZStagePlayLayerGamePrepare::onBarClicked(const char* label)
{
}

void BZStagePlayLayerGamePrepare::onStateBegin(CAState* from, void* param) 
{
	const string& fname = from->getFullName();
	if (0) ;
	else if (CAString::startWith(fname, "root.idle"))		//_onStateBeginIdle(from);
	{
		_NullGetters();
		this->removeAllTimelines();
	}
	else if (CAString::startWith(fname, "root.create"))		//_onStateBeginFadein(from);
	{
		//_pstage->resetTimer();
		_pstage->setOffset(ccp(0, 0), 0.0f);
		this->activeAllTimelines();
	}
	else if (CAString::startWith(fname, "root.fadein"))		//_onStateBeginFadein(from);
	{
		_InitGetters();

		float bs = _settings.getFloat("menu_size");
		int cols = _settings.getInteger("menu_cols");
		CCPoint lt = _settings.getPoint("menu_classic_lefttop");
		string bubbletype = _settings.getString("menu_classic_type");

		_pmenu = new BZGameMenu(this, this);
		_pmenu->addBar("menu_classic", bubbletype.c_str(), lt, cols, bs);
		lt.y -= 0.09f;
		_pmenu->addBar("menu_tapboom", "bubble_blue", lt, cols, bs);

		/*
		const char* cm = "0123456789mc+";
		
#define NUMBER_COUNT 7
		CASprite* psprs[NUMBER_COUNT];
		_findNumberSprites("dist_last", psprs, NUMBER_COUNT);
		_dist_last.init(this, psprs, NUMBER_COUNT, cm);
		_findNumberSprites("dist_max", psprs, NUMBER_COUNT);
		_dist_max.init(this, psprs, NUMBER_COUNT, cm);
		_findNumberSprites("coin_last", psprs, NUMBER_COUNT);
		_coin_last.init(this, psprs, NUMBER_COUNT, cm);
		_findNumberSprites("coin_max", psprs, NUMBER_COUNT);
		_coin_max.init(this, psprs, NUMBER_COUNT, cm);
		*/
		CASprite* psprsStatic[] = 
		{
			null,
		};
		CASprite* psprsControls[] =
		{
			_prepare_ui_button_newgame(),
			null,
		};
		
		_setSpritesState(STATE_Fadein, psprsStatic);
		_setSpritesState(STATE_Fadein, psprsControls);
		//_button_music()->setState(stage()->isMusicMute() ? "off_fadein" : "on_fadein");
		//_button_sound()->setState(stage()->isSoundMute() ? "off_fadein" : "on_fadein");
	}
	else if (CAString::startWith(fname, "root.running"))	//_onStateBeginRunning(from);
	{
	}
	else if (CAString::startWith(fname, "root.onshop"))		//_onStateBeginOnShop(from);
	{
		//_Assert(this->_playerParent);
		//this->_playerParent->onEvent(new CAEventCommand(this, "onShop"));
	}
	else if (CAString::startWith(fname, "root.onplay"))		//_onStateBeginOnPlay(from);
	{
		_Assert(this->_playerParent);
		this->_playerParent->onEvent(new CAEventCommand(this, "onPlay"));
	}
	else if (CAString::startWith(fname, "root.fadeout"))	//_onStateBeginFadeout(from);
	{
		CASprite* psprsStatic[] = 
		{
			null,
		};
		CASprite* psprsControls[] =
		{
			_prepare_ui_button_newgame(),
			null,
		};
		_setSpritesState(STATE_Fadeout, psprsStatic);
		_setSpritesState(STATE_Fadeout, psprsControls);
		//_button_music()->setState(stage()->isMusicMute() ? "off_fadeout" : "on_fadeout");
		//_button_sound()->setState(stage()->isSoundMute() ? "off_fadeout" : "on_fadeout");

		//_dist_last.setState(STATE_Fadeout);
		//_dist_max.setState(STATE_Fadeout);
		//_coin_last.setState(STATE_Fadeout);
		//_coin_max.setState(STATE_Fadeout);

		if (null != _pmenu)
		{
			_pmenu->release();
			_pmenu = null;
		}
	}
	else ;
};

void BZStagePlayLayerGamePrepare::onStateEnd(CAState* from, void* param) 
{
	const string& fname = from->getFullName();
	if (0) ;
	else if (CAString::startWith(fname, "root.idle"))		//_onStateEndIdle(from);
	{
	}
	else if (CAString::startWith(fname, "root.create"))		//_onStateEndFadein(from);
	{
	}
	else if (CAString::startWith(fname, "root.fadein"))		//_onStateEndFadein(from);
	{
		//_button_music()->setState(stage()->isMusicMute() ? "off" : "on");
		//_button_sound()->setState(stage()->isSoundMute() ? "off" : "on");
	}
	else if (CAString::startWith(fname, "root.running"))	//_onStateEndRunning(from);
	{
	}
	else if (CAString::startWith(fname, "root.onshop"))		//_onStateEndOnShop(from);
	{
	}
	else if (CAString::startWith(fname, "root.onplay"))		//_onStateEndOnPlay(from);
	{
	}
	else if (CAString::startWith(fname, "root.fadeout"))	//_onStateEndFadeout(from);
	{
	}
	else ;
};

void BZStagePlayLayerGamePrepare::show(bool s)
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

void BZStagePlayLayerGamePrepare::onEnter()
{
	GUARD_FUNCTION();

    //CCLabelTTF::labelWithString("Crazy Amber", "Arial", 16);

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

void BZStagePlayLayerGamePrepare::onUpdate() 
{
	CAStageLayer::onUpdate();
	if (_pmenu)
	{
		_pmenu->onUpdate();
	}
	if (this->getCurrentState()->getFullName() == "root.running")
	{
		/*
		char szNumber[32];
		sprintf(szNumber, "%dm", 0x1234);
		_dist_last.setText(szNumber);
		_dist_last.onUpdate();

		sprintf(szNumber, "%dm", 0x2234);
		_dist_max.setText(szNumber);
		_dist_max.onUpdate();

		sprintf(szNumber, "%dc", 0x2234);
		_coin_last.setText(szNumber);
		_coin_last.onUpdate();

		sprintf(szNumber, "%dc", 0x2234);
		_coin_max.setText(szNumber);
		_coin_max.onUpdate();
		*/
	}
};

void BZStagePlayLayerGamePrepare::onExit()
{
	CAStageLayer::onExit();
}

void BZStagePlayLayerGamePrepare::onEvent(CAEvent* pevt)
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
				if (name == "button_shop")
				{
					this->setConditionResult("root.running@user.shop", true);
				}
				else if (name == "prepare_ui_button_newgame")
				{
					this->setConditionResult("root.running@user.play", true);
				}
				/*
				else if (name == "button_music")
				{
					if (stage()->isMusicMute())
					{
						stage()->enableMusic(true);
						_button_music()->setState("on");
					}
					else
					{
						stage()->enableMusic(false);
						_button_music()->setState("off");
					}
				}
				else if (name == "button_sound")
				{
					if (stage()->isSoundMute())
					{
						stage()->enableSound(true);
						_button_sound()->setState("on");
					}
					else
					{
						stage()->enableSound(false);
						_button_sound()->setState("off");
					}
				}
				*/
			}
		}
		break;
	}
}

