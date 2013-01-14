#if !defined(_TSSPRITE_BUTTON_H_)
#define _TSSPRITE_BUTTON_H_
#include "ASprite.h"

class BZSpriteButton :
	public CASprite
{
protected:
	void _on_state_event(EStateFlag flag);
	virtual void _onAnimationStop();

	int _nClickState_;
	void _setClickState(int s);
public:
	BZSpriteButton(CAStageLayer* palyer = null, const char* name = null);
	virtual ~BZSpriteButton(void);

	void onStateChanged(const string& olds, const string& news);

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();

	virtual void onTouched(CAEventTouch* pEvent);
	virtual void onTouchLeave(CAEventTouch* pEvent);
};

#endif //_TSSPRITE_BUTTON_H_
