#if !defined(_TSSPRITE_COMMON2_H_)
#define _TSSPRITE_COMMON2_H_
#include "ASprite.h"

class BZSpriteCommon2 :
	public CASprite
{
protected:
	string _overrided_deadpose;
	void _on_state_event(EStateFlag flag);
	virtual void _onAnimationStop();
public:
	BZSpriteCommon2(CAStageLayer* palyer = null, const char* name = null);
	virtual ~BZSpriteCommon2(void);

	void onStateChanged(const string& olds, const string& news);

	void setDeadPose(const string& pose) { _overrided_deadpose = pose; }

	virtual void onUpdate();
};

#endif //_TSSPRITE_COMMON2_H_
