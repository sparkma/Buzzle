
#include "BZStagePlayLayerPlayEndlessLogic.h"
#include "BZStageCommon.h"
#include "BZStagePlayLayerDialog.h"
#include "BZGameClassic.h"

#include "AWorld.h"
#include "AString.h"

BZStagePlayLayerPlayEndlessLogic::BZStagePlayLayerPlayEndlessLogic(CAStage* pstage, CAStageLayer* playerParent) 
: BZStagePlayLayerPlayEndless(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);

	_nScore = -1;
	_nLevel = 0;
	_pgame = null;

	_pgame = null;
	_score = null;
	_level = null;
}

BZStagePlayLayerPlayEndlessLogic::~BZStagePlayLayerPlayEndlessLogic(void)
{
	if (null != _pgame)
		_pgame->release();
	_pgame = null;
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
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStagePlayLayerPlayEndlessLogic::_onHome()
{
	BZStagePlayLayerPlayEndless::_onHome();
	_pgame->release();
	_pgame = null;
}

void BZStagePlayLayerPlayEndlessLogic::_onSaveQuit()
{
	BZStagePlayLayerPlayEndless::_onSaveQuit();
	_pgame->saveData();
	_pgame->release();
	_pgame = null;
}

void BZStagePlayLayerPlayEndlessLogic::_onRestart()
{
	BZStagePlayLayerPlayEndless::_onRestart();
	_pgame->release();
	_pgame = null;
	_initGame();
}

void BZStagePlayLayerPlayEndlessLogic::_onFadein()
{
	BZStagePlayLayerPlayEndless::_onFadein();

	string strScore = _settings.getString("score_spr") + "-%d";
	int nScoreItems = _settings.getInteger("score_items");
	string strLevel = _settings.getString("level_spr") + "-%d";
	int nLevelItems = _settings.getInteger("level_items");

	CASprite* psprs[16];

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
	nScoreItems = _findSprites(psprs, strScore.c_str(), 1, nScoreItems);
	_score = new BZGroupNumber(this, psprs, nScoreItems);
	_score->setChangeType(NCM_Near, NCO_Left);

	nLevelItems = _findSprites(psprs, strLevel.c_str(), 1, nLevelItems);
	_level = new BZGroupNumber(this, psprs, nLevelItems);
	_level->setChangeType(NCM_Near, NCO_Left);
}

void BZStagePlayLayerPlayEndlessLogic::_onRunning()
{
	BZStagePlayLayerPlayEndless::_onRunning();
	_initGame();
}

void BZStagePlayLayerPlayEndlessLogic::_onFadeout()
{
	BZStagePlayLayerPlayEndless::_onFadeout();
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

void BZStagePlayLayerPlayEndlessLogic::_initGame()
{
	_Assert(null == _pgame);

	CCPoint lb = _settings.getPoint("leftbottom");
	int rows = _settings.getInteger("rows");
	int cols = _settings.getInteger("cols");
	float bs = _settings.getFloat("bubblesize");
	float zo = _settings.getFloat("zorder");
	
	string how = CAWorld::sharedWorld().gameenv().getString("how");

	int levels = _settings.getInteger("levels", 128);

	BZLevelParams lp[2];
	
	lp[0].fDelayOneRow = _settings.getFloat("level_min_DelayOneRow", 15);
	lp[0].timeDelayBorn = _settings.getFloat("level_min_DelayBorn", 4);
	lp[0].fPercentStarBorn = _settings.getFloat("level_min_PercentStarBorn", 45);
	lp[0].nMinStarsInOneBubbleType = _settings.getInteger("level_min_MinStarsInOneBubbleType", 4);
	lp[0].nRangeBubbleBorn = _settings.getInteger("level_min_RangeBubbleBorn", 3);

	lp[1].fDelayOneRow = _settings.getFloat("level_max_DelayOneRow", 3);
	lp[1].fPercentStarBorn = _settings.getFloat("level_max_PercentStarBorn", 35);
	lp[1].nMinStarsInOneBubbleType = _settings.getInteger("level_max_MinStarsInOneBubbleType", 2);
	lp[1].nRangeBubbleBorn = _settings.getInteger("level_max_RangeBubbleBorn", 7);

	int level_base_score = _settings.getInteger("level_base_score", 3000);
	int level_mul_score = _settings.getInteger("level_mul_score", 1200);
	int bubble_score = _settings.getInteger("bubble_score", 20);

	string map = _settings.getString("classic_level_1_map");

	BZGameClassic* pgame = new BZGameClassic(this);
	pgame->initLevelParams(levels, bubble_score, level_base_score, level_mul_score, lp[0], lp[1]);
	pgame->setLevel1Map(map);

	_pgame = pgame;

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

void BZStagePlayLayerPlayEndlessLogic::_updateScoreAndLevel()
{
	char sz[18];
	
	int n = _pgame->getScore();
	if (n != _nScore)
	{
		sprintf(sz, "%d", n);
		_score->setText(sz);
		_nScore = n;
		_Info("score has changed to %d", n);
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

void BZStagePlayLayerPlayEndlessLogic::onUpdate() 
{
	BZStagePlayLayerPlayEndless::onUpdate();

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
			if (_pgame->isGameOver())
			{
				_onGameOver();
				return;
			}
		}
	}
};

void BZStagePlayLayerPlayEndlessLogic::onExit()
{
	if (null != _pgame)
	{
		_pgame->onExit();
	}
	BZStagePlayLayerPlayEndless::onExit();
}

void BZStagePlayLayerPlayEndlessLogic::onEvent(const CAEvent* pevt)
{
	BZStagePlayLayerPlayEndless::onEvent(pevt);

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
			/*
			CAEventCommand* pec = (CAEventCommand*)pevt;
			if (pec->command() == EVENT_ONRESUME)
			{
				this->setConditionResult("root.pause@user.resume", true);
			}
			else if (pec->command() == EVENT_ONRESTART)
			{
				this->setConditionResult("root.pause@user.restart", true);
			}
			else if (pec->command() == EVENT_ONQUIT)
			{
				this->setConditionResult("root.pause@user.quit", true);
			}
			else if (pec->command() == EVENT_ONSAVE_QUIT)
			{
				this->setConditionResult("root.pause@user.save_quit", true);
			}
			*/
		}
		break;
	}
}


#include "AWorld.h"
REG_CLASS(BZStagePlayLayerPlayEndlessLogic);
