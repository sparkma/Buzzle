#if !defined(_TSSPRITE_BUBBLE_EYE_H_)
#define _TSSPRITE_BUBBLE_EYE_H_
#include "BZSpriteCommon.h"

class BZSpriteBubbleEye : public BZSpriteCommon
{
protected:
	//string _overrided_deadpose;
	int _lastTime;
	int _interval;
	int _estate;
public:
	BZSpriteBubbleEye(CAStageLayer* palyer = null, const char* name = null);
	virtual ~BZSpriteBubbleEye(void);

	void onStateChanged(const string& olds, const string& news);
	virtual void onUpdate();
};

#endif //_TSSPRITE_BUBBLE_EYE_H_
