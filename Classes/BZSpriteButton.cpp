
#include "BZSpriteButton.h"
#include "AStageLayer.h"

//#define ButtonPose_Stand	"stand"
#define ButtonPose_Pressed	"pressed"
#define ButtonPose_DeadPose	"deadpose"

BZSpriteButton::BZSpriteButton(CAStageLayer* player, const char* name) : CASprite(player, name)
{
	_animateState = BS_Idle;
	_bEnabled = true;
}

BZSpriteButton::~BZSpriteButton(void)
{
}

void BZSpriteButton::_setAnimateState(EAnimateState s)
{
	GUARD_FUNCTION();
	_animateState = s;
	if (BS_Idle == s)
	{
		_touchState = TS_None;
	}
	_Info("button %s -> %d", this->getModName().c_str(), s);
}

void BZSpriteButton::_on_state_event(EStateFlag flag)
{
	switch(flag)
	{
	case SF_Begin:
		{
			if (_model->hasPose(_state))
			{
				this->switchPose(_state);
			}
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

void BZSpriteButton::_onAnimationStop()
{
	CASpriteModelPose* ppose = this->getCurrentPose();
	if (ppose->name() == ButtonPose_Pressed)
	{
		_onPressedAnimationFinished();
	}
	if (_settings().hasKey(ButtonPose_DeadPose))
	{
		string dp = _settings().getString(ButtonPose_DeadPose);
		CASpriteModelPose* ppose = this->_currentPose;
		if (_state == dp && ppose->name() == dp)
		{
			this->killMyself();
		}
	}
}

void BZSpriteButton::onStateChanged(const string& olds, const string& news)
{
	_on_state_event(SF_Begin);
}

void BZSpriteButton::onEnter()
{
	this->setTouchable(true);
	this->setControlOrder(1);
	_pLayer->addControl(this);
	CASprite::onEnter();
}

void BZSpriteButton::onUpdate()
{
	CASprite::onUpdate();
	_on_state_event(SF_Update);
}

void BZSpriteButton::onExit()
{
	CASprite::onExit();
}

void BZSpriteButton::_onClick()
{
	if (!_bEnabled)
		return;

	_setAnimateState(BS_Idle);
	_Info("button %s clicked", this->getModName().c_str());
	_pLayer->onEvent(new CAEventCommand(this, "onClick"));
}

void BZSpriteButton::_onPressedAnimationFinished()
{
	if (!_bEnabled)
		return;

	_Info("button %s animated", this->getModName().c_str());
	_setAnimateState(BS_Animated);
	if ((_touchState & TS_Down) && (_touchState & TS_Up))
	{
		_onClick();
	}
}

void BZSpriteButton::onTouchLeave(CAEventTouch* pEvent) 
{
	if (!_bEnabled)
		return;

	_Info("button %s touch leave", this->getModName().c_str());
	_setAnimateState(BS_Idle);
}

void BZSpriteButton::onTouched(CAEventTouch* pEvent) 
{
	if (!_bEnabled)
		return;

	GUARD_FUNCTION();

	switch (pEvent->state())
	{
	case kTouchStateGrabbed:
		_Info("button %s touch grabbed", this->getModName().c_str());
		if (_animateState == BS_Idle)
		{
			_Info("button %s press", this->getModName().c_str());
			_touchState |= TS_Down;
			_setAnimateState(BS_Animating);
			setState(ButtonPose_Pressed, true);
			CASpriteModelPose* ppose = this->getCurrentPose();
			if (ppose->name() != ButtonPose_Pressed)
			{
				_onPressedAnimationFinished();
			}
		}
		else if (_animateState == BS_Animating)
		{
			//do nothing
		}
		break;
	case kTouchStateUngrabbed:
		_Info("button %s touch ungrabbed", this->getModName().c_str());
		if (_animateState == BS_Idle)
		{
		}
		else if (_animateState == BS_Animating)
		{
			_Info("button %s touch up flag", this->getModName().c_str());
			_touchState |= TS_Up;
		}
		else if (_animateState == BS_Animated)
		{
			_Info("button %s touch end, onclick", this->getModName().c_str());
			_onClick();
		}
		break;
	}
}

#include "AWorld.h"
REG_CLASS(BZSpriteButton);
