#if !defined(_TSSPRITE_COMMON_H_)
#define _TSSPRITE_COMMON_H_
#include "ASprite.h"

class BZSpriteCommon :
	public CASprite
{
protected:
	//string _overrided_deadpose;
	void _on_state_event(EStateFlag flag);
	virtual void _onAnimationStop();

	float _delayAnimation;

	string _pendingStates[16];
	int _pendingStatesCount;
	int _currentState;
	void _navigate2NextState();
public:
	BZSpriteCommon(CAStageLayer* palyer = null, const char* name = null);
	virtual ~BZSpriteCommon(void);

	void onStateChanged(const string& olds, const string& news);
	//void setDeadPose(const string& pose) { _overrided_deadpose = pose; }

	void setAnamitionDelay(float delayAnimation) { _delayAnimation = delayAnimation; }
	void pushState(const char* state);

	virtual void onUpdate();
};

#endif //_TSSPRITE_COMMON_H_
