
#include "BZBubble.h"
#include "BZGame.h"

#include "AStageLayer.h"
#include "AWorld.h"
#include "AString.h"

#define _FALLING_DX_	(1.0f / 20.0f)
#define _FALLING_DELAY	(0.0001f)
#define DELAY_OF_STOPPING	(2.1f)
#define DEFAULT_ACCELERATION (36.70f)

static int s_debug_id = 0;

///Block
BZBubble::BZBubble(BZBoard* pboard)
{
	_pboard = pboard;
	_pblock = null;

	_acceleration = DEFAULT_ACCELERATION;
	_fallingSpeed = 0;
	_fallingOffset = 0;

	_row = _col = -1;
	_pos.x = _pos.y = -100;

	_psprBubble = null;
	_psprProp = null;
	_psprDoodad = null;

	_psprDeadEffect = null;

	//_bRainfallMode = true;

	_reborn = false;

	_dragingPositions.init(4);

	s_debug_id++;
	_debug_id = s_debug_id;
	_Debug("bubble #%02d created(%p)", _debug_id, this);

	_hasStar = false;

	autorelease();

	_setState(BS_NA);
}

BZBubble::~BZBubble()
{
	_Debug("bubble #%02d released(%p)", _debug_id, this);
	setBlock(null);
	detach(_pboard->layer());
}

#if 0
void BZBubble::loadData(CADataBuf& data)
{
	int n;
	string str;

	data >> str; _Assert(str == "bubbleb");
	data >> n; _Assert(n == 0x10000);

	string bubble, prop, doodad, pose;
	data >> bubble;
	data >> prop;
	data >> doodad;
	data >> pose;

	ccTime time;
	data >> time; _timeStateBegin = _pboard->getTimeNow() - time;
	data >> _state;
	data >> time; _lastFallingTime = _pboard->getTimeNow() - time;

	data >> this->_acceleration;
	data >> this->_fallingSpeed;

	CCPoint pos;
	data >> pos;
	int row, col;
	data >> row;
	data >> col;

	this->initialize(
		bubble.c_str(), 
		prop.length() > 0 ? prop.c_str() : null, 
		doodad.length() > 0 ? doodad.c_str() : null,
		_pboard->getBubbleZOrder());

	this->_setPos(pos.x, pos.y);
	this->setPose(pose);

	_Assert(_row == row);
	_Assert(_col == col);

	data >> str; _Assert(str == "bubblee");
}

void BZBubble::saveData(CADataBuf& data)
{
	data << "bubbleb";
	data << 0x10000;

	data << this->_bubbleType;
	data << this->_propType;
	data << this->_doodadType;
	data << this->_pose;
	data << _pboard->getTimeNow() - this->_timeStateBegin;
	data << this->_state;
	data << _pboard->getTimeNow() - this->_lastFallingTime;
	data << this->_acceleration;
	data << this->_fallingSpeed;
	data << this->_pos;
	data << this->_row;
	data << this->_col;

	data << "bubblee";
}
#endif

