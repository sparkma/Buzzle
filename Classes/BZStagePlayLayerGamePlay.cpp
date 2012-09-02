
#include "BZStagePlayLayerGamePlay.h"
#include "BZStagePlayLayerGamePlayPause.h"
#include "AWorld.h"
#include "AStage.h"
#include "AString.h"
#include "BZSpriteCommon.h"
#include "BZSpriteButton.h"

#include "BZGameClassic.h"
#include "BZGameTapBoom.h"

BZStagePlayLayerGamePlay::BZStagePlayLayerGamePlay(CAStage* pstage, CAStageLayer* playerParent) : CAStageLayer(pstage, playerParent)
{
	GUARD_FUNCTION();

	_Trace("%s allocated", __FUNCTION__);
	_nScore = -1;
	_nLevel = 0;
	_pgame = null;

	_pgame = null;
	_score = null;
	_level = null;

	_NullGetters();
}

BZStagePlayLayerGamePlay::~BZStagePlayLayerGamePlay(void)
{
	if (null != _pgame)
		_pgame->release();
	_pgame = null;
	_Trace("%s destroyed", __FUNCTION__);
}

string BZStagePlayLayerGamePlay::debuglog() 
{ 
	char sz[256];
	sprintf(sz, "[%.2f%%], sprs=%d, state=%s\n%s", 
		_pgame ? _pgame->getLevelPercent() * 100.0f : 100.0f,
		this->_getNamedSpritesCount(), 
		this->getCurrentState()->getLeafState()->getFullName().c_str(),
		_pgame ? _pgame->debuglog().c_str() : "");
	return sz;
}

bool BZStagePlayLayerGamePlay::checkCondition(CAState* from, const CATransition& trans)
{
	//root.idle:	inner condition
	//root.running:	button event
	if (CAMachine::checkCondition(from, trans))
		return true;

	bool result = false;
	float timeout = trans.timeout;
	if (timeout > 0 && this->getTimeNow() - from->getTimeEnter() > timeout)
	{
		return true;
	}

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
		if (result)
		{
			string result = "";
			result += from->getFullName();
			result += "@";
			result += trans.condition;
			this->setConditionResult(result.c_str(), true);
		}
	}
	else if (fname == "root.diving")
	{
	}
	else if (fname == "root.resume")
	{
		CAStageLayer* pl = this->_getSubLayer("game.play.pause");
		_Assert(pl);
		_Assert(trans.condition == "pause.isidle");
		if (pl->getCurrentState()->getFullName() == "root.idle")
		{
			return true;
		}
	}

	if (CAMachine::checkCondition(from, trans))
		return true;

	return false;
}

