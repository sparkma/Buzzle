#include "BZWorld.h"

#if 0
#include "BZStageTest.h"
#	include "BZStageTestLayerTest.h"
#		include "BZStageTestLayerTestBubbles.h"
#endif

#include "BZStageLogo.h"
#	include "BZStageLogoLayerMain.h"

//#include "BZStageMenu.h"
//#	include "BZStageMenuLayerMain.h"

#include "BZStagePlay.h"
#	include "BZStagePlayLayerGame.h"
#	include "BZStagePlayLayerGamePrepare.h"
#	include "BZStagePlayLayerGameShop.h"
#	include "BZStagePlayLayerGamePlay.h"
#	include "BZStagePlayLayerGamePlayPause.h"
//#		include "BZStagePlayLayerGameBackground.h"
//#		include "BZStagePlayLayerGameDoodads1.h"
//#		include "BZStagePlayLayerGamePlay.h"
//#			include "BZStagePlayLayerGamePlayDialog.h"
//#		include "BZStagePlayLayerGameDoodads2.h"

//#include "BZStageHelp.h"
//#	include "BZStageHelpLayerMain.h"

//#include "BZStageOptions.h"
//#	include "BZStageOptionsLayerMain.h"

//#include "BZStageCredits.h"
//#	include "BZStageCreditsLayerMain.h"

//lay on the road
#	include "BZStagePlayLayerCommon.h"

#include "BZSpriteCommon.h"
#include "BZSpriteButton.h"

BZWorld::BZWorld(void)
{
}

BZWorld::~BZWorld(void)
{
}

static const char* _stage_play			= "game/game";
static		const char* _layer_game		= "game";
static			const char* _layer_game_prepare	= "game.prepare";
static			const char* _layer_game_shop	= "game.shop";
static			const char* _layer_game_play	= "game.play";
static			const char* _layer_game_play_pause	= "game.play.pause";

static const char* _stage_test			= "test";
static		const char* _layer_test		= "test";
static			const char* _layer_test_bubbles = "test.bubbles";

static const char* _stage_logo			= "logo/logo";
static		const char* _layer_logo		= "logo";

//static const char* _stage_menu			= "menu";
//static		const char* _layer_menu		= "menu";

//static const char* _stage_help			= "help";
//static		const char* _layer_help		= "help";

//static const char* _stage_options		= "options";
//static		const char* _layer_options	= "options";

//static const char* _stage_credits		= "credits";
//static		const char* _layer_credits	= "credits";

CAStage* BZWorld::createStage(const char* _name)
{
	GUARD_FUNCTION();

	string name = _name;
	CAStage* ps = NULL;
	if (name == _stage_play)
	{
		GUARD_FIELD(stage_play);
		ps = new BZStagePlay(_name);
	}
	else if (name == _stage_logo)
	{
		GUARD_FIELD(stage_logo);
		ps = new BZStageLogo(_name);
	}
	/*
	else if (name == _stage_menu)
	{
		GUARD_FIELD(stage_menu);
		ps = new BZStageMenu(_name);
	}
	else if (name == _stage_help)
	{
		GUARD_FIELD(stage_help);
		ps = new BZStageHelp(_name);
	}
	else if (name == _stage_options)
	{
		GUARD_FIELD(stage_options);
		ps = new BZStageOptions(_name);
	}
	else if (name == _stage_credits)
	{
		GUARD_FIELD(stage_credits);
		ps = new BZStageCredits(_name);
	}
	else if (name == _stage_test)
	{
		GUARD_FIELD(stage_test);
		ps = new BZStageTest(_name);
	}
	*/

	return ps;
}

