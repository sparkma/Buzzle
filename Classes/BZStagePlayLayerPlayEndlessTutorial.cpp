
#include "BZStagePlayLayerPlayEndlessTutorial.h"
#include "BZStageCommon.h"
#include "BZStagePlayLayerDialog.h"
#include "BZGameClassic.h"
#include "BZSpriteButton.h"
#include "BZSpriteButtonItem.h"
#include "BZSpriteStarHolder.h"
#include "BZWorld.h"

#include "AString.h"
#include "AUserData.h"


/*
1 构造4-8个相同颜色泡泡放在屏幕右侧的4列位置，构造一个泡泡A在最左侧。
2 等待全部standing
2.5 然后显示hand&arrow,指示将A移动到指定区域。目的是让用户了解，泡泡可以拖动，并可以和相同颜色的泡泡融合。
3 等待A被拖动，检测到之后，隐藏hand&arrow, 并在右侧4列范围内松手。
4 等待A在右侧4列2行以上的位置松手。成功后，进入下一个状态。
5 等待全部Standing。
6 构造两个泡泡在最左侧，A,B A在下，B在上，B是包含心的。
7 等待全部standing
8 重复做2.5->5.
9 构造一个B,包含心的。重复上述2.5->5,这次会引发消除。将Resume按钮移动到屏幕中心，等待用户点击。
*/


BZStagePlayLayerPlayEndlessTutorial::BZStagePlayLayerPlayEndlessTutorial(CAStage* pstage, CAStageLayer* playerParent) 
: BZStageLayerCommon(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);

	_pgame = null;
}

BZStagePlayLayerPlayEndlessTutorial::~BZStagePlayLayerPlayEndlessTutorial(void)
{
	if (null != _pgame)
		_pgame->release();
	_pgame = null;
	_Trace("%s destroyed", __FUNCTION__);
}

string BZStagePlayLayerPlayEndlessTutorial::debuglog()
{
	return _debug_log;
}

void BZStagePlayLayerPlayEndlessTutorial::_onHome()
{
	this->replaceLayer("home");
	_pgame->release();
	_pgame = null;
}


void BZStagePlayLayerPlayEndlessTutorial::onStateBegin(CAState* from, void* param) 
{
	GUARD_FUNCTION();

	const string& fname = from->getFullName();
	if (0) ;
	else _HANDLE_STATE(running, 
	{
		postGameEvent("ui", "game_endless");
		BZSpriteButton* pbutton = (BZSpriteButton*)this->_getNamedSprite("game_button_audio", 0);
		if (pbutton) pbutton->setState(_pstage->isAudioMute() ? "off_game" : "on_game");
		BZStageLayerCommon::onStateBegin(from, param);
	})
	else
	{
		BZStageLayerCommon::onStateBegin(from, param);
	}
}

