#if !defined(_STAGE_PLAY_LAYER_GAME_PLAY_OVER_H_)
#define _STAGE_PLAY_LAYER_GAME_PLAY_OVER_H_

#include "AStageLayer.h"

class BZSpritePlayer;
class BZSpriteCommon;
class BZSpriteButton;

#include "BZGameMenu.h"

//显示本次完的时间
//消除的大块个数
//消除的泡泡个数
//combo 个数
//Level 级别
//获得的分数
//分享按钮
class BZStagePlayLayerGamePlayOver :
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

	BZGameMenu* _pmenu;
	string _selectedMenuBar;
	void _addMenuBar(const char* label);
	void _handleMenuMessage(const string& id);
public:
	BZStagePlayLayerGamePlayOver(CAStage* pstage, CAStageLayer* playerParent);
	virtual ~BZStagePlayLayerGamePlayOver(void);

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

#endif //_STAGE_PLAY_LAYER_GAME_PLAY_OVER_H_