void BZStagePlayLayerGamePlay::onStateBegin(CAState* from, void* param) 
{
	const string& fname = from->getFullName();
	_state = fname;

	if (0) ;
	else if (CAString::startWith(fname, "root.idle"))		//_onStateBeginIdle(from);
	{
		//this->setIsVisible(false);
		_NullGetters();
		removeAllTimelines();
	}
	else if (CAString::startWith(fname, "root.create"))		//_onStateBeginFadein(from);
	{
		//this->stage()->resetTimer();
		//this->stage()->setOffset(ccp(0, 0), 0);
		activeAllTimelines();
	}
	else if (CAString::startWith(fname, "root.fadein"))		//_onStateBeginFadein(from);
	{
		_InitGetters();

		string strScore = _settings.getString("score_spr");
		float fScoreScale = _settings.getFloat("score_scale");
		CCPoint ptScore = _settings.getPoint("score_pos");
		CAWorld::percent2view(ptScore);
		CCSize sizeScore = _settings.getSize("score_size");
		CAWorld::percent2view(sizeScore);

		string strLevel = _settings.getString("level_spr");
		float fLevelScale = _settings.getFloat("level_scale");
		CCPoint ptLevel = _settings.getPoint("level_pos");
		CAWorld::percent2view(ptLevel);
		CCSize sizeLevel = _settings.getSize("level_size");
		CAWorld::percent2view(sizeLevel);

		_score = new BZGroupNumber(this, strScore);
		_score->setLayout(+1, ptScore, sizeScore, fScoreScale);
		_score->setChangeType(NCM_Near, NCO_Left);

		_level = new BZGroupNumber(this, strLevel);
		_level->setLayout(+1, ptLevel, sizeLevel, fLevelScale);
		_level->setChangeType(NCM_Near, NCO_Left);

		_pstage->setFocus(this);
	}
	else if (CAString::startWith(fname, "root.levelup"))	
	{
		//stage()->setOffset(CCPointZero, 0);
	}
	else if (CAString::startWith(fname, "root.running"))	//_onStateBeginRunning(from);
	{
		_initGame();
	}
	else if (fname == "root.pause")
	{
		CAStageLayer* pl = this->_getSubLayer("game.play.pause");
		_Assert(pl);
		pl->setConditionResult("root.idle@user.show", true);
		_pstage->setFocus(pl);
		pause();
	}
	else if (CAString::startWith(fname, "root.quit"))
	{
		//wait plPause's state in idle
		//_pstage->setFocus(this);
		resume();
		if (null != _pgame)
		{
			_pgame->release();
			_pgame = null;
		}
		//_initGame();
		this->_playerParent->onEvent(new CAEventCommand(this, "play.finished"));
	}
	else if (CAString::startWith(fname, "root.resume"))
	{
		//wait plPause's state in idle
		_pstage->setFocus(this);
		resume();
	}
	else if (CAString::startWith(fname, "root.save_quit"))
	{
		resume();
		if (null != _pgame)
		{
			_pgame->saveData();
			_pgame->release();
			_pgame = null;
		}
		//_initGame();
		this->_playerParent->onEvent(new CAEventCommand(this, "play.finished"));
	}
	else if (CAString::startWith(fname, "root.over"))	
	{
		//show game over message
		//save record here
	}
	else if (CAString::startWith(fname, "root.fadeout"))
	{
		//show fadeout effects
		//transite to idle
		if (_score)
		{
			_score->release();
			_score = null;
		}
		if (_level)
		{
			_level->release();
			_level = null;
		}
	}
	else ;
};

void BZStagePlayLayerGamePlay::onStateEnd(CAState* from, void* param) 
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
	else if (CAString::startWith(fname, "root.pause"))	
	{
	}
	else if (CAString::startWith(fname, "root.quit"))	
	{
	}
	else if (CAString::startWith(fname, "root.save_quit"))	
	{
	}
	else if (CAString::startWith(fname, "root.resume"))
	{
	}
	else if (CAString::startWith(fname, "root.over"))	
	{
	}
	else if (CAString::startWith(fname, "root.fadeout"))	
	{
	}
	else ;
};

void BZStagePlayLayerGamePlay::pause(bool bsub)
{
	showTimeline("play_ui_buttons", false);
	CAStageLayer::pause(bsub);
}

void BZStagePlayLayerGamePlay::resume(bool bsub)
{
	CAStageLayer::resume(bsub);
	showTimeline("play_ui_buttons", true);
	_play_ui_button_pause()->setState(STATE_Stand);
}

void BZStagePlayLayerGamePlay::onEnter()
{
	GUARD_FUNCTION();

	CAStageLayer::onEnter();
}

