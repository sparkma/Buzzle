
#include "BZBubble.h"
#include "BZGame.h"

#include "AStageLayer.h"
#include "AWorld.h"

#define _FALLING_DX_	(1.0f / 20.0f)
#define _FALLING_DELAY	(0.0005f)
#define DELAY_OF_STOPPING	(2.1f)
#define DEFAULT_ACCELERATION (36.70f)

static int s_debug_id = 0;

///Block
BZBubble::BZBubble(BZBoard* pboard)
{
	_pboard = pboard;
	_pblock = null;

	_acceleration = DEFAULT_ACCELERATION;

	_row = _col = -1;
	_pos.x = _pos.y = -100;

	_psprBubble = null;
	_psprProp = null;
	_psprDoodad = null;

	autorelease();

	s_debug_id++;
	_debug_id = s_debug_id;
	_Debug("bubble #%02d created(%p)", _debug_id, this);

	_setState(BS_NA);
}

BZBubble::~BZBubble()
{
	_Debug("bubble #%02d released(%p)", _debug_id, this);
	setBlock(null);
	detach(_pboard->game()->layer());
}

void BZBubble::setBlock(BZBlock* pblock) 
{
	if (null == _pblock && null == pblock)
		return;

	_Debug("bubble #%02d leave block #%02d, enter block #%02d", 
		_debug_id, 
		null == _pblock ? -1 : _pblock->debug_id(),
		null == pblock ? -1 : pblock->debug_id());

	if (null != _pblock)
	{
		_pblock->release();
	}
	_pblock = pblock; 
	if (null != pblock)
	{
		pblock->retain();
	}
}

void BZBubble::setFallingAcceleration(float a)
{
	_acceleration = a;
}

static const char* _state2str(EBubbleState s)
{
	switch (s)
	{
#define HANDLE_STATE2STR(a) case a: return ""#a
	HANDLE_STATE2STR(BS_NA);
	HANDLE_STATE2STR(BS_NAing);
	HANDLE_STATE2STR(BS_Born);
	HANDLE_STATE2STR(BS_Borning);
	HANDLE_STATE2STR(BS_Release);
	HANDLE_STATE2STR(BS_Fall);
	HANDLE_STATE2STR(BS_Falling);
	HANDLE_STATE2STR(BS_Drag);
	HANDLE_STATE2STR(BS_Dragging);
	HANDLE_STATE2STR(BS_Stop);
	HANDLE_STATE2STR(BS_Stopping);
	HANDLE_STATE2STR(BS_BlockBlend);
	HANDLE_STATE2STR(BS_BlockBlending);
	HANDLE_STATE2STR(BS_PoseBlend);
	HANDLE_STATE2STR(BS_PoseBlending);
	HANDLE_STATE2STR(BS_Stand);
	HANDLE_STATE2STR(BS_Standing);
	HANDLE_STATE2STR(BS_Die);
	HANDLE_STATE2STR(BS_Dying);
	HANDLE_STATE2STR(BS_Died);
	default: _Assert(false); break;
#undef HANDLE_STATE2STR
	}
	return "***UnknownState***";
}

void BZBubble::_setState(EBubbleState s)
{
	_timeStateBegin = _pboard->getTimeNow();
	_state = s;
	_Trace("bubble #%02d state ==> %s", this->debug_id(), _state2str(s));
	if (_pblock)
	{
		_pblock->setDirty(true);
	}
	_pboard->onBubbleStateChanged(this, _state);
}

void BZBubble::initialize(const char* bubble, 
						  const char* prop, 
						  const char* doodad,
						  float zorder)
{
	GUARD_FUNCTION();

	_bubbleType = bubble;
	CASprite* pspr = new BZSpriteCommon(_pboard->game()->layer(), bubble);
	pspr->setState("na");
	_pboard->game()->layer()->addSprite(pspr);
	_psprBubble = pspr;

	CCSize size = CAWorld::getScreenSize();
	
	_psprBubble->setScl(_pboard->getBubbleSize() / size.width);
	_psprBubble->setZOrder(zorder);
	//_psprBubble->setScale(10.0f);

	if (null != prop)
	{
		_propType = prop;
		pspr = new BZSpriteCommon(_pboard->game()->layer(), prop);
		pspr->setState("stand");
		pspr->setZOrder(zorder + 2);
		_pboard->game()->layer()->addSprite(pspr);
		_psprProp = pspr;
	}
	if (null != doodad)
	{
		pspr = new BZSpriteCommon(_pboard->game()->layer(), prop);
		pspr->setState("stand");
		pspr->setZOrder(zorder + 1);
		_pboard->game()->layer()->addSprite(pspr);
		_psprDoodad = pspr;
	}
}

void BZBubble::setPose(const string& pose)
{
	_psprBubble->setState(pose);
}

void BZBubble::_setPos(float x, float y)
{
	_Assert(_pboard);
	_Assert(_pblock);

	CCPoint posOld = _pos;
	
	_pos.x = x;
	_pos.y = y;
	
	_row = _ROW(y);
	_col = _COL(x);

	//this will refine board in game
	_pboard->onBubblePositionChanged(this, posOld, _pos);

	_Assert(_pboard->verifyBubble(this));
}

bool BZBubble::isStoped() const 
{ 
	return _state >= BS_Stop && _state <= BS_Standing; 
}