void BZStagePlayLayerPlayEndlessTutorial::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();

	string difficulty = CAWorld::sharedWorld()->gameenv().getString("difficulty");
	_Assert((difficulty == "easy") || (difficulty == "normal") || (difficulty == "hard"));

	if ("game_item_button_changecolor" == btn)
	{
		_pgame->setState(GS_ItemChangeColor);
		postGameEvent("prop_use", (difficulty + "-" + BUBBLE_PROP_CHANGECOLOR).c_str());
	}
	else if ("game_item_button_samecolor" == btn)
	{
		_pgame->setState(GS_ItemSameColorBooom);
		postGameEvent("prop_use", (difficulty + "-" + BUBBLE_PROP_SAMECOLOR).c_str());
	}
	else if ("game_item_button_bomb" == btn)
	{
		_pgame->setState(GS_ItemBooom);
		postGameEvent("prop_use", (difficulty + "-" + BUBBLE_PROP_BOOOM).c_str());
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

void BZStagePlayLayerPlayEndlessTutorial::_onRestart()
{
	//BZStageLayerCommon::_onRestart();
	int level = _pgame->getLevel();
	_pgame->release();
	_pgame = null;
	_initGame(level);
}

void BZStagePlayLayerPlayEndlessTutorial::_onFadein()
{
	BZStageLayerCommon::_onFadein();
}

void BZStagePlayLayerPlayEndlessTutorial::_onRunning()
{
	BZStageLayerCommon::_onRunning();
	_initGame(-1);
}

void BZStagePlayLayerPlayEndlessTutorial::_onFadeout()
{
	BZStageLayerCommon::_onFadeout();
}

void BZStagePlayLayerPlayEndlessTutorial::_initGame(int restartlevel)
{
	_Assert(null == _pgame);

	//CCPoint lb = _settings.getPoint("leftbottom");
	CCPoint lt = _settings.getPoint("boardlefttop");
	int rows = _settings.getInteger("rows");
	int cols = _settings.getInteger("cols");
	float bs = _settings.getFloat("bubblesize");
	
	//float zo = _settings.getFloat("zorder");
	
	//CAWorld::sharedWorld()->gameenv().setString("mode", mode);
	//CAWorld::sharedWorld()->gameenv().setString("difficulty", _difficulty);
	//CAWorld::sharedWorld()->gameenv().setBool("newgame", newgame);

	string mode = CAWorld::sharedWorld()->gameenv().getString("mode");
	_Assert(mode == "endless");
	string difficulty = CAWorld::sharedWorld()->gameenv().getString("difficulty");
	_Assert((difficulty == "easy") || (difficulty == "normal") || (difficulty == "hard"));
	
	//bool bNewGame = CAWorld::sharedWorld()->gameenv().getBool("newgame");
	//if (restart) bNewGame = false;

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

	int level;
	if (restartlevel > 0)
	{
		level = restartlevel;
	}
	else
	{
		level = CAWorld::sharedWorld()->gameenv().getInteger("levelfrom");
	}

	BZGameClassic* pgame = new BZGameClassic(this);
	_pgame = pgame;
	pgame->addEventListener(this);

	BZSpriteButtonItem* pb = null;
	BZSpriteButtonItem* ps = null;
	BZSpriteButtonItem* pc = null;
	pgame->initializeProps(difficulty, 
		pb = (BZSpriteButtonItem*)_getNamedSprite("game_item_button_bomb"), 
		ps = (BZSpriteButtonItem*)_getNamedSprite("game_item_button_samecolor"), 
		pc = (BZSpriteButtonItem*)_getNamedSprite("game_item_button_changecolor"),
		_settings.getInteger("endless_item_bomb_level_limit", 0),
		_settings.getInteger("endless_item_samecolor_level_limit", 10),
		_settings.getInteger("endless_item_changecolor_level_limit", 20)
		);
	pgame->initializeStarHolder(
		(BZSpriteStarHolder*)_getNamedSprite("game_star_holder-1"), 
		(BZSpriteStarHolder*)_getNamedSprite("game_star_holder-2"), 
		(BZSpriteStarHolder*)_getNamedSprite("game_star_holder-3"));

	float combo_rate = _settings.getFloat("combo_rate", 0.2f);
	pgame->initLevelParams(levels, bubble_score, level_base_score, level_mul_score, level_base_interval, level_mul_interval, score_dx, score_scale, combo_rate, level, lp[0], lp[1]);
	pgame->setLevel1Map(map);

	_pgame->createBoard(lt, rows, cols, bs);

	_pgame->onEnter();
}

void BZStagePlayLayerPlayEndlessTutorial::onUpdate() 
{
	BZStageLayerCommon::onUpdate();

	string fname = this->getCurrentState()->getLeafState()->getFullName();

	if (CAString::startWith(fname, "root.running"))
	{
		if (_pgame)
		{
			_pgame->onUpdate();
		}
	}
};

void BZStagePlayLayerPlayEndlessTutorial::onExit()
{
	if (null != _pgame)
	{
		_pgame->onExit();
	}
	BZStageLayerCommon::onExit();
}

void BZStagePlayLayerPlayEndlessTutorial::_onResume()
{
	GUARD_FUNCTION();

	BZStageLayerCommon::_onResume();

	//BZStagePlayLayerDialog* pdlg = (BZStagePlayLayerDialog*)_pstage->getSubLayer("endless_pause");
	//_Assert(pdlg);
	//const string& result = pdlg->getResult();
	const string& result = this->_dialogResult;

	if (result == "game_button_restart")
	{
	}
	else if (result == "game_button_resume")
	{
	}

	if (null != _pgame)
	{
		_pgame->onResume();
	}
}

bool BZStagePlayLayerPlayEndlessTutorial::onEvent(const CAEvent* pevt)
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
			if (KE_Back == pek->key() /*|| KE_Menu == pek->key() */ && _pgame)
			{
				if (_pgame->getState() == GS_Running)
				{
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
				}
				else if (pcmd->command() == "level_stars" && _pgame)
				{
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
REG_CLASS(BZStagePlayLayerPlayEndlessTutorial);
