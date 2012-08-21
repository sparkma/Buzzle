#if !defined(_TSSPRITE_COMMON_H_)
#define _TSSPRITE_COMMON_H_
#include "ASprite.h"

class BZSpriteCommon :
	public CASprite
{
protected:
	string _overrided_deadpose;
	void _on_state_event(EStateFlag flag);
	virtual void _onAnimationStop();
public:
	BZSpriteCommon(CAStageLayer* palyer, const char* name);
	virtual ~BZSpriteCommon(void);

	void onStateChanged(const string& olds, const string& news);

	void setDeadPose(const string& pose) { _overrided_deadpose = pose; }

	virtual void onUpdate();
};

#endif //_TSSPRITE_COMMON_H_
