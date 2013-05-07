#if !defined(_TSSPRITE_BUTTON_H_)
#define _TSSPRITE_BUTTON_H_
#include "ASprite.h"

// idle
//		touchbegin --> animating
//		touchend --> idle
// animating
//		touchbegin --> nothing
//		touchend --> set pending flag
//		anim_end --> animated
// animated
//		touchbegin -> nothing
//		if (pending flag) onClick -> idle
//		touchend -> onClick -> idle
// 

class BZSpriteButton :
	public CASprite
{
protected:
	void _on_state_event(EStateFlag flag);
	virtual void _onAnimationStop();

	typedef enum enumTouchState
	{
		TS_None	= 0,
		TS_Down,
		TS_Up
	}
	ETouchState;
	int _touchState;

	typedef enum enumAnimateState
	{
		BS_Idle	= 0,
		//play pressed pose
		BS_Animating = 1,
		//pressed pose finished
		BS_Animated = 2,
	}
	EAnimateState;

	EAnimateState _animateState;
	void _setAnimateState(EAnimateState s);

	bool _bEnabled;

	void _onPressedAnimationFinished();
	void _onClick();
public:
	BZSpriteButton(CAStageLayer* palyer = null, const char* name = null);
	virtual ~BZSpriteButton(void);

	void enable(bool e) { _bEnabled = e; }
	void onStateChanged(const string& olds, const string& news);

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();

	virtual void onTouched(CAEventTouch* pEvent);
	virtual void onTouchLeave(CAEventTouch* pEvent);
};

#endif //_TSSPRITE_BUTTON_H_
