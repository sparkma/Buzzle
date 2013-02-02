
#include "BZSpriteCommon.h"

BZSpriteCommon::BZSpriteCommon(CAStageLayer* player, const char* name) : CASprite(player, name)
{
	_currentState = -1;
	_pendingStatesCount = 0;
	_delayAnimation = 0.0f;
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
			bool r = this->switchPose(_state, _delayAnimation);
			//_Assert(r);
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

	if (_pendingStatesCount <= 0)
	{
		//old mode
	}
	else
	{
		_Assert(_currentState >= 0);
		if (_currentState + 1 < _pendingStatesCount)
		{
			setState(_pendingStates[++_currentState]);
		}
		else
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

void BZSpriteCommon::pushState(const char* state)
{
	if (_pendingStatesCount < SIZE_OF_ARRAY(_pendingStates))
	{
		_pendingStates[_pendingStatesCount++] = state;
	}
	if (_currentState < 0)
	{
		setState(state);
		_currentState = 0;
	}
}

#include "AWorld.h"
REG_CLASS(BZSpriteCommon);
