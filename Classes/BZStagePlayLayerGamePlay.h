#if !defined(_STAGE_PLAY_LAYER_GAME_PLAY_H_)
#define _STAGE_PLAY_LAYER_GAME_PLAY_H_

#include "AStageLayer.h"

class BZSpriteCommon;
class BZSpriteButton;
#include "BZGroupNumber.h"

class BZGame;

class BZStagePlayLayerGamePlay :
	public CAStageLayer //, public CCTargetedTouchDelegate
{
protected:
	Define_Sprite_Getter(BZSpriteButton, play_ui_button_pause);

	void _NullGetters()
	{
		Init_Sprite_Getter_Var(play_ui_button_pause);
	}

	void _InitGetters()
	{
		Init_Sprite_Getter(BZSpriteButton, play_ui_button_pause);
	}

	BZGroupNumber* _score;

	string	_state;
	int		_nScore;
	void _updateScore();
	
	BZGame* _pgame;
	void _initGame();
public:
	BZStagePlayLayerGamePlay(CAStage* pstage, CAStageLayer* playerParent);
	virtual ~BZStagePlayLayerGamePlay(void);

	virtual string debuglog();

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();

	virtual void pause(bool bsub = false);
	virtual void resume(bool bsub = false);

	virtual void onEvent(CAEvent* pevt);

	virtual bool checkCondition(CAState* from, const CATransition& trans);

	virtual void onStateBegin(CAState* from, void* param);
	//virtual void onStateUpdate(CAState* from, size_t counter, ccTime time);
	virtual void onStateEnd(CAState* from, void* param);
};

#endif //_STAGE_PLAY_LAYER_GAME_PLAY_H_
