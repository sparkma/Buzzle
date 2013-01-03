#if !defined(_STAGE_PLAY_LAYER_GAME_PREPARE_H_)
#define _STAGE_PLAY_LAYER_GAME_PREPARE_H_

#include "AStageLayer.h"
#include "BZGroupNumber.h"

class BZSpritePlayer;
class BZSpriteCommon;
class BZSpriteButton;

#include "BZGameMenu.h"

class BZStagePlayLayerGamePrepare :
	public CAStageLayer, public IBZGameMenuListener
{
protected:
	//Define_Sprite_Getter(BZSpriteCommon, prepare_ui_button_newgame);

	void _NullGetters()
	{
		//Init_Sprite_Getter_Var(prepare_ui_button_newgame);
	}
	void _InitGetters()
	{
		//Init_Sprite_Getter(BZSpriteCommon, prepare_ui_button_newgame);
	}

	void _findNumberSprites(const char* prefix, CASprite** ppsprs, int size);
	//BZGroupNumber _dist_last;
	//BZGroupNumber _dist_max;
	//BZGroupNumber _coin_last;
	//BZGroupNumber _coin_max;

	BZGameMenu* _pmenu;
	string _gamemode;
	string _selectedMenuBar;
	void _addMenuBar(const char* label);
	void _handleMenuMessage(const string& id);
public:
	BZStagePlayLayerGamePrepare(CAStage* pstage, CAStageLayer* playerParent);
	virtual ~BZStagePlayLayerGamePrepare(void);

	virtual void show(bool s);
	virtual string debuglog();

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();

	virtual void onBarClicked(const string& id);

	virtual void onEvent(const CAEvent* pevt);

	virtual bool checkCondition(CAState* from, const CATransition& trans);

	virtual void onStateBegin(CAState* from, void* param);
	//virtual void onStateUpdate(CAState* from, size_t counter, ccTime time);
	virtual void onStateEnd(CAState* from, void* param);
};

#endif //_STAGE_PLAY_LAYER_GAME_PREPARE_H_