void BZStagePlayLayerGamePlay::_initGame()
{
	_Assert(null == _pgame);

	CCPoint lb = _settings.getPoint("leftbottom");
	int rows = _settings.getInteger("rows");
	int cols = _settings.getInteger("cols");
	float bs = _settings.getFloat("bubblesize");
	float zo = _settings.getFloat("zorder");
	
	string mode = CAWorld::sharedWorld().gameenv().getString("mode");
	string how = CAWorld::sharedWorld().gameenv().getString("how");
	if (mode == "classic" || mode.length() <= 0)
	{
		int levels = _settings.getInteger("levels", 128);

		BZLevelParams lp[2];
		
		lp[0].timeDelayBorn = _settings.getFloat("level_min_DelayBorn", 4);
		lp[0].fPercentStarBorn = _settings.getFloat("level_min_PercentStarBorn", 45);
		lp[0].fMinPercentStar = _settings.getFloat("level_min_MinPercentStar", 20);
		lp[0].nRangeBubbleBorn = _settings.getInteger("level_min_RangeBubbleBorn", 3);

		lp[1].timeDelayBorn = _settings.getFloat("level_max_DelayBorn", 0.5);
		lp[1].fPercentStarBorn = _settings.getFloat("level_max_PercentStarBorn", 35);
		lp[1].fMinPercentStar = _settings.getFloat("level_max_MinPercentStar", 10);
		lp[1].nRangeBubbleBorn = _settings.getInteger("level_max_RangeBubbleBorn", 7);

		int level_base_score = _settings.getInteger("level_base_score", 3000);
		int level_mul_score = _settings.getInteger("level_mul_score", 1200);
		int bubble_score = _settings.getInteger("bubble_score", 20);

		string map = _settings.getString("classic_level_1_map");

		BZGameClassic* pgame = new BZGameClassic(this);
		pgame->initLevelParams(levels, bubble_score, level_base_score, level_mul_score, lp[0], lp[1]);
		pgame->setLevel1Map(map);

		_pgame = pgame;
	}
	else if (mode == "tapboom")
	{
		BZGameTapBoom* pgame = new BZGameTapBoom(this);
		_pgame = pgame;
	}
	else
	{
		_pgame = null;
		_Assert(false);
	}

	_Assert(_pgame);
	_pgame->createBoard(lb, rows, cols, bs, zo);
	if (how == "newgame" || how.length() <= 0)
	{
	}
	else if (how == "continue")
	{
		_pgame->loadData();
	}
	else
	{
		_Assert(false);
	}

	_pgame->onEnter();
}

void BZStagePlayLayerGamePlay::_updateScoreAndLevel()
{
	char sz[18];
	
	int n = _pgame->getScore();
	if (n != _nScore)
	{
		sprintf(sz, "%d", n);
		_score->setText(sz);
	}
	_score->onUpdate();

	n = _pgame->getLevel();
	if (n != _nLevel)
	{
		sprintf(sz, "%d", n);
		_level->setText(sz);
	}
	_level->onUpdate();
}

void BZStagePlayLayerGamePlay::onUpdate() 
{
	CAStageLayer::onUpdate();

	if (_score && _level && _pgame)
	{
		_updateScoreAndLevel();
	}

	string fname = this->getCurrentState()->getLeafState()->getFullName();
	if (CAString::startWith(fname, "root.running"))
	{
		if (_pgame)
		{
			_pgame->onUpdate();
		}
	}
};

void BZStagePlayLayerGamePlay::onExit()
{
	if (null != _pgame)
	{
		_pgame->onExit();
	}
	CAStageLayer::onExit();
}

void BZStagePlayLayerGamePlay::onEvent(CAEvent* pevt)
{
	CAStageLayer::onEvent(pevt);

	switch (pevt->type())
	{
	case ET_Touch:
		{
			if (null != _pgame)
			{
				_pgame->onEvent(pevt);
			}
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
	case ET_Key:
		{
			CAEventKey* pek = (CAEventKey*)pevt;
			if (KE_Back == pek->key() || KE_Menu == pek->key())
			{
				this->setConditionResult("root.running@user.pause", true);
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
				if (name == "play_ui_button_pause")
				{
					this->setConditionResult("root.running@user.pause", true);
				}
			}
			else if (pec->command() == EVENT_ONRESUME)
			{
				this->setConditionResult("root.pause@user.resume", true);
			}
			else if (pec->command() == EVENT_ONQUIT)
			{
				this->setConditionResult("root.pause@user.quit", true);
			}
			else if (pec->command() == EVENT_ONSAVE_QUIT)
			{
				this->setConditionResult("root.pause@user.save_quit", true);
			}
		}
		break;
	}
}

