
#include "BZSpriteButton.h"
#include "AStageLayer.h"

//#define ButtonPose_Stand	"stand"
#define ButtonPose_Pressed	"pressed"
#define ButtonPose_DeadPose	"deadpose"

//na
#define BS_Idle			0
//play pressed pose
#define BS_Pressing		1
//pressed pose finished
#define BS_Pressed		2
//user released
#define BS_Released		4

BZSpriteButton::BZSpriteButton(CAStageLayer* player, const char* name) : CASprite(player, name)
{
	_nClickState_ = BS_Idle;
}

BZSpriteButton::~BZSpriteButton(void)
{
}

void BZSpriteButton::_setClickState(int s)
{
	GUARD_FUNCTION();

	_nClickState_ = s;
	_Debug("button %s -> %d", this->getModName().c_str(), s);

	if (BS_Idle == s)
	{
		//setState(ButtonPose_Stand);
	}
	else if (BS_Pressing == s)
	{
		setState(ButtonPose_Pressed, true);
		CASpriteModelPose* ppose = this->getCurrentPose();
		if (ppose->name() != ButtonPose_Pressed)
		{
			//assume that pose has finished
			_setClickState(_nClickState_ | BS_Pressed);
		}
	}
	else
	{
		if ((BS_Pressed & s) && (BS_Released & s) && (BS_Pressing & s))
		{
			_Debug("button %s clicked", this->getModName().c_str());
			s = BS_Idle;
			//setState(ButtonPose_Stand);
			_pLayer->onEvent(new CAEventCommand(this, "onClick"));
		}
	}
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
		_setClickState(_nClickState_ | BS_Pressed);
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
	_Debug("button %s touch leave", this->getModName().c_str());
	_setClickState(BS_Idle);
	//setState(ButtonPose_Stand);
}

void BZSpriteButton::onTouched(CAEventTouch* pEvent) 
{
	GUARD_FUNCTION();

	switch (pEvent->state())
	{
	case kTouchStateGrabbed:
		_Debug("button %s touch grabbed", this->getModName().c_str());
		_setClickState(BS_Pressing);
		break;
	case kTouchStateUngrabbed:
		_Debug("button %s touch ungrabbed", this->getModName().c_str());
		if (_nClickState_ & BS_Pressing)
		{
			_setClickState(_nClickState_ | BS_Released);
		}
		else
		{
			_setClickState(BS_Idle);
		}
		break;
	}
}

#include "AWorld.h"
REG_CLASS(BZSpriteButton);
