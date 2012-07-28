
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
}

BZStagePlayLayerGamePlayPause::~BZStagePlayLayerGamePlayPause(void)
{
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
		float timeout = trans.timeout;
		if (timeout > 0.00001f && this->getTimeNow() - from->getTimeEnter() > timeout)
		{
			result = true;
		}
		else
		{
			//check 3 controls pose
			CASprite* psprs[] = 
			{
				_button_restart(),
				_button_resume(),
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

void BZStagePlayLayerGamePlayPause::onStateBegin(CAState* from, void* param) 
{
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

		CASprite* pspr;
		pspr = _button_music();
		_Assert(pspr);
		pspr = _button_music();
		pspr->setState(stage()->isMusicMute() ? "off_fadein" : "on_fadein");
		pspr = _button_sound();
		_Assert(pspr);
		pspr->setState(stage()->isSoundMute() ? "off_fadein" : "on_fadein");
		CASprite* psprsControls[] =
		{
			//_label_pause_title(),
			_panel_black(),
			_button_restart(),
			_button_resume(),
			null,
		};
		_setSpritesState(STATE_Fadein, psprsControls);
	}
	else if (CAString::startWith(fname, "root.running"))	//_onStateBeginRunning(from);
	{
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
	else if (CAString::startWith(fname, "root.fadeout"))	//_onStateBeginFadeout(from);
	{
		CASprite* pspr;
		pspr = _button_music();
		_Assert(pspr);
		pspr = _button_music();
		pspr->setState(stage()->isMusicMute() ? "off_fadeout" : "on_fadeout");
		pspr = _button_sound();
		_Assert(pspr);
		pspr->setState(stage()->isSoundMute() ? "off_fadeout" : "on_fadeout");
		CASprite* psprsControls[] =
		{
			//_label_pause_title(),
			_panel_black(),
			_button_restart(),
			_button_resume(),
			null,
		};
		_setSpritesState(STATE_Fadeout, psprsControls);
	}
	else ;
};

void BZStagePlayLayerGamePlayPause::onStateEnd(CAState* from, void* param) 
{
	const string& fname = from->getFullName();
	if (0) ;
	else if (CAString::startWith(fname, "root.idle"))		//_onStateEndIdle(from);
	{
	}
	else if (CAString::startWith(fname, "root.fadein"))		//_onStateEndFadein(from);
	{
	}
	else if (CAString::startWith(fname, "root.running"))	//_onStateEndRunning(from);
	{
	}
	else if (CAString::startWith(fname, "root.onresume"))	//_onStateEndOnResume(from);
	{
	}
	else if (CAString::startWith(fname, "root.onrestart"))	//_onStateEndOnRestart(from);
	{
	}
	else if (CAString::startWith(fname, "root.fadeout"))	//_onStateEndFadeout(from);
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

void BZStagePlayLayerGamePlayPause::_updateNumber(const char* prefix, int nValue)
{
	if (nValue < 0)
		return;

	unsigned int i, count;

	int len = strlen(prefix);
	count = _getNamedSpritesCount(prefix);
	if (count > 0)
	{
		_Assert(count == 6);
		char szFmt[32];
		sprintf(szFmt, "%%%02dd", count);
		char szScore[32];
		sprintf(szScore, szFmt, nValue);
		for (i = 0; i < count; i++)
		{
			CASprite* pspr = _getNamedSprite(prefix, i);
			string gname = pspr->getGroupName();
			strings items;
			CAString::split(gname, "-", items);
			const char* szIndex = items[items.size() - 1].c_str();
			if (*szIndex >= '0' && *szIndex <= '9')
			{
			}
			else
			{
				continue;
			}
			int index = atoi(szIndex);
			char szPose[16];
			strcpy(szPose, pspr->getCurrentPose()->name().c_str());

			int need = szScore[index] - '0';
			int now = szPose[0] - '0';
			
			int should = _goNear(now, need, 10, 1);
			szPose[0] = '0' + should;
			_Assert(szPose[0] >= '0' && szPose[0] <= '9');
			pspr->switchPose(szPose);
		}
	}
}

void BZStagePlayLayerGamePlayPause::onUpdate() 
{
	CAStageLayer::onUpdate();
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
				if (name == "button_restart")
				{
					this->setConditionResult("root.running@user.restart", true);
				}
				else if (name == "button_resume")
				{
					this->setConditionResult("root.running@user.resume", true);
				}
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
			}
		}
		break;
	}
}

