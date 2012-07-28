#if !defined(_STAGE_PLAY_LAYER_GAME_PLAY_PAUSE_H_)
#define _STAGE_PLAY_LAYER_GAME_PLAY_PAUSE_H_

#include "AStageLayer.h"

class BZSpritePlayer;
class BZSpriteCommon;
class BZSpriteButton;

#define EVENT_ONRESUME	"onResume"
#define EVENT_ONRESTART	"onRestart"

class BZStagePlayLayerGamePlayPause :
	public CAStageLayer //, public CCTargetedTouchDelegate
{
protected:
	Define_Sprite_Getter(BZSpriteCommon, label_pause_title);
	Define_Sprite_Getter(BZSpriteCommon, panel_black);
	Define_Sprite_Getter(BZSpriteButton, button_restart);
	Define_Sprite_Getter(BZSpriteButton, button_resume);
	Define_Sprite_Getter(BZSpriteButton, button_music);
	Define_Sprite_Getter(BZSpriteButton, button_sound);

	void _NullGetters()
	{
		Init_Sprite_Getter_Var(label_pause_title);
		Init_Sprite_Getter_Var(panel_black);
		Init_Sprite_Getter_Var(button_restart);
		Init_Sprite_Getter_Var(button_resume);
		Init_Sprite_Getter_Var(button_music);
		Init_Sprite_Getter_Var(button_sound);
	}

	void _InitGetters()
	{
		//Init_Sprite_Getter(BZSpriteCommon, label_pause_title);
		Init_Sprite_Getter(BZSpriteCommon, panel_black);
		Init_Sprite_Getter(BZSpriteButton, button_restart);
		Init_Sprite_Getter(BZSpriteButton, button_resume);
		Init_Sprite_Getter(BZSpriteButton, button_music);
		Init_Sprite_Getter(BZSpriteButton, button_sound);
	}

	void _updateNumber(const char* prefix, int nValue);
public:
	BZStagePlayLayerGamePlayPause(CAStage* pstage, CAStageLayer* playerParent);
	virtual ~BZStagePlayLayerGamePlayPause(void);

	virtual void show(bool s);

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();

	virtual void onEvent(CAEvent* pevt);

	virtual bool checkCondition(CAState* from, const CATransition& trans);

	virtual void onStateBegin(CAState* from, void* param);
	//virtual void onStateUpdate(CAState* from, size_t counter, ccTime time);
	virtual void onStateEnd(CAState* from, void* param);
};

#endif //_STAGE_PLAY_LAYER_GAME_PLAY_PAUSE_H_
