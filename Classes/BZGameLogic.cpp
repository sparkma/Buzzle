
#include "BZGameLogic.h"
#include "AStageLayer.h"
#include "AWorld.h"

#define _FALLING_DX_	(1.0f / 20.0f)
#define _FALLING_DELAY	(0.2f)
#define DEFAULT_ACCELERATION (16.70f)

///Block
BZBlock::BZBlock(BZGame* pgame)
{
	_pgame = pgame;
	_pgroup = null;
	_acceleration = DEFAULT_ACCELERATION;
	_setState(BS_NA);

	_row = _col = -1;

	memset(_neighbours, 0, sizeof(_neighbours));

	autorelease();
}

BZBlock::~BZBlock()
{
}

void BZBlock::setFallingAcceleration(float a)
{
	_acceleration = a;
}

void BZBlock::_setState(EBlockState s)
{
	_timeStateBegin = _pgame->getTimeNow();
	_state = s;
	_pgame->onBlockStateChanged(this, _state);
}

void BZBlock::setBlock(const char* block)
{
	GUARD_FUNCTION();
	_type = block;
	CASprite* pspr = new BZSpriteCommon(_pgame->layer(), block);
	pspr->setState("na");
	_psprBlock = pspr;
}

void BZBlock::setNeighbour(EBlockNeighbour bn, BZBlock* pblock)
{
	BZBlock* pn;

	if (_neighbours[bn] == pblock)
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
		pn->setNeighbour((EBlockNeighbour)((bn + 2) % 4), null);
	}
	_neighbours[bn] = pblock;
	if (pblock) pblock->retain();
	if (pblock) pblock->setNeighbour((EBlockNeighbour)((bn + 2) % 4), this);

	//change pose here
	string pose = "";
	int dir = 0;


	pn = _neighbours[N_RIGHT];	if (null != pn && pn->getType() == _type) 	pose += "o"; else pose += "x";
	pn = _neighbours[N_BOTTOM]; if (null != pn && pn->getType() == _type) 	pose += "o"; else pose += "x";
	pn = _neighbours[N_LEFT];	if (null != pn && pn->getType() == _type) 	pose += "o"; else pose += "x";
	pn = _neighbours[N_TOP];	if (null != pn && pn->getType() == _type) 	pose += "o"; else pose += "x";
	
	_psprBlock->switchPose(pose);
}

void BZBlock::_setPos(float x, float y)
{
	_Assert(_pgame);

	_pos.x = x;
	_pos.y = y;
	
	//this will refine board in game
	_pgame->onBlockPositionChanged(this, _pos);
	
	_row = (int)(y + 0.5f);
	_col = (int)(x + 0.5f);

	_Assert(_pgame->verifyBlock(this));
}

void BZBlock::onEnter()
{
	_Assert(_pgame);
	_Assert(_pgame->layer());
	_Assert(_psprBlock);

	_pgame->layer()->addSprite(_psprBlock);
}

