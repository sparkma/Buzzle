
#include "BZSpriteButton.h"
#include "AStageLayer.h"

#define ButtonPose_Stand	"stand"
#define ButtonPose_Pressed	"pressed"
#define ButtonPose_DeadPose	"deadpose"

BZSpriteButton::BZSpriteButton(CAStageLayer* player, const char* name) : CASprite(player, name)
{
	_nClickState = 0;
}

BZSpriteButton::~BZSpriteButton(void)
{
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
	if (ppose->name() == ButtonPose_Pressed &&_nClickState == 2)
	{
		setState(ButtonPose_Stand);
		_nClickState = 0;
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

void BZSpriteButton::onTouchLeave(CAEventTouch* pEvent) 
{
	_nClickState = 0;
	//setState(ButtonPose_Stand);
}

void BZSpriteButton::onTouched(CAEventTouch* pEvent) 
{
	GUARD_FUNCTION();

	switch (pEvent->state())
	{
	case kTouchStateGrabbed:
		_nClickState = 1;
		if (ButtonPose_Pressed == getState())
			setState(ButtonPose_Stand);
		setState(ButtonPose_Pressed);
		//_Trace("button %s pressed", this->getModName().c_str());
		break;
	case kTouchStateUngrabbed:
		//_Trace("button %s state=%d", this->getModName().c_str(), _nClickState);
		if (_nClickState == 1) //_state == ButtonPose_Pressed && 
		{
			GUARD_FIELD(_button_touch_event);
			//_Trace("button %s clicked", this->getModName().c_str());
			_nClickState = 2;
			_pLayer->onEvent(new CAEventCommand(this, "onClick"));
			//setState(ButtonPose_Stand);
		}
		else
		{
			setState(ButtonPose_Stand);
		}
		break;
	}
}

#include "AWorld.h"
REG_CLASS(BZSpriteButton);
