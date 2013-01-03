
#include "BZSpriteCommon.h"

BZSpriteCommon::BZSpriteCommon(CAStageLayer* player, const char* name) : CASprite(player, name)
{
}

BZSpriteCommon::~BZSpriteCommon(void)
{
}

void BZSpriteCommon::_on_state_event(EStateFlag flag)
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

void BZSpriteCommon::_onAnimationStop()
{
	CASprite::_onAnimationStop();
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

void BZSpriteCommon::onStateChanged(const string& olds, const string& news)
{
	_on_state_event(SF_Begin);
}

void BZSpriteCommon::onUpdate()
{
	CASprite::onUpdate();
	_on_state_event(SF_Update);
}

#include "AWorld.h"
REG_CLASS(BZSpriteCommon);