void BZBlock::onUpdate()
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
		_psprBlock->setState("born");
		_setState(BS_Borning);
		break;
	case BS_Borning:
		if (_psprBlock->isAnimationDone())
		{
			_setState(BS_Fall);
		}
		break;
	case BS_Fall: 
		//delay for _FALLING_DELAY(0.2f) sec before falling
		if (_pgame->getTimeNow() - _timeStateBegin > _FALLING_DELAY)
		{
			_setState(BS_Falling);
			_lastFallingTime = _pgame->getTimeNow();
			_fallingspeed = 0;
		}
		break;
	case BS_Falling:
		{
			CCPoint posOld = _pos;
			CCPoint posNew = _pos;

			float t = _pgame->getTimeNow() - _lastFallingTime;
			//falling time is eaten, even this move may fail.
			_lastFallingTime = _pgame->getTimeNow();

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
				_Assert(posNew.x - f < 0.5f); 
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
			EBlockerType bt = _pgame->getBlocker(this, posChair);
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
			posNew.x = (float)((int)(posNew.x + 0.5f));
			posNew.y = (float)((int)(posNew.y + 0.5f));
			_setPos(posNew.x, posNew.y);
			//_psprBlock->setState("stop");
		}
		break;
	case BS_Stopping:
		//calcualte blending here
		if (_pgame->getTimeNow() - _timeStateBegin > 0.3f)
			//&& 
			//_psprBlock->isAnimationDone())
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
		if (null == this->_neighbours[N_BOTTOM] && _row != _pgame->getRows() - 1)
		{
			_setState(BS_Fall);
		}
		break;
	case BS_Die:
		_setState(BS_Dying);
		//_psprBlock->setState("dead");
		break;
	case BS_Dying:
		//do nothing, "dead" is the deadpose
		//and _psprBlock is BZSpriteCommon
		_setState(BS_Died);
		break;
	case BS_Died:
		break;
	default:
		_Assert(false);
		break;
	}

	CCPoint pt = _pos;
	_pgame->getBlockRenderPos(pt);

	_psprBlock->setZOrder(50.0f);
	//_psprBlock->setScale(10.0f);
	_psprBlock->setPos(pt);
}

void BZBlock::onExit()
{
	this->setNeighbour(N_TOP, null);
	this->setNeighbour(N_LEFT, null);
	this->setNeighbour(N_BOTTOM, null);
	this->setNeighbour(N_RIGHT, null);

	_Assert(_pgame);
	_Assert(_pgame->layer());
	_Assert(_psprBlock);

	_pgame->layer()->removeSprite(_psprBlock);
}


/// Game
BZGame::BZGame(CAStageLayer* player)
{
	GUARD_FUNCTION();

	_pLayer = player;
	
	_rows = -1;
	_cols = -1;

	_nScores = 0;
	_timeLastBorn = 0;

	memset(_blocksGrabbed, 0, sizeof(_blocksGrabbed));

	_blocks = CCArray::arrayWithCapacity(100);
	_blocks->retain();
	_groups = CCArray::arrayWithCapacity(40);
	_groups->retain();
	_psprDoodads = CCArray::arrayWithCapacity(20);
	_psprDoodads->retain();
}

BZGame::~BZGame()
{
	int r, c;
	for (r = 0; r < _rows; r++)
	{
		for (c = 0; c < _cols; c++)
		{
			_setBlock(r, c, null);
		}
	}

	for (r = 0; r < _MAX_GRABBED_BLOCKS; r++)
	{
		_setGrabbedBlock(r, null);
	}

	CAObject* pobj;
	CCARRAY_FOREACH(_blocks, pobj)
	{
		BZBlock* pb = (BZBlock*)pobj;
		pb->onExit();
	}

	_blocks->release();
	_blocks = null;
	_psprDoodads->release();
	_psprDoodads = null;
	_groups->release();
	_groups = null;
}

void BZGame::setParams(const CCPoint& ptLeftTop, int rows, int cols, float blocksize)
{
	_rows = rows;
	_cols = cols;

	_ptLeftTop = ptLeftTop;
	CAWorld::percent2view(_ptLeftTop);
	CAWorld::percent2view(blocksize, true);
	_blockSize = blocksize;

#if defined(_DEBUG)
	memset(_blocksInBoards, 0, sizeof(_blocksInBoards));
#else
	_blocksInBoards = new BZBlock* [rows * cols];
	memset(_blocksInBoards, 0, sizeof(BZBlock*) * rows * cols);
#endif
}

float BZGame::getTimeNow()
{ 
	return _pLayer->getTimeNow(); 
}

void BZGame::onEnter()
{
	_timeLastBorn = 0;

	_params._fDelayBlockBorn = 0.2f;
	_params._fDelayStar = 30.0f;
	_params._fRangeblockBorn = 3.0f;

	//later, we will load this from xml
	int n = 0;
	_types[n++] = "yellow";
	_types[n++] = "pink";
	_types[n++] = "pred";
}