void BZBubble::onUpdate()
{
	retain();

	switch (_state)
	{
	case BS_NA:
		_setState(BS_NAing);;
		break;
	case BS_NAing:
		//do nothing, until state is changed to another
		break;
	case BS_Born:
		_psprBubble->setState("born");
		_setState(BS_Borning);
		break;
	case BS_Borning:
		if (_psprBubble->isAnimationDone())
		{
			_setState(BS_Fall);
		}
		break;
	case BS_Fall: 
		//delay for _FALLING_DELAY(0.2f) sec before falling
		if (_pboard->getTimeNow() - _timeStateBegin > _FALLING_DELAY)
		{
			_setState(BS_Falling);
			_psprBubble->setState("xxxx");
			_lastFallingTime = _pboard->getTimeNow();
			_fallingspeed = 0;
		}
		break;
	case BS_Falling:
		{
			CCPoint posOld = _pos;
			CCPoint posNew = _pos;

			float t = _pboard->getTimeNow() - _lastFallingTime;
			//falling time is eaten, even this move may fail.
			_lastFallingTime = _pboard->getTimeNow();

			//adjust posNew.x
			float f = floor(posNew.x);
			float c = ceil(posNew.x);
			if (c - posNew.x < 0.5f) 
			{ 
				if (posNew.x + _FALLING_DX_ < c) 
					posNew.x += _FALLING_DX_; 
				else 
					posNew.x = c; 
			}
			else 
			{ 
				_Assert(posNew.x - f <= 0.5f); 
				if (posNew.x - _FALLING_DX_ > f) 
					posNew.x -= _FALLING_DX_; 
				else 
					posNew.x = f; 
			}

			//acc = 10	v0 = 0
			//0		1		2		3		4
			//		5 s=10	20 s=20	45 s=30	80						 
			float dy = _fallingspeed * t + _acceleration * t / 2.0f;
			posNew.y += dy;
			_fallingspeed += _acceleration * t;

			CCPoint posChair;
			EBubbleBlockerType bt = _pboard->getBubbleBlocker(this, posChair);
			switch (bt)
			{
			case BT_FallingBlock:
				//we are close enough to chair?
				if (posNew.y + 1.0f > posChair.y)
				{
					//posNew.x = posChair.x; //adjust x ?
					posNew.y = posChair.y - 1.0f;
					_fallingspeed = 0.0f; //we will re-fall later
				}
				else
				{
					//posNew.y is accepted.
				}
				break;
			case BT_StoppingBlock:
			case BT_Bottom:
				if (posNew.y + 1.0f >= posChair.y)
				{
					//posNew.x = posChair.x; //adjust x ?
					posNew.y = posChair.y - 1.0f;
					_fallingspeed = 0.0f; 
					//we should stoped
					_setState(BS_Stop);
				}
				break;
			}
			_setPos(posNew.x, posNew.y);
 		}
		break;
	case BS_Drag:
		//enable block light here
		//and change state
		_setState(BS_Dragging);
		_psprBubble->setState("xxxx");
		break;
	case BS_Dragging:
		break;
	case BS_Release:
		//turn off block light now
		//block will falling
		_setState(BS_Fall);
		break;
	case BS_Stop:
		_setState(BS_Stopping);
		{
			//adjust delta x
			CCPoint posNew = _pos;
			posNew.x = (float)_COL(posNew.x);
			posNew.y = (float)_ROW(posNew.y);
			_setPos(posNew.x, posNew.y);
			_psprBubble->setState("stop");
		}
		break;
	case BS_Stopping:
		//calcualte blending here
		if (true || _psprBubble->isAnimationDone())
		{
			_setState(BS_BlockBlend);
		}
		break;
	case BS_BlockBlend:
		_setState(BS_BlockBlending);
		break;
	case BS_BlockBlending:
		_setState(BS_PoseBlend);
		break;
	case BS_PoseBlend:
		if (_psprBubble->getState() == "stop" && !_psprBubble->isAnimationDone())
		{
			//wait
		}
		else
		{
			_setState(BS_PoseBlending);
		}
		break;
	case BS_PoseBlending:
		_setState(BS_Stand);
		break;
	case BS_Stand:
		_setState(BS_Standing);
		break;
	case BS_Standing:
		//do nothing here
		//if I should be belended with another block, game will call setNeighbour
		if (_pboard->canFall(this))
		{
			_setState(BS_Fall);
		}
		break;
	case BS_Die:
		_setState(BS_Dying);
		_psprBubble->setState("dead");
		break;
	case BS_Dying:
		//and _psprBubble is BZSpriteCommon
		if (_psprBubble->isAnimationDone())
		{
			_setState(BS_Died);
		}
		break;
	case BS_Died:
		break;
	default:
		_Assert(false);
		break;
	}

	if (null != _psprBubble)
	{
		CCPoint pt = _pos;
		_pboard->getBubbleRenderPos(pt);
		_psprBubble->setPos(pt);

		if (_psprProp)
		{
			//_psprBubble->setAlpha(0.3f);
			_psprProp->setZOrder(20.0f);
			_psprProp->setPos(pt);
		}
	}

	release();
}

bool BZBubble::canMove() const
{
	BZBubble* pn = null;

	if (BS_Standing != _state && BS_Falling != _state)
		return false;

	return _pboard->canMove(this);
}

void BZBubble::detach(CAStageLayer* player)
{
	_Debug("bubble #%02d detached from stage layer(%p)", _debug_id, this);

	_Assert(player);

	_setState(BS_NA);

	if (_psprBubble)
	{
		player->removeSprite(_psprBubble);
		_psprBubble = null;
	}
	if (_psprProp)
	{
		player->removeSprite(_psprProp);
		_psprProp = null;
	}
	if (_psprDoodad)
	{
		player->removeSprite(_psprDoodad);
		_psprDoodad = null;
	}
}
