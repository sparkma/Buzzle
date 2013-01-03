#if !defined(_STAGE_PLAY_LAYER_GAME_SHOP_H_)
#define _STAGE_PLAY_LAYER_GAME_SHOP_H_

#include "AStageLayer.h"

class BZSpritePlayer;
class BZSpriteCommon;
class BZSpriteButton;

class BZStagePlayLayerGameShop :
	public CAStageLayer //, public CCTargetedTouchDelegate
{
protected:
	Define_Sprite_Getter(BZSpriteCommon, label_pause_title);
	Define_Sprite_Getter(BZSpriteCommon, panel_back);
	Define_Sprite_Getter(BZSpriteButton, button_restart);
	Define_Sprite_Getter(BZSpriteButton, button_resume);
	Define_Sprite_Getter(BZSpriteButton, button_music);
	Define_Sprite_Getter(BZSpriteButton, button_sound);

	void _updateNumber(const char* prefix, int nValue);

	DELCARE_STATE_HANDLER(Idle);
	DELCARE_STATE_HANDLER(Fadein);
	DELCARE_STATE_HANDLER(Running);
	DELCARE_STATE_HANDLER(OnResume);
	DELCARE_STATE_HANDLER(OnRestart);
	DELCARE_STATE_HANDLER(Fadeout);
public:
	BZStagePlayLayerGameShop(CAStage* pstage, CAStageLayer* playerParent);
	virtual ~BZStagePlayLayerGameShop(void);

	virtual void show(bool s);
	virtual string debuglog();

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();

	virtual void onEvent(const CAEvent* pevt);

	virtual bool checkCondition(CAState* from, const CATransition& trans);

	virtual void onStateBegin(CAState* from, void* param);
	//virtual void onStateUpdate(CAState* from, size_t counter, ccTime time);
	virtual void onStateEnd(CAState* from, void* param);
};

#endif //_STAGE_PLAY_LAYER_GAME_SHOP_H_