BZBlock* BZGame::_createUnmanagedBlock(const char* type)
{
	GUARD_FUNCTION();
	string name = type;
	name += "_block";
	BZBlock* pblock = new BZBlock(this);
	pblock->setBlock(name.c_str());
	pblock->setFallingAcceleration(DEFAULT_ACCELERATION);
	pblock->setState(BS_Born);

	return pblock;
}

#define _IS_IN_BOARD(_row_, _col_)	(((_row_) >= 0 && (_row_) < _rows) && ((_col_) >= 0 && (_col_) < _cols))

BZBlock* BZGame::_getBlock(int r, int c) 
{ 
	if (_IS_IN_BOARD(r, c))
	{
#if defined(_DEBUG)
		_Assert(_rows <= 32 && _cols <= 32);
		return _blocksInBoards[r][c]; 
#else
		return _blocksInBoards[r * _cols + c]; 
#endif
	}
	//out of bounds, returns null
	return null;
}

void BZGame::_setBlock(int r, int c, BZBlock* pblock)
{
	_Assert(null == pblock || null == _getBlock(r, c));
	if (pblock) pblock->retain();
#if defined(_DEBUG)
	_Assert(_rows <= 32 && _cols <= 32);
	if (_blocksInBoards[r][c]) _blocksInBoards[r][c]->release();
	_blocksInBoards[r][c] = pblock;
#else
	if (_blocksInBoards[r * _cols + c]) _blocksInBoards[r * _cols + c]->release();
	_blocksInBoards[r * _cols + c] = pblock;
#endif
}

#if 0
//we can refine posNew here, and we can stop this block here
bool BZGame::modifyBlockPosition(BZBlock* pblock, 
								 const CCPoint& posOld, CCPoint& posNew)
{
	//do not fall through the block under me
 	int r = (int)(posNew.y + 0.5f);
	int c = (int)(posNew.x + 0.5f);
	if (_IS_IN_BOARD(r, c))
	{
	}
	else
	{
		return false;
	}

	BZBlock* pb = _getBlock(r, c);
	if (pblock == pb)
	{
		return true;
	}
	if (null == pb)
	{
		//uodate block
		int or = pblock->getIndexRow();
		int oc = pblock->getIndexColumn();
		if (_IS_IN_BOARD(or, oc))
			_setBlock(or, oc, null);
		_setBlock(r, c, pblock);
		return true;
	}
	//there is another block at (r, c)
	return false;
}
#endif

bool BZGame::verifyBlock(BZBlock* pblock)
{
	return _getBlock(pblock->getIndexRow(), pblock->getIndexColumn()) == pblock;
}

EBlockerType BZGame::getBlocker(BZBlock* pblock, CCPoint& pos)
{
	int r = pblock->getIndexRow();
	int c = pblock->getIndexColumn();

	if (!(r >= _rows - 1))
	{
		int i;
		for (i = r + 1; i < _rows; i++)
		{
			BZBlock* pblockBottom = _getBlock(i, c);
			if (null != pblockBottom)
			{
				pos.x = (float)c;
				pos.y = (float)i;
				if (BS_Standing == pblockBottom->getState())
				{
					return BT_StoppingBlock;
				}
				else
				{
					return BT_FallingBlock;
				}
			}
		}
	}

	//bottom
	pos.x = (float)c;
	pos.y = (float)_rows; //NOT _rows - 1! over bottom ONE block
	return BT_Bottom;
}

//block position to screen position
void BZGame::_bp2sp(CCPoint& pos) const
{
	//CCSize size = CAWorld::sharedWorld().getScreenSize();
	pos.x *= this->_blockSize;
	pos.y *= this->_blockSize;
	pos.x += this->_ptLeftTop.x;
	pos.y = this->_ptLeftTop.y - pos.y;
}