CAStageLayer* BZWorld::createLayer(CAStage* pstage, CAStageLayer* playerParent, const char* _name)
{
	GUARD_FUNCTION();

	string name = "";
	
	if (playerParent)
	{
	//	name += playerParent->name();
	}
	name += _name;

	if (pstage->name() == _stage_play)
	{
		if (name == _layer_game)
		{
			GUARD_FIELD(layer_game);
			return new BZStagePlayLayerGame(pstage, playerParent);
		}
		else if (name == _layer_game_prepare)
		{
			GUARD_FIELD(layer_game_prepare);
			return new BZStagePlayLayerGamePrepare(pstage, playerParent);
		}
		else if (name == _layer_game_play)
		{
			GUARD_FIELD(layer_game_play);
			return new BZStagePlayLayerGamePlay(pstage, playerParent);
		}
		else if (name == _layer_game_shop)
		{
			GUARD_FIELD(layer_game_shop);
			return new BZStagePlayLayerGameShop(pstage, playerParent);
		}
		else if (name == _layer_game_play_pause)
		{
			GUARD_FIELD(layer_game_play_pause);
			return new BZStagePlayLayerGamePlayPause(pstage, playerParent);
		}
		/*
		else if (name == _layer_game_background) 
		{
			GUARD_FIELD(layer_back);
			return new BZStagePlayLayerGameBackground(pstage, playerParent);
		}
		else if (name == _layer_game_doodads1)
		{
			GUARD_FIELD(layer_doodads1);
			return new BZStagePlayLayerGameDoodads1(pstage, playerParent);
		}
		else if (name == _layer_game_play_dialog)
		{
			GUARD_FIELD(layer_game_play_dialog);
			return new BZStagePlayLayerGamePlayDialog(pstage, playerParent);
		}
		else if (name == _layer_game_doodads2) 
		{
			GUARD_FIELD(layer_doodads2);
			return new BZStagePlayLayerGameDoodads2(pstage, playerParent);
		}
		*/
		else
		{
			_Assert(false);
		}
	}
	else if (pstage->name() == _stage_logo)
	{
		if (name == _layer_logo)
		{
			GUARD_FIELD(layer_logo);
			return new BZStageLogoLayerMain(pstage, playerParent);
		}
	}
	/*
	else if (pstage->name() == _stage_menu)
	{
		if (name == _layer_menu)
		{
			GUARD_FIELD(layer_menu);
			return new BZStageMenuLayerMain(pstage, playerParent);
		}
	}
	else if (pstage->name() == _stage_help)
	{
		if (name == _layer_help)
		{
			GUARD_FIELD(layer_help);
			return new BZStageHelpLayerMain(pstage, playerParent);
		}
	}
	else if (pstage->name() == _stage_options)
	{
		if (name == _layer_options)
		{
			GUARD_FIELD(layer_options);
			return new BZStageOptionsLayerMain(pstage, playerParent);
		}
	}
	else if (pstage->name() == _stage_credits)
	{
		if (name == _layer_credits)
		{
			GUARD_FIELD(layer_credits);
			return new BZStageCreditsLayerMain(pstage, playerParent);
		}
	}
	else if (pstage->name() == _stage_test)
	{
		if (name == _layer_test)
		{
			GUARD_FIELD(layer_test);
			return new BZStageTestLayerTest(pstage, playerParent);
		}
		else if (name == _layer_test_bubbles)
		{
			GUARD_FIELD(layer_test_bubbles);
			return new BZStageTestLayerTestBubbles(pstage, playerParent);
		}
	}
	*/
	_Assert(false);

	return NULL;
}

/* 
这里生成的 sprite 是全局的，可能任何Stage， 任何Layer都会使用。
这里不对他们的坐标等具象化东西进行初始化。
*/
CASprite* BZWorld::createSprite(CAStageLayer* player, const char* _name)
{
	GUARD_FUNCTION();

	CASprite* pspr = null;
	string name = _name;
	//if (player->stage()->name() == _stage_play)
	{
		//if (player->name() == _layer_game)
		{
			if (name.find("button") == ((size_t)-1))
			{
				GUARD_FIELD(common_sprite);
				pspr = new BZSpriteCommon(player, name.c_str()); 
			}
			else
			{
				GUARD_FIELD(button);
				pspr = new BZSpriteButton(player, name.c_str()); 
			}
		}
	}
	
	//_Trace("sprite [%s] created, p=%p", name.c_str(), pspr);

	return pspr;
}

