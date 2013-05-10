
#include "BZStagePlayLayerPlayEndlessLogic.h"
#include "BZStageCommon.h"
#include "BZStagePlayLayerDialog.h"
#include "BZGameClassic.h"

#include "AWorld.h"
#include "AString.h"
#include "AUserData.h"

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

void BZStagePlayLayerPlayEndlessLogic::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	if ("game_item_button_changecolor" == btn)
	{
		_pgame->setState(GS_ItemChangeColor);
	}
	else if ("game_item_button_samecolor" == btn)
	{
		_pgame->setState(GS_ItemSameColorBooom);
	}
	else if ("game_item_button_bomb" == btn)
	{
		_pgame->setState(GS_ItemBooom);
	}
	BZStagePlayLayerPlayEndless::_onButtonCommand(pbutton);
}

void BZStagePlayLayerPlayEndlessLogic::_onRestart()
{
	BZStagePlayLayerPlayEndless::_onRestart();
	_pgame->release();
	_pgame = null;
	_initGame(true);
}

void BZStagePlayLayerPlayEndlessLogic::_onFadein()
{
	BZStagePlayLayerPlayEndless::_onFadein();

	string strScore = _settings.getString("score_spr");
	int nScoreItems = _settings.getInteger("score_items");
	string strLevel = _settings.getString("level_spr");
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
	nScoreItems = _findSprites(psprs, strScore.c_str(), 0, nScoreItems);
	_score = new BZGroupNumber(this, psprs, nScoreItems, true);
	_score->setChangeType(NCM_Near, NCO_Left);

	nLevelItems = _findSprites(psprs, strLevel.c_str(), 0, nLevelItems);
	_level = new BZGroupNumber(this, psprs, nLevelItems, true);
	_level->setChangeType(NCM_Near, NCO_Left);
}

void BZStagePlayLayerPlayEndlessLogic::_onRunning()
{
	BZStagePlayLayerPlayEndless::_onRunning();
	_initGame(false);
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

void BZStagePlayLayerPlayEndlessLogic::_initGame(bool restart)
{
	_Assert(null == _pgame);

	//CCPoint lb = _settings.getPoint("leftbottom");
	CCPoint lt = _settings.getPoint("boardlefttop");
	int rows = _settings.getInteger("rows");
	int cols = _settings.getInteger("cols");
	float bs = _settings.getFloat("bubblesize");
	
	//float zo = _settings.getFloat("zorder");
	
	//CAWorld::sharedWorld().gameenv().setString("mode", mode);
	//CAWorld::sharedWorld().gameenv().setString("difficulty", _difficulty);
	//CAWorld::sharedWorld().gameenv().setBool("newgame", newgame);

	string mode = CAWorld::sharedWorld().gameenv().getString("mode");
	_Assert(mode == "endless");
	string difficulty = CAWorld::sharedWorld().gameenv().getString("difficulty");
	_Assert((difficulty == "easy") || (difficulty == "normal") || (difficulty == "hard"));
	bool bNewGame = CAWorld::sharedWorld().gameenv().getBool("newgame");
	if (restart) bNewGame = false;

	int levels = _settings.getInteger("levels", 64);

	BZLevelParams lp[2];
	
	lp[0].fDelayOneRow = _settings.getFloat("level_min_DelayOneRow", 15);
#if defined(_DEBUG) && 0
	lp[0].fDelayOneRow = 10;
#endif
	lp[0].timeDelayBorn = _settings.getFloat("level_min_DelayBorn", 7);
	lp[0].nAvailableStars = _settings.getInteger("level_min_AvailableStars", 6);
	lp[0].nRangeBubbleBorn = _settings.getInteger("level_min_RangeBubbleBorn", 3);
	lp[0].fPrebornLines = _settings.getFloat("level_min_PrebornLines", rows * 0.4f);

	lp[1].fDelayOneRow = _settings.getFloat("level_max_DelayOneRow", 3);
	lp[1].timeDelayBorn = _settings.getFloat("level_max_DelayBorn", 2);
	lp[1].nAvailableStars = _settings.getInteger("level_max_AvailableStars", 4);
	lp[1].nRangeBubbleBorn = _settings.getInteger("level_max_RangeBubbleBorn", 7);
	lp[1].fPrebornLines = _settings.getFloat("level_max_PrebornLines", rows * 0.6f);

	int level_base_score = _settings.getInteger(("level_base_score_" + difficulty).c_str(), 3000);
	int level_mul_score = _settings.getInteger(("level_mul_score_" + difficulty).c_str(), 1200);
	float level_base_interval = _settings.getFloat(("level_base_interval_" + difficulty).c_str(), 1.8f);
	float level_mul_interval = _settings.getFloat(("level_mul_interval_" + difficulty).c_str(), 0.9f);
	int bubble_score = _settings.getInteger("bubble_score", 20);

	string map = _settings.getString("classic_level_1_map");

	float score_dx = _settings.getFloat("score_dx", 32.0f);
	float score_scale = _settings.getFloat("score_scale", 1.0f);

	int level = 1;
	if (bNewGame)
	{
		CAUserData::sharedUserData().setInteger((difficulty + "_level").c_str(), 1);
	}
	else //confinue
	{
		level = CAUserData::sharedUserData().getInteger((difficulty + "_level").c_str(), 1);
	}
	BZGameClassic* pgame = new BZGameClassic(this);
	_pgame = pgame;
	pgame->addEventListener(this);

	pgame->initLevelParams(levels, bubble_score, level_base_score, level_mul_score, level_base_interval, level_mul_interval, score_dx, score_scale, level, lp[0], lp[1]);
	pgame->setLevel1Map(map);

	_pgame->createBoard(lt, rows, cols, bs);

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

	string fname = this->getCurrentState()->getLeafState()->getFullName();
	if (_score && _level && _pgame)
	{
		//score and level has animation, we must update them here
		_updateScoreAndLevel();
	}
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
			CASprite* pspr = this->_getNamedSprite("game_level_progress_fill");
			if (null != pspr)
			{
				pspr->setVisible(true);
				pspr->setSclX(_pgame->getLevelPercent());
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

void BZStagePlayLayerPlayEndlessLogic::_onResume()
{
	BZStagePlayLayerPlayEndless::_onResume();
	if (null != _pgame)
	{
		_pgame->onResume();
	}
}

bool BZStagePlayLayerPlayEndlessLogic::onEvent(const CAEvent* pevt)
{
	bool ret = BZStagePlayLayerPlayEndless::onEvent(pevt);

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
			CAEventCommand* pcmd = (CAEventCommand*)pevt;
			if (pcmd->getSenderType() == ST_UserDefined)
			{
				if (pcmd->command() == "levelup" && _pgame)
				{
					int n = _pgame->getLevel();
					if (n > 1)
					{
						string difficulty = CAWorld::sharedWorld().gameenv().getString("difficulty");
						_Assert((difficulty == "easy") || (difficulty == "normal") || (difficulty == "hard"));
						CAUserData::sharedUserData().setInteger((difficulty + "_level").c_str(), n);

						//_pstage->playEffect("audios/levelup.ogg");
						this->showDialog("dialog_levelup", 30.0f);
					}
				}
				else if (pcmd->command() == "scoreup" && _pgame)
				{
				}
				else if (pcmd->command() == "leveluped" && _pgame)
				{
					//this->showDialog("dialog_levelup", 30.0f);
				}
			}
		}
		break;
	}
	return ret;
}


#include "AWorld.h"
REG_CLASS(BZStagePlayLayerPlayEndlessLogic);