//screen position to block position
void BZGame::_sp2bp(CCPoint& pos) const
{
	pos.y = this->_ptLeftTop.y - pos.y;
	pos.x -= this->_ptLeftTop.x;
	pos.y /= this->_blockSize;
	pos.x /= this->_blockSize;
}

void BZGame::onBlockPositionChanged(BZBlock* pblock, const CCPoint& pos)
{
	//update blocks
 	int r = (int)(pos.y + 0.5f);
	int c = (int)(pos.x + 0.5f);

	_Assert(_IS_IN_BOARD(r, c));

	BZBlock* pb = _getBlock(r, c);
	_Assert(null == pb || pb == pblock);
	if (null == pb)
	{
		//uodate block
		int or = pblock->getIndexRow();
		int oc = pblock->getIndexColumn();
		if (_IS_IN_BOARD(or, oc))
		{
			_setBlock(or, oc, null);
		}
		_setBlock(r, c, pblock);
	}
}

void BZGame::onBlockStateChanged(BZBlock* pblock, EBlockState state)
{
	switch (state)
	{
	case BS_Die:
	case BS_Drag:
	case BS_Fall:
		//remove neighbours of me
		pblock->setNeighbour(N_TOP, null);
		pblock->setNeighbour(N_LEFT, null);
		pblock->setNeighbour(N_BOTTOM, null);
		pblock->setNeighbour(N_RIGHT, null);
		break;
	case BS_Died:
		pblock->onExit();
		_blocks->removeObject(pblock);
		break;
	case BS_Standing:
		//find neighbours here, and blend with them
		{
			int r = pblock->getIndexRow();
			int c = pblock->getIndexColumn();
			//test top
			int				dr[4] = { 0,	+1,			0,			-1};
			int				dc[4] = {-1,	0,			+1,			0};
			EBlockNeighbour	nb[4] = {N_LEFT,N_BOTTOM,	N_RIGHT,	N_TOP};
			BZBlock*		pblockNeighbour;
			int i;
			for (i = 0; i < 4; i++)
			{
				pblockNeighbour = this->_getBlock(r + dr[i], c + dc[i]);
				if (null != pblockNeighbour && BS_Standing == pblockNeighbour->getState())
				{	
					pblock->setNeighbour(nb[i], pblockNeighbour);
				}
			}
		}
		break;
	}
}

void BZGame::_doBornStrategy()
{
#if defined(_DEBUG) && 0
	static int blocks = 0;
	if (blocks < 4) 
		;
	else
		return;
	ccTime time = _pLayer->getTimeNow();
	if (time - _timeLastBorn > _params._fDelayBlockBorn)
	{
		string type = _types[0];

		BZBlock* pblock = _createUnmanagedBlock(type.c_str());

		int slots[] = {6, 7, 8, 7};
		int slot = slots[blocks];

		_setBlock(0, slot, pblock);
		pblock->setInitialPosition(ccp(slot, 0));

		pblock->onUpdate();	//update real position of this block
		pblock->attachTo(_pLayer);
	
		blocks++;
	}
	return;
#else
	ccTime time = _pLayer->getTimeNow();
	if (time - _timeLastBorn > _params._fDelayBlockBorn && 
		_blocks->count() < 5 * _cols)
	{
		_timeLastBorn = time;

		//select column first
		int typ = (int)CAUtils::Rand(0, (float)_params._fRangeblockBorn);
		_Assert(typ >= 0 && typ < BLOCK_TYPES);
		string type = _types[typ];

		//how many free slots 
		int free = 0;
		int slots[64];
		_Assert(_cols <= 64);

		memset(slots, 0, sizeof(slots));
		int i;
		for (i = 0; i < _cols; i++)
		{
			if (null == _getBlock(0, i))
			{
				slots[free++] = i;
			}
		}

		//create a block if there is a free slot
		if (free > 0)
		{
			//rand a slot
			int rand = (int)CAUtils::Rand(0, (float)free);
			int slot = slots[rand];

			BZBlock* pblock = _createUnmanagedBlock(type.c_str());
			_blocks->addObject(pblock);

			_setBlock(0, slot, pblock);
			pblock->setInitialPosition(ccp(slot, 0));

			pblock->onEnter();
			//pblock->onUpdate();	//update real position of this block
		}
	}
#endif
}


