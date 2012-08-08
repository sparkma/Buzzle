
#include "BZBlockBubble.h"
#include "BZGame.h"

#include "AStageLayer.h"
#include "AWorld.h"

#define _FALLING_DX_	(1.0f / 20.0f)
#define _FALLING_DELAY	(0.05f)
#define DEFAULT_ACCELERATION (16.70f)
#define _ROW(a)		((int)((a) + 0.5f))
#define _COL(a)		_ROW(a)
///Block
BZBlockBubble::BZBlockBubble(BZBoard* pboard)
{
	_pboard = pboard;
	_pblock = null;

	_acceleration = DEFAULT_ACCELERATION;
	_setState(BS_NA);

	_row = _col = -1;

	memset(_neighbours, 0, sizeof(_neighbours));

	autorelease();
}

BZBlockBubble::~BZBlockBubble()
{
	//_psprBubble->release();
	//_psprBubble = null;
}

void BZBlockBubble::setFallingAcceleration(float a)
{
	_acceleration = a;
}

void BZBlockBubble::_setState(EBubbleState s)
{
	_timeStateBegin = _pboard->getTimeNow();
	_state = s;
	_pboard->onBubbleStateChanged(this, _state);
}

void BZBlockBubble::initialize(const char* name)
{
	_type = name;
	CASprite* pspr = new BZSpriteCommon(_pboard->game()->layer(), name);
	pspr->setState("na");
	//pspr->retain();
	_pboard->game()->layer()->addSprite(pspr);
	_psprBubble = pspr;
}

void BZBlockBubble::setNeighbour(EBubbleNeighbour bn, BZBlockBubble* pbubble)
{
	BZBlockBubble* pn;

	if (_neighbours[bn] == pbubble)
	{
		return;
	}
	if (_neighbours[bn]) 
	{
		pn = _neighbours[bn];
		pn->release();
		//must set to null first, or the next pn->setNeighbure 
		//will make us fall into a deadloop
		_neighbours[bn] = null; 
		pn->setNeighbour((EBubbleNeighbour)((bn + 2) % 4), null);
	}
	_neighbours[bn] = pbubble;
	if (pbubble) pbubble->retain();
	if (pbubble) pbubble->setNeighbour((EBubbleNeighbour)((bn + 2) % 4), this);

	//change pose here
	string pose = "";
	int dir = 0;


	pn = _neighbours[N_RIGHT];	if (null != pn && pn->getType() == _type) 	pose += "o"; else pose += "x";
	pn = _neighbours[N_BOTTOM]; if (null != pn && pn->getType() == _type) 	pose += "o"; else pose += "x";
	pn = _neighbours[N_LEFT];	if (null != pn && pn->getType() == _type) 	pose += "o"; else pose += "x";
	pn = _neighbours[N_TOP];	if (null != pn && pn->getType() == _type) 	pose += "o"; else pose += "x";
	
	_psprBubble->switchPose(pose);
}

void BZBlockBubble::_setPos(float x, float y)
{
	_Assert(_pboard);

	_pos.x = x;
	_pos.y = y;
	
	//this will refine board in game
	_pboard->onBubblePositionChanged(this, _pos);
	
	_row = _ROW(y);
	_col = _COL(x);

	_Assert(_pboard->verifyBubble(this));
}

void BZBlockBubble::onUpdate()
{
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
			//_psprBubble->setState("stop");
		}
		break;
	case BS_Stopping:
		//calcualte blending here
		if (_pboard->getTimeNow() - _timeStateBegin > 0.3f)
			//&& 
			//_psprBubble->isAnimationDone())
		{
			_setState(BS_Stand);
		}
		break;
	case BS_Stand:
		_setState(BS_Standing);
		break;
	case BS_Standing:
		//do nothing here
		//if I should be belended with another block, game will call setNeighbour
		if (null == this->_neighbours[N_BOTTOM] && _row != _pboard->getRows() - 1)
		{
			_setState(BS_Fall);
		}
		break;
	case BS_Die:
		_setState(BS_Dying);
		//_psprBubble->setState("dead");
		break;
	case BS_Dying:
		//do nothing, "dead" is the deadpose
		//and _psprBubble is BZSpriteCommon
		_setState(BS_Died);
		break;
	case BS_Died:
		break;
	default:
		_Assert(false);
		break;
	}

	CCPoint pt = _pos;
	_pboard->getBubbleRenderPos(pt);

	_psprBubble->setZOrder(50.0f);
	//_psprBubble->setScale(10.0f);
	_psprBubble->setPos(pt);
}

void BZBlockBubble::detach(CAStageLayer* player)
{
	_Assert(player);
	_Assert(_psprBubble);

	this->setNeighbour(N_TOP, null);
	this->setNeighbour(N_LEFT, null);
	this->setNeighbour(N_BOTTOM, null);
	this->setNeighbour(N_RIGHT, null);

	player->removeSprite(_psprBubble);
}
