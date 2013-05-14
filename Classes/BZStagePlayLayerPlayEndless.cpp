
#include "BZStagePlayLayerPlayEndless.h"
#include "BZStageCommon.h"
#include "BZStagePlayLayerDialog.h"
#include "BZGameClassic.h"
#include "BZSpriteButton.h"
#include "BZSpriteButtonItem.h"

#include "AWorld.h"
#include "AString.h"
#include "AUserData.h"

BZStagePlayLayerPlayEndless::BZStagePlayLayerPlayEndless(CAStage* pstage, CAStageLayer* playerParent) 
: BZStageLayerCommon(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);

	_nScore = -1;
	_nLevel = 0;
	_pgame = null;

	_pgame = null;
	_score = null;
	_level = null;

	_oldLevelPercent = 0;
}

BZStagePlayLayerPlayEndless::~BZStagePlayLayerPlayEndless(void)
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

string BZStagePlayLayerPlayEndless::debuglog()
{
	return _debug_log;
}

void BZStagePlayLayerPlayEndless::_onHome()
{
	this->replaceLayer("home");
	_pgame->release();
	_pgame = null;
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
	else if ("game_button_pause" == btn)
	{
		if (_pgame->getState() == GS_Running)
		{
			_pgame->setState(GS_GamePaused);
			this->showDialog("dialog_paused", _pgame->getBaseZOrder() + VZ_DIALOG_PAUSE);
		}
	}
	else if ("game_button_audio" == btn)
	{
		bool mute = _pstage->isAudioMute();
		pbutton->setState(mute ? "on_game" : "off_game");
		_pstage->enableAudio(mute);
	}
}

void BZStagePlayLayerPlayEndless::_onRestart()
{
	//BZStageLayerCommon::_onRestart();
	_pgame->release();
	_pgame = null;
	_initGame(true);
}

void BZStagePlayLayerPlayEndless::_onFadein()
{
	BZStageLayerCommon::_onFadein();

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

void BZStagePlayLayerPlayEndless::_onRunning()
{
	BZStageLayerCommon::_onRunning();
	_initGame(false);
}

void BZStagePlayLayerPlayEndless::_onFadeout()
{
	BZStageLayerCommon::_onFadeout();
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

void BZStagePlayLayerPlayEndless::_initGame(bool restart)
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

	BZSpriteButtonItem* pb = null;
	BZSpriteButtonItem* ps = null;
	BZSpriteButtonItem* pc = null;
	pgame->initializeProp(difficulty, 
		pb = (BZSpriteButtonItem*)_getNamedSprite("game_item_button_bomb"), 
		ps = (BZSpriteButtonItem*)_getNamedSprite("game_item_button_samecolor"), 
		pc = (BZSpriteButtonItem*)_getNamedSprite("game_item_button_changecolor"),
		_settings.getInteger("endless_item_bomb_level_limit", 0),
		_settings.getInteger("endless_item_samecolor_level_limit", 10),
		_settings.getInteger("endless_item_changecolor_level_limit", 20)
		);

	float combo_rate = _settings.getFloat("combo_rate", 0.2f);
	pgame->initLevelParams(levels, bubble_score, level_base_score, level_mul_score, level_base_interval, level_mul_interval, score_dx, score_scale, combo_rate, level, lp[0], lp[1]);
	pgame->setLevel1Map(map);

	_pgame->createBoard(lt, rows, cols, bs);

	_pgame->onEnter();
}

void BZStagePlayLayerPlayEndless::_updateScoreAndLevel()
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

void BZStagePlayLayerPlayEndless::onUpdate() 
{
	BZStageLayerCommon::onUpdate();

	string fname = this->getCurrentState()->getLeafState()->getFullName();
	if (_score && _level && _pgame)
	{
		//score and level has animation, we must update them here
		_updateScoreAndLevel();

		_debug_log = _pgame->debuglog();
	}

	if (CAString::startWith(fname, "root.running"))
	{
		if (_pgame)
		{
			_pgame->onUpdate();
			if (_pgame->isGameOver())
			{
				_pgame->setState(GS_GameOver);
				this->showDialog("dialog_gameover", _pgame->getBaseZOrder() + VZ_DIALOG_GAMEOVER);
				return;
			}
			CASprite* pspr = this->_getNamedSprite("game_level_progress_fill");
			if (null != pspr)
			{
				pspr->setVisible(true);
				float percent = _pgame->getLevelPercent();
				float diff = percent - _oldLevelPercent;
				float used = 0;
				{
					float d = CAUtils::Absf(diff);
					if (d < 0.01f)
					{
						used = percent;
					}
					else
					{
						used = _oldLevelPercent + diff * 1.0f / 5.0f;
					}
					_oldLevelPercent = used;
					pspr->setSclX(used);
				}
			}
		}
	}
};

void BZStagePlayLayerPlayEndless::onExit()
{
	if (null != _pgame)
	{
		_pgame->onExit();
	}
	BZStageLayerCommon::onExit();
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

	if (null != _pgame)
	{
		_pgame->onResume();
	}
}

bool BZStagePlayLayerPlayEndless::onEvent(const CAEvent* pevt)
{
	bool ret = BZStageLayerCommon::onEvent(pevt);

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
			if (KE_Back == pek->key() || KE_Menu == pek->key() && _pgame)
			{
				if (_pgame->getState() == GS_Running)
				{
					_pgame->setState(GS_GamePaused);
					this->showDialog("dialog_paused", _pgame->getBaseZOrder() + VZ_DIALOG_PAUSE);
				}
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

						this->showDialog("dialog_levelup", _pgame->getBaseZOrder() + VZ_DIALOG_LEVELUP);
					}
				}
				else if (pcmd->command() == "scoreup" && _pgame)
				{
				}
				else if (pcmd->command() == "leveluped" && _pgame)
				{

				}
			}
		}
		break;
	}
	return ret;
}

#include "AWorld.h"
REG_CLASS(BZStagePlayLayerPlayEndless);