float BZBubble::_zorder(float delta)
{
	_Assert(_pboard);
	return _pboard->getBaseZOrder() + delta;
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

const char* BZBubble::state2str(EBubbleState s)
{
	switch (s)
	{
#define HANDLE_STATE2STR(a) case a: return ""#a
	HANDLE_STATE2STR(BS_NA);
	HANDLE_STATE2STR(BS_NAing);
	HANDLE_STATE2STR(BS_Born);
	HANDLE_STATE2STR(BS_Borning);
	HANDLE_STATE2STR(BS_Borned);
	HANDLE_STATE2STR(BS_Gen);
	HANDLE_STATE2STR(BS_Gening);
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
	_Trace("bubble #%02d state ==> %s", this->debug_id(), state2str(s));
	if (_pblock)
	{
		_pblock->setDirty(true);
	}
	_pboard->onBubbleStateChanged(this, _state);
}

//this is attach
void BZBubble::initialize(const char* bubble, const char* prop, const char* doodad)
{
	GUARD_FUNCTION();

	CAStageLayer* player = _pboard->layer();

	_bubbleType = bubble;
	CASprite* pspr = new BZSpriteCommon(player, bubble);
	pspr->setState("na");
	pspr->setVertexZ(_zorder(LAYER_BUBBLE));
	CCSize size = CAWorld::getScreenSize();
	//mul 1.01f, make bubble a bit larger 
	pspr->setScl(_pboard->getBubbleSize() / size.width * 1.06f);

	player->addSprite(pspr);
	_psprBubble = pspr;

	if (null != prop)
	{
		_propType = prop;
		pspr = CAWorld::sharedWorld().createSprite(player, prop);
		pspr->setState("stand");
		pspr->setVertexZ(_zorder(LAYER_PROPS));
		player->addSprite(pspr);
		_posProp.x = CAUtils::Rand(-0.2f, +0.2f);
		_posProp.y = CAUtils::Rand(-0.2f, +0.2f);
		_psprProp = pspr;
	}
	
	if (null != doodad)
	{
		addDoodad(doodad);
	}
	const string& pt = getPropType();
	_hasStar = CAString::hasSubString(pt, "prop_connector");
}

void BZBubble::addDoodad(const char* doodad, const char* pszPose)
{
	if (null != doodad && _doodadType == doodad)
	{
		//_Info("set doodad (%d,%d) as %s, leave 1", _col, _row, pszPose);
		return;
	}
	if (null != _psprProp)
	{
		//_Info("set doodad (%d,%d) as %s, leave 2", _col, _row, pszPose);
		return;
	}

	CAStageLayer* player = _pboard->layer();

	if (_psprDoodad)
	{
		_doodadType = "";
		//player->removeSprite(_psprDoodad);
		_psprDoodad->killMyself();
		_psprDoodad = null;
	}

	if (doodad)
	{
		_doodadType = doodad;
		BZSpriteCommon* pspr = new BZSpriteCommon(player, doodad);
		pszPose = pszPose ? pszPose : "stand";
		pspr->setState(pszPose);
		pspr->setVertexZ(_zorder(LAYER_DOODADS));
		player->addSprite(pspr);
		//_Info("set doodad (%d,%d) as %s", _col, _row, pszPose);
		_posDoodad.x = CAUtils::Rand(-0.3f, +0.3f), 0;
		_posDoodad.y = CAUtils::Rand(-0.3f, +0.3f), 0;
		_psprDoodad = pspr;
	}
}

void BZBubble::setPose(const string& pose)
{
	_pose = pose;
	_psprBubble->setState(pose);
}

void BZBubble::_setPos(float x, float y)
{
	_Assert(_pboard);

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
			_setState(BS_Borned);
		}
		break;
	case BS_Borned:
		//if (_bRainfallMode)
		//{
		//	_setState(BS_Fall);
		//}
		break;
	case BS_Fall: 
		//delay for _FALLING_DELAY(0.2f) sec before falling
		if (_pboard->getTimeNow() - _timeStateBegin > _FALLING_DELAY)
		{
			_setState(BS_Falling);
			_psprBubble->setState("xxxx");
			_lastFallingTime = _pboard->getTimeNow();
			_fallingSpeed = 0;
		}
		break;
	case BS_Falling:
		{
			if (this->_pblock->getState() != Block_Running)
				break;
			CCPoint posOld = _pos;
			CCPoint posNew = _pos;

			float t = _pboard->getTimeNow() - _lastFallingTime;
			//falling time is eaten, even this move may fail.
			_lastFallingTime = _pboard->getTimeNow();

			/*
			//adjust posNew.x

			//如果距离着点过近，则不做调整。Thredhood = 1.0f
			//如果大于此距离，则根据水平偏倚值，计算落点磁性
			//选择磁性最大的点，作为计算下落偏倚量
			int dir = _fallingOffset >= 0 ? 1 : -1;
			int testc = (int)_fallingOffset; if (testc < 0) testc = -testc;

			float mags[32];
			memset(mags, 0, sizeof(mags));

			int i;
			for (i = 0; i < testc; i++)
			{
				int r = _row;
				int c = _col + i * dir;
				
				//if (null != _pboard->getBubbleByGridPos(_row, _col + i
				CCPoint posChair;
				EBubbleBlockerType bt = _pboard->getBubbleBlocker(_row, _col + i * dir, posChair);
			}
			*/

			//adjust pos.x
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

			float dy = _fallingSpeed * t + _acceleration * t / 2.0f;
			posNew.y += dy;
			_fallingSpeed += _acceleration * t;

			/*
			//float dx = _FALLING_DX_ * t * _fallingOffset;
			float dx = t * _fallingOffset;
			_fallingOffset *= 0.9f;
			if (dx > 0.3f) dx = 0.3f;
			else if (dx < -0.3f) dx = -0.3f;
			posNew.x += dx;
			if (_fallingOffset != 0.0f)
			{
				_Info("_fallingOffset = %.4f, dx=%.4f", _fallingOffset, dx);
			}
			*/

			//CCPoint posTest = posNew;
			//if (_adjustPosition(posTest))
			//{
			//	posNew = posTest;
			//}

			//int r = _ROW(posNew.y);
			//int c = _COL(posNew.x);

			CCPoint posChair;
			EBubbleBlockerType bt = _pboard->getBubbleBlocker(_row, _col, posChair);
			switch (bt)
			{
			case BT_FallingBlock:
				//we are close enough to chair?
				if (posNew.y + 1.0f > posChair.y)
				{
					//posNew.x = posChair.x; //adjust x ?
					posNew.y = posChair.y - 1.0f;
					_fallingSpeed = 0.0f; //we will re-fall later
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
					_fallingSpeed = 0.0f; 
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
		_dragingPositions.clear();
		break;
	case BS_Dragging:
		break;
	case BS_Gen:
		_psprBubble->setState("gen");
		_setState(BS_Gening);
		_dragingPositions.clear();
		break;
	case BS_Gening:
		if (_psprBubble->isAnimationDone() || _psprBubble->getCurrentPose()->name() != "gen")
		{
			_dragingPositions.clear();
			_setState(BS_Release);
		}
		break;
	case BS_Release:
		//block will falling
		_setState(BS_Fall);
		//how can I find a comfirt way to falling down?
		_onBubbleRelease();
		break;
	case BS_Stop:
		_setState(BS_Stopping);
		{
			//adjust delta x
			_fallingOffset = 0;
			CCPoint posNew = _pos;
			posNew.x = (float)_COL(posNew.x);
			posNew.y = (float)_ROW(posNew.y);
			_setPos(posNew.x, posNew.y);
			_psprBubble->setState("stop");
		}
		break;
	case BS_Stopping:
		//calcualte blending here
		if (_psprBubble->isAnimationDone() || _psprBubble->getCurrentPose()->name() != "stop")
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
		//_psprBubble->setState("xxxx");
		//pop some effects here
		//common effect / heavy effect / prop effect
		_psprBubble->setState("dead");
		if (null != _psprProp)
		{
			_psprProp->setState("dead");
		}
		if (null != _psprDoodad)
		{
			_psprDoodad->setState("dead");
		}
		{
			int i;
			for (i = 0; i < 2; i++)
			{
				int rand = (int)CAUtils::Rand(0, 7);
				char szMod[32];
				sprintf(szMod, "effect_boom%02d", rand + 1);
				char szPose[32];
				rand = (int)CAUtils::Rand(0, 3);
				sprintf(szPose, "b%d", rand + 1);
				string pose = szPose;

				addEffect(szMod, szPose, true);
			}
		}
		break;
	case BS_Dying:
		//and _psprBubble is BZSpriteCommon
		if (_psprBubble->isAnimationDone() || _psprBubble->getCurrentPose()->name() != "dead")
		{
			bool trans = false;
			if (null != _psprDeadEffect)
			{
				if (_psprDeadEffect->isAnimationDone())
					trans = true;
			}
			else trans = true;
			if (trans)
			{
				_setState(BS_Died);
			}
		}
		break;
	case BS_Died:
		//check my post action, if I should gen some new prop-bubble
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
			//_psprProp->setZOrder(20.0f);
			//_psprProp->setVertexZ(_psprBubble->getVertexZ() + 2.0f);
			CCPoint pt = _pos;
			pt.x += _posProp.x;
			pt.y += _posProp.y;
			_pboard->getBubbleRenderPos(pt);
			_psprProp->setPos(pt);
		}
		if (_psprDoodad)
		{
			//_psprBubble->setAlpha(0.3f);
			//_psprProp->setZOrder(20.0f);
			//_psprDoodad->setVertexZ(_psprBubble->getVertexZ() + 2.0f);
			CCPoint pt = _pos;
			pt.x += _posDoodad.x;
			pt.y += _posDoodad.y;
			_pboard->getBubbleRenderPos(pt);
			_psprDoodad->setPos(pt);
		}
	}

	release();
}

bool BZBubble::canMove() const
{
	BZBubble* pn = null;

	if (BS_Stand != _state && BS_Standing != _state && BS_Falling != _state)
		return false;

	return _pboard->canMove(this);
}

void BZBubble::_onBubbleRelease()
{
	int pos = _dragingPositions.pos();
	BZTimedPos* p3 = (BZTimedPos*)_dragingPositions[pos];
	BZTimedPos* p2 = (BZTimedPos*)_dragingPositions[pos - 1];
	BZTimedPos* p1 = (BZTimedPos*)_dragingPositions[pos - 2];
	if (null == p1 || null == p2 || null == p3)
	{
		//do nothing
		_fallingOffset = 0;
	}
	else
	{
		CCPoint d0;
		CCPoint d1;
		d0.x = p2->pos.x - p1->pos.x;
		d0.y = p2->pos.y - p1->pos.y;
		d1.x = p3->pos.x - p2->pos.x;
		d1.y = p3->pos.y - p2->pos.y;
		d0.x = (float)((double)d0.x / ((p2->time - p1->time + 0.0000001f) / 1000.0f));
		d0.y = (float)((double)d0.y / ((p2->time - p1->time + 0.0000001f) / 1000.0f));
		d1.x = (float)((double)d1.x / ((p3->time - p2->time + 0.0000001f) / 1000.0f));
		d1.y = (float)((double)d1.y / ((p3->time - p2->time + 0.0000001f) / 1000.0f));
		CCPoint d;
		d.x = d0.x * 0.2f + d1.x * 0.8f;
		d.y = d0.y * 0.2f + d1.y * 0.8f;
		_Info("drag speed.x = %.2f", d.x);
		//calculate falling direction
		_fallingOffset = d.x;
	}

	_dragingPositions.clear();
}

bool BZBubble::_trySetPos(const CCPoint& p)
{
	CCPoint pos = p;
	bool c = _adjustPosition(pos);
	if (c)
	{
		_setPos(pos.x, pos.y);
	}
	return c;
}

static CCPoint _lerpPoint(float p, const CCPoint& posFrom, const CCPoint& posTo)
{
	CCPoint pos;
	pos.x = posFrom.x * p + posTo.x * (1.0f - p);
	pos.y = posFrom.y * p + posTo.y * (1.0f - p);
	return pos;
}

bool BZBubble::collidAdjust(const CCPoint& posFrom, CCPoint& posTo)
{
	CCPoint& p = this->_pos;
	
	CCRect rectMe;
	CCRect rectTest;
	this->getBoundBox(rectMe);
	BZBubble::getBoundBox(rectTest, posTo);
	bool r = CCRect::CCRectIntersectsRect(rectMe, rectTest);
	if (r)
	{
		//resolve collid
		float left = 0.0f;
		float right = 1.0f;
		CCPoint posTest = posFrom;
		int count = 1;
		while (left < right && count++ < 7)
		{
			float mid = (left + right) * 0.5f;
			CCPoint posTest = _lerpPoint(mid, posFrom, posTo);
			BZBubble::getBoundBox(rectTest, posTest);
			bool collid = CCRect::CCRectIntersectsRect(rectMe, rectTest);
			if (collid)
			{
				right = mid - 0.0001f;
			}
			else
			{
				left = mid + 0.0001f;
			}
		}
		posTo = posTest;
	}
	return r;
}

bool BZBubble::_adjustPosition(CCPoint& pos)
{
	CCPoint posC = pos;
	int r, c;
	int r0 = _ROW(pos.y);
	int c0 = _COL(pos.x);
	_Debug("pos.x=%.2f pos.y=%.2f r0=%d,c0=%d", pos.x, pos.y, r0, c0);

	BZBubble* pbubble = this;

	/*
	BZBubble* pbLeft	= _pboard->getBubble(_ROW(pos.y - 0.5f), c0 - 0);
	BZBubble* pbRight	= _pboard->getBubble(_ROW(pos.y + 0.5f), c0 + 0);
	BZBubble* pbUp		= _pboard->getBubble(r0 - 0, _COL(pos.x - 0.5f));
	BZBubble* pbDown	= _pboard->getBubble(r0 + 0, _COL(pos.x + 0.5f));
	
	if (null != pbLeft	&& pbubble != pbLeft)	pbLeft->collidAdjust(_pos, pos);
	if (null != pbRight && pbubble != pbRight)	pbRight->collidAdjust(_pos, pos);
	if (null != pbUp	&& pbubble != pbUp)		pbUp->collidAdjust(_pos, pos);
	if (null != pbDown	&& pbubble != pbDown)	pbDown->collidAdjust(_pos, pos);
	*/

	float minx, maxx;
	float miny, maxy;
	minx = miny = -10000;
	maxx = maxy = +10000;
	//check left
	r = _ROW(pos.y + 0.0f);	c = _COL(pos.x - 1.0f);	if (_pboard->hasBeenOccupied(r, c, pbubble)) { minx = (float)c0;  }
	//check right
	r = _ROW(pos.y + 0.0f);	c = _COL(pos.x + 1.0f);	if (_pboard->hasBeenOccupied(r, c, pbubble)) { maxx = (float)c0;  }
	//check u
	r = _ROW(pos.y - 1.0f);	c = _COL(pos.x + 0.0f);	if (_pboard->hasBeenOccupied(r, c, pbubble)) { miny = (float)r0;  }
	//check doen
	r = _ROW(pos.y + 1.0f);	c = _COL(pos.x + 0.0f);	if (_pboard->hasBeenOccupied(r, c, pbubble)) { maxy = (float)r0;  }
	_Debug("minx=%.2f maxx=%.2f miny=%.2f maxy=%.2f", minx, maxx, miny, maxy);

	if (pos.x < minx) { _Debug("minx pos.x %.2f->%2f ", pos.x, minx); pos.x = minx; }
	if (pos.x > maxx) { _Debug("maxx pos.x %.2f->%2f ", pos.x, maxx); pos.x = maxx; }
	if (pos.y < miny) { _Debug("miny pos.y %.2f->%2f ", pos.y, miny); pos.y = miny; }
	if (pos.y > maxy) { _Debug("maxy pos.y %.2f->%2f ", pos.y, maxy); pos.y = maxy; }

	r = _ROW(pos.y);
	c = _COL(pos.x);
	_Debug("pos.x=%.2f pos.y=%.2f r=%d c=%d", pos.x, pos.y, r, c);
	if (!_pboard->hasBeenOccupied(r, c, pbubble))
	{
		return true;
	}
	return false;
}

void BZBubble::setDraggingPos(const CCPoint& p, double time)
{ 
	//float d = time - _pboard->getTimeNow();
	BZTimedPos* pp = new BZTimedPos(time, _pboard->getTimeNow(), p);
	_dragingPositions.push(pp);
	CCPoint pos = p;
	_trySetPos(pos);
}

bool BZBubble::hasStar() const
{
	return _hasStar;
}

void BZBubble::detach(CAStageLayer* player)
{
	GUARD_FUNCTION();
	_Debug("bubble #%02d detached from stage layer(%p)", _debug_id, this);

	_Assert(player);

	_setState(BS_NA);

	if (_psprBubble)
	{
		//player->removeSprite(_psprBubble);
		_psprBubble->killMyself();
		_psprBubble = null;
	}
	if (_psprProp)
	{
		//player->removeSprite(_psprProp);
		_propType = "";
		_psprProp->killMyself();
		_psprProp = null;
	}
	if (_psprDoodad)
	{
		//player->removeSprite(_psprDoodad);
		_doodadType = "";
		_psprDoodad->killMyself();
		_psprDoodad = null;
	}
	if (_psprDeadEffect)
	{
		_psprDeadEffect->release();
		_psprDeadEffect = null;
	}
}

void BZBubble::try2Reborn()
{
	GUARD_FUNCTION();

	if (_reborn)
	{
		_Assert(_rebornBubble.length() > 0);
		_Assert(_rebornBubble.length() > 0);
		BZBubble* pb = _pboard->createBubble1(_rebornBubble.c_str(), _pos, _rebornProp.c_str());
		pb->setState(BS_Release);
		_reborn = false;
		//_rebornBubble = bubble;
		//_rebornProp = prop;
	}
}

void BZBubble::setRebornBubble(const char* bubble, const char* prop)
{
	_reborn = true;
	_rebornBubble = bubble;
	_rebornProp = prop;
}

void BZBubble::addEffect(const char* spr, const char* pose, bool bDeadEffect)
{
	GUARD_FUNCTION();
	_Assert(_pboard);

	CAStageLayer* player = _pboard->layer();

	BZSpriteCommon* pspr = new BZSpriteCommon(player, spr);
	CCPoint pos = getPos();
	_pboard->getBubbleRenderPos(pos);
	pspr->setPos(pos);
	pspr->setVertexZ(_zorder(LAYER_P_EFFECTS));
	float delay = CAUtils::Rand(0.02f, 0.3f);
	pspr->setAnamitionDelay(delay);
	pspr->pushState(pose);
	player->addSprite(pspr);

	if (bDeadEffect)
	{
		if (null != _psprDeadEffect) _psprDeadEffect->release();
		if (pspr) pspr->retain();
		_psprDeadEffect = pspr;
	}
}