void BZGame::_doBlocksUpdate()
{
	int i;

	for (i = 0; i < _rows * _cols; i++)
	{
		int r = i / _cols;
		int c = i % _cols;
		BZBlock* pblock = _getBlock(r, c);
		if (pblock)
		{
			pblock->onUpdate();
		}
	}
}

void BZGame::onUpdate()
{
	_doBornStrategy();
	_doBlocksUpdate();
}

BZBlock* BZGame::_getGrabbedBlock(int finger)
{
	if (finger >= 0 && finger < _MAX_GRABBED_BLOCKS)
	{
		return _blocksGrabbed[finger];
	}
	return null;
}

void BZGame::_setGrabbedBlock(int finger, BZBlock* pblock)
{
	if (finger >= 0 && finger < _MAX_GRABBED_BLOCKS)
	{
	}
	else
	{
		return;
	}
	if (pblock)
	{
		_Assert(null == _blocksGrabbed[finger]);
		_blocksGrabbed[finger] = pblock;
		pblock->retain();
	}
	else
	{
		if (null != _blocksGrabbed[finger])
		{
			_blocksGrabbed[finger]->release();
			_blocksGrabbed[finger] = null;
		}
	}
}

BZBlock* BZGame::_getBlockByPoint(const CCPoint& pos)
{
	CCPoint p = pos;
	_sp2bp(p);
	int r = (int)pos.y;
	int c = (int)pos.x;
	//_getBlock will check bounds
	return _getBlock(r, c);
}

void BZGame::_onTouchGrabbed(CAEventTouch* ptouch)
{
	BZBlock* pblock = _getBlockByPoint(ptouch->pt());
	//block could be null
	if (null != pblock)
	{
		pblock->setState(BS_Drag);
		_setGrabbedBlock(ptouch->fingler(), pblock);
	}
}

void BZGame::_onTouchMoving(CAEventTouch* ptouch)
{
	//find if this finger has grabbed a block?
	BZBlock* pblock = _getGrabbedBlock(ptouch->fingler());
	if (null == pblock)
	{
		//we can try grab another one in moving state
		_onTouchGrabbed(ptouch);
	}
	else
	{
		//move the grabbed block
		CCPoint pos = ptouch->pt();
		_sp2bp(pos);
		pblock->setDraggingPos(pos);
	}
}

void BZGame::_onTouchUngrabbed(CAEventTouch* ptouch)
{
	BZBlock* pblock = _getGrabbedBlock(ptouch->fingler());
	if (pblock)
	{
		pblock->setState(BS_Fall);
		_setGrabbedBlock(ptouch->fingler(), null);
	}
}

void BZGame::onEvent(CAEvent* pevt)
{
	switch (pevt->type())
	{
	case ET_Touch:
		{
			CAEventTouch* ptouch = (CAEventTouch*)pevt;
			_Assert(ptouch->fingler() >= 0 && ptouch->fingler() < 5);
			switch (ptouch->state())
			{
			case kTouchStateGrabbed:
				_onTouchGrabbed(ptouch);
				break;
			case kTouchStateMoving:
				_onTouchMoving(ptouch);
				break;
			case kTouchStateUngrabbed:
				_onTouchUngrabbed(ptouch);
				break;
			}
		}
		break;
	case ET_Command:
		{
		}
		break;
	}
}

void BZGame::onExit()
{
}

BZGameClassic::BZGameClassic(CAStageLayer* player)
: BZGame(player)
{
}

BZGameClassic::~BZGameClassic()
{
}
