#if !defined(_STAGE_PLAY_LAYER_GAME_PLAY_PAUSE_H_)
#define _STAGE_PLAY_LAYER_GAME_PLAY_PAUSE_H_

#include "AStageLayer.h"

class BZSpritePlayer;
class BZSpriteCommon;
class BZSpriteButton;

#define EVENT_ONRESUME	"onResume"
#define EVENT_ONRESTART	"onRestart"

#include "BZGameMenu.h"

class BZStagePlayLayerGamePlayPause :
	public CAStageLayer, public IBZGameMenuListener
{
protected:
	Define_Sprite_Getter(BZSpriteCommon, pause_ui_back);

	void _NullGetters()
	{
		Init_Sprite_Getter_Var(pause_ui_back);
	}

	void _InitGetters()
	{
		//Init_Sprite_Getter(BZSpriteCommon, label_pause_title);
		Init_Sprite_Getter(BZSpriteCommon, pause_ui_back);
	}

	void _updateNumber(const char* prefix, int nValue);

	CASprite* _psprSound;

	BZGameMenu* _pmenu;
	string _selectedMenuBar;
	void _addMenuBar(const char* label);
	void _handleMenuMessage(const string& id);
public:
	BZStagePlayLayerGamePlayPause(CAStage* pstage, CAStageLayer* playerParent);
	virtual ~BZStagePlayLayerGamePlayPause(void);

	virtual void show(bool s);

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();

	virtual void onBarClicked(const string& id);
	virtual void onEvent(CAEvent* pevt);

	virtual bool checkCondition(CAState* from, const CATransition& trans);

	virtual void onStateBegin(CAState* from, void* param);
	//virtual void onStateUpdate(CAState* from, size_t counter, ccTime time);
	virtual void onStateEnd(CAState* from, void* param);
};

#endif //_STAGE_PLAY_LAYER_GAME_PLAY_PAUSE_H_
