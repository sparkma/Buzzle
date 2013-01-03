
#include "BZSpriteCommon2.h"

BZSpriteCommon2::BZSpriteCommon2(CAStageLayer* player, const char* name) : CASprite(player, name)
{
}

BZSpriteCommon2::~BZSpriteCommon2(void)
{
}

void BZSpriteCommon2::_on_state_event(EStateFlag flag)
{
	switch(flag)
	{
	case SF_Begin:
		{
			this->switchPose(_state);
		}
		break;
	case SF_Update:
		{
		}
		break;
	default:
		_Assert(false);
		break;
	}
}

void BZSpriteCommon2::_onAnimationStop()
{
	string dp = _overrided_deadpose;
	if (dp.length() <= 0)
	{
		if (_settings().hasKey("deadpose"))
		{
			dp = _settings().getString("deadpose");
		}
	}
	if (dp.length() > 0)
	{
		CASpriteModelPose* ppose = this->_currentPose;
		if (_state == dp && ppose->name() == dp)
		{
			this->killMyself();
		}
	}
}

void BZSpriteCommon2::onStateChanged(const string& olds, const string& news)
{
	_on_state_event(SF_Begin);
}

void BZSpriteCommon2::onUpdate()
{
	CASprite::onUpdate();
	_on_state_event(SF_Update);
}

#include "AWorld.h"
REG_CLASS(BZSpriteCommon2);
