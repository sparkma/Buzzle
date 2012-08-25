
#include "BZBoard.h"
#include "BZGame.h"
#include "AStageLayer.h"
#include "AWorld.h"

#define DEFAULT_ACCELERATION (269.0f)

/// Game
BZBoard::BZBoard(BZGame* pgame)
{
	GUARD_FUNCTION();

	_pgame = pgame;
	
	_zorder = 0;
	_rows = -1;
	_cols = -1;

	memset(_bubblesGrabbed, 0, sizeof(_bubblesGrabbed));

	_blocksRunning = CCArray::create(40);
	_blocksRunning->retain();
	_blocksIdle = CCArray::create(40);
	_blocksIdle->retain();
	//_blocksWillBeRemoved = CCArray::create(40);
	//_blocksWillBeRemoved->retain();
}

BZBoard::~BZBoard()
{
	int r, c;
	for (r = 0; r < _rows; r++)
	{
		for (c = 0; c < _cols; c++)
		{
			BZBubble* pbubble = _getBubble(r, c);
			if (pbubble)
			{
				//reduce block counter
				pbubble->setBlock(null);
			
				_setBubble(r, c, null);
			}
		}
	}

	for (r = 0; r < _MAX_GRABBED_BLOCKS; r++)
	{
		_setGrabbedBubble(r, null);
	}

	_blocksIdle->release();
	_blocksIdle = null;

	_blocksRunning->release();
	_blocksRunning = null;
}

//for debugging
string BZBoard::debuglog()
{
	char sz[128];
	sprintf(sz, "blocks=%d(-%d),bubbles=%d", 
		_blocksRunning->count(), _blocksIdle->count(), this->getBubblesCount());
	return sz;
}

ccTime BZBoard::getTimeNow() const
{ 
	_Assert(_pgame);
	return _pgame->getTimeNow(); 
}

unsigned int BZBoard::getBubblesCount() const
{
	unsigned int n = 0;
	CAObject* pobj;
	CCARRAY_FOREACH(_blocksRunning, pobj)
	{
		BZBlock* pb = (BZBlock*)pobj;
		n += pb->getBubbles()->count();
	}
	return n;
}

void BZBoard::setParams(const CCPoint& ptLeftTop, 
						int rows, int cols, float bubblesize,
						float zorder)
{
	_rows = rows;
	_cols = cols;
	_zorder = zorder;

	_ptLeftTop = ptLeftTop;
	CAWorld::percent2view(_ptLeftTop);
	CAWorld::percent2view(bubblesize, true);
	_fBubbleSize = bubblesize;

#if defined(_DEBUG)
	memset(_bubblesInBoards, 0, sizeof(_bubblesInBoards));
#else
	_bubblesInBoards = new BZBubble* [rows * cols];
	memset(_bubblesInBoards, 0, sizeof(BZBubble*) * rows * cols);
#endif
}


#define _IS_IN_BOARD(_row_, _col_)	(((_row_) >= 0 && (_row_) < _rows) && ((_col_) >= 0 && (_col_) < _cols))

BZBubble* BZBoard::_getBubble(int r, int c) const
{ 
	if (_IS_IN_BOARD(r, c))
	{
#if defined(_DEBUG)
		_Assert(_rows <= 16 && _cols <= 16);
		return _bubblesInBoards[r][c]; 
#else
		return _bubblesInBoards[r * _cols + c]; 
#endif
	}
	//out of bounds, returns null
	return null;
}

void BZBoard::_setBubble(int r, int c, BZBubble* pbubble)
{
	_Assert(null == pbubble || null == _getBubble(r, c));
	if (pbubble) 
	{
		pbubble->retain();
	}
#if defined(_DEBUG)
	_Assert(_rows <= 32 && _cols <= 32);
	if (_bubblesInBoards[r][c]) 
	{
		_bubblesInBoards[r][c]->release();
	}
	_bubblesInBoards[r][c] = pbubble;
#else
	if (_bubblesInBoards[r * _cols + c]) 
	{
		_bubblesInBoards[r * _cols + c]->release();
	}
	_bubblesInBoards[r * _cols + c] = pbubble;
#endif
}

int BZBoard::getEmptyBornSlots(int* slots, int scount) const
{
	_Assert(_cols <= scount);
	memset(slots, 0, scount * sizeof(int));

	int freed = 0;
	int i;
	for (i = 0; i < _cols; i++)
	{
		if (null == _getBubble(0, i))
		{
			slots[freed++] = i;
		}
	}
	return freed;
}

bool BZBoard::verifyBubble(BZBubble* pbubble)
{
	return _getBubble(pbubble->getIndexRow(), pbubble->getIndexColumn()) == pbubble;
}

bool BZBoard::canFall(const BZBubble* pbubble) const
{
	int r = pbubble->getIndexRow();
	int c = pbubble->getIndexColumn();
	if (r == _rows - 1)
		return false;
	BZBubble* pbb = this->_getBubble(r + 1, c);
	return null == pbb;
}

bool BZBoard::canMove(const BZBubble* pbubble) const
{
	int n = 0;
	BZBubble* pn;
	int r = pbubble->getIndexRow();
	int c = pbubble->getIndexColumn();

	pn = _getBubble(r, c + 1);	if (null != pn || c == _cols - 1) n++;
	pn = _getBubble(r + 1, c);	if (null != pn || r == _rows - 1) n++; 
	pn = _getBubble(r, c - 1);	if (null != pn || c == 0) n++;
	pn = _getBubble(r - 1, c);	if (null != pn || r == 0) n++;

	return n != 4;
}

EBubbleBlockerType BZBoard::getBubbleBlocker(BZBubble* pbubble, CCPoint& pos)
{
	int r = pbubble->getIndexRow();
	int c = pbubble->getIndexColumn();

	if (!(r >= _rows - 1))
	{
		int i;
		for (i = r + 1; i < _rows; i++)
		{
			BZBubble* pbubbleBottom = _getBubble(i, c);
			if (null != pbubbleBottom)
			{
				pos.x = (float)c;
				pos.y = (float)i;
				EBubbleState s = pbubbleBottom->getState();
				if (pbubbleBottom->isStoped())
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
void BZBoard::_bp2sp(CCPoint& pos) const
{
	//CCSize size = CAWorld::sharedWorld().getScreenSize();
	pos.x *= this->_fBubbleSize;
	pos.y *= this->_fBubbleSize;
	pos.x += this->_ptLeftTop.x;
	pos.y = this->_ptLeftTop.y - pos.y;
}

//screen position to block position
void BZBoard::_sp2bp(CCPoint& pos) const
{
	pos.y = this->_ptLeftTop.y - pos.y;
	pos.x -= this->_ptLeftTop.x;
	pos.y /= this->_fBubbleSize;
	pos.x /= this->_fBubbleSize;
}

void BZBoard::verify()
{
#if defined(_DEBUG)
	CAObject* pobj;
	CCARRAY_FOREACH(_blocksRunning, pobj)
	{
		BZBlock* pb = (BZBlock*)pobj;
		pb->verify();
	}
#endif
}

void BZBoard::onBubblePositionChanged(BZBubble* pbubble, const CCPoint& posOld, const CCPoint& posNew)
{
	//update blocks
 	int r = _ROW(posNew.y);
	int c = _COL(posNew.x);

	_Assert(_IS_IN_BOARD(r, c));

	BZBubble* pb = _getBubble(r, c);
	_Assert(null == pb || pb == pbubble);
	if (null == pb)
	{
		//uodate block
		int oldr = _ROW(posOld.y);
		int oldc = _COL(posOld.x);
		if (_IS_IN_BOARD(oldr, oldc))
		{
			_Assert(pbubble == _getBubble(oldr, oldc));
			_setBubble(oldr, oldc, null);
		}
		_setBubble(r, c, pbubble);
	}
}

BZBlock* BZBoard::_newBlockHolder(BZBubble* pbubble)
{
	//new a block for this bubble
	if (_blocksIdle->count() <= 0)
	{
		_blocksIdle->addObject(new BZBlock(this));
		_blocksIdle->addObject(new BZBlock(this));
		_blocksIdle->addObject(new BZBlock(this));
		_blocksIdle->addObject(new BZBlock(this));
	}
	BZBlock* pblock = (BZBlock*)_blocksIdle->objectAtIndex(0);
	pblock->reset();
	pblock->retain();
	{
		_blocksRunning->addObject(pblock);
		_blocksIdle->removeObjectAtIndex(0);
		pblock->attachBubble(pbubble);
	}
	pblock->release();

	return pblock;
}

void BZBoard::_doBlockBlend(BZBubble* pbubble)
{
	_Trace("bubble #%02d block blending", pbubble->debug_id());

	int r = pbubble->getIndexRow();
	int c = pbubble->getIndexColumn();
	//test top
	int				dr[4] = { 0,	+1,			0,			-1};
	int				dc[4] = {-1,	0,			+1,			0};
	EBubbleNeighbour	nb[4] = {N_LEFT,N_BOTTOM,	N_RIGHT,	N_TOP};
	BZBubble*		pbubbleNeighbour;

	const string& type = pbubble->getBubbleType();
	int i;
	for (i = 0; i < 4; i++)
	{
		pbubbleNeighbour = this->_getBubble(r + dr[i], c + dc[i]);
		if (null != pbubbleNeighbour)
		{	
			EBubbleState s = pbubbleNeighbour->getState();
			if (s >= BS_Stop && s <= BS_Standing)
			{
				const string& bntype = pbubbleNeighbour->getBubbleType();
				if (type == bntype)
				{ 
					//combine pbubble->getBlock and pbubbleNeighbour->getBlock
					BZBlock* pb = pbubble->getBlock();
					BZBlock* pbn= pbubbleNeighbour->getBlock();
					if (pb->debug_id() != pbn->debug_id())
					{
						pb->append(pbn);
					}
					if (s >= BS_PoseBlend)
					{
						pbubbleNeighbour->setState(BS_PoseBlend);
					}
				}
			}
		}
	}
}

void BZBoard::_doPoseBlend(BZBubble* pbubble)
{
	int r = pbubble->getIndexRow();
	int c = pbubble->getIndexColumn();
	//test top
	int					dr[4] = {0,			+1,		 0,		-1};
	int					dc[4] = {+1,		0,			-1,		0};
	EBubbleNeighbour	nb[4] = {N_RIGHT,	N_BOTTOM,	N_LEFT,	N_TOP};
	BZBubble*			pbubbleNeighbour;

	BZBlock* pblock = pbubble->getBlock();
	const string& type = pbubble->getBubbleType();

	string pose = "";
	int i;
	for (i = 0; i < 4; i++)
	{
		pbubbleNeighbour = this->_getBubble(r + dr[i], c + dc[i]);
		if (null != pbubbleNeighbour && pblock == pbubbleNeighbour->getBlock())
		{	
			_Assert(pbubbleNeighbour->getBubbleType() == type);
			pose += "o";
			//if (pbubbleNeighbour->getState() > BS_PoseBlend)
			//{
			//	pbubbleNeighbour->setState(BS_PoseBlend);
			//}
		}
		else
		{
			pose += "x";
		}
	}
	_Trace("bubble #%02d change pose to %s", pbubble->debug_id(), pose.c_str());
	pbubble->setPose(pose);
}

void BZBoard::_doLeaveBlock(BZBubble* pbubble)
{
	BZBlock* pblock = pbubble->getBlock();
	CCArray* pbubbles = pblock->getBubbles();
	unsigned i, c = pbubbles->count();
	if (c == 1)
		return;
	CCArray* pbubblesDetached = CCArray::create(c);
	for (i = 0; i < c; i++)
	{
		BZBubble* pb = (BZBubble*)pbubbles->objectAtIndex(i);
		pbubblesDetached->addObject(pb);
		if (pb != pbubble)
		{
			//others
			pb->setState(BS_BlockBlend);
		}
	}
	c = pbubblesDetached->count();
	for (i = 0; i < c; i++)
	{
		BZBubble* pb = (BZBubble*)pbubblesDetached->objectAtIndex(i);
		pblock->detachBubble(pb);
		_newBlockHolder(pb);
	}
	pbubblesDetached->release();
}

void BZBoard::onBubbleStateChanged(BZBubble* pbubble, EBubbleState state)
{
	GUARD_FUNCTION();

	switch (state)
	{
	case BS_NA:
	case BS_NAing:
	case BS_Born:
	case BS_Borning:
		break;
	case BS_Fall:
	case BS_Drag:
		_doLeaveBlock(pbubble);
		break;
	case BS_Dragging:
		break;
	case BS_Release:
		break;
	case BS_Falling:
		//create some sparks here?
		break;
	case BS_Stop:
		break;
	case BS_Stopping:
		break;
	case BS_BlockBlend:
		break;
	case BS_BlockBlending:
		_doBlockBlend(pbubble);
		break;
	case BS_PoseBlend:
		break;
	case BS_PoseBlending:
		_doPoseBlend(pbubble);
		break;
	case BS_Stand:
		break;
	case BS_Standing:
		break;
	case BS_Die:
		break;
	case BS_Dying:
		break;
	case BS_Died:
		{
			//do not release bubble here, we are in block::onUpdate loop
			BZBlock* pblock = pbubble->getBlock();
			_Trace("bubble #%02d (%d,%d) is died, leave block #%02d", 
				pbubble->debug_id(), 
				pbubble->getIndexRow(), pbubble->getIndexColumn(),
				pblock->debug_id());
			pbubble->setState(BS_NA);
			pblock->detachBubble(pbubble); //ref = 1
			_setBubble(pbubble->getIndexRow(), pbubble->getIndexColumn(), null); //ref = 0

			//mv from onBlockUpdate
			pbubble->detach(_pgame->layer());
		}
		break;
	default:
		_Assert(false);
		break;
	}
}

BZBubble* BZBoard::createBubble(int row, int col, const char* bubble, const char* prop, const char* doodad)
{
	GUARD_FUNCTION();

	BZBubble* pb = new BZBubble(this);
	pb->initialize(bubble, prop, doodad, _zorder);

	_newBlockHolder(pb);

	pb->setInitialPosition(ccp(col, row));
	pb->setState(BS_Born);

	return pb;
}

BZBubble* BZBoard::_getGrabbedBubble(int finger)
{
	if (finger >= 0 && finger < _MAX_GRABBED_BLOCKS)
	{
		return _bubblesGrabbed[finger];
	}
	return null;
}

void BZBoard::_setGrabbedBubble(int finger, BZBubble* pbubble)
{
	if (finger >= 0 && finger < _MAX_GRABBED_BLOCKS)
	{
	}
	else
	{
		return;
	}
	if (pbubble)
	{
		_Assert(null == _bubblesGrabbed[finger]);
		_bubblesGrabbed[finger] = pbubble;
		pbubble->retain();
	}
	else
	{
		if (null != _bubblesGrabbed[finger])
		{
			_bubblesGrabbed[finger]->release();
			_bubblesGrabbed[finger] = null;
		}
	}
}

BZBubble* BZBoard::_getBubbleByPoint(const CCPoint& pos)
{
	CCPoint p = pos;
	_sp2bp(p);
	int r = _ROW(p.y);
	int c = _COL(p.x);
	//_Trace("trying find block at %d,%d", r, c);
	//_getBubble will check bounds
	return _getBubble(r, c);
}

void BZBoard::_onTouchGrabbed(CAEventTouch* ptouch)
{
	BZBubble* pbubble = _getBubbleByPoint(ptouch->pt());
	//block could be null
	if (null != pbubble)
	{
		if (pbubble->canMove())
		{
			_Trace("bubble #%02d (%d,%d) is grabbed", pbubble->debug_id(),
				pbubble->getIndexRow(), pbubble->getIndexColumn());
			pbubble->setState(BS_Drag);
		}
		_setGrabbedBubble(ptouch->fingler(), pbubble);
	}
}

void BZBoard::_onTouchMoving(CAEventTouch* ptouch)
{
	//find if this finger has grabbed a block?
	BZBubble* pbubble = _getGrabbedBubble(ptouch->fingler());
	if (null == pbubble)
	{
		//we can try grab another one in moving state
		//_onTouchGrabbed(ptouch);
	}
	else
	{
		EBubbleState s = pbubble->getState();
		if (BS_Dragging != s)
			return;

		//move the grabbed block
		CCPoint pos = ptouch->pt();
		_sp2bp(pos);
		int r = _ROW(pos.y);
		int c = _COL(pos.x);
		if (_IS_IN_BOARD(r, c))
		{
			BZBubble* pbHere = _getBubble(r, c);
			if (null == pbHere || pbubble == pbHere)
			{
				pbubble->setDraggingPos(pos);
			}
		}
	}
}

void BZBoard::_onTouchUngrabbed(CAEventTouch* ptouch)
{
	BZBubble* pbubble = _getGrabbedBubble(ptouch->fingler());
	if (pbubble)
	{
		BZBubble* pbubbleThisPos = _getBubbleByPoint(ptouch->pt());
		if (pbubble == pbubbleThisPos)
		{
			_pgame->onBubbleClicked(pbubble);
		}
		EBubbleState s = pbubble->getState();
		if (BS_Dragging == s)
		{
			pbubble->setState(BS_Fall);
		}
		_setGrabbedBubble(ptouch->fingler(), null);
	}
}

void BZBoard::onEvent(CAEvent* pevt)
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

void BZBoard::onEnter()
{
	//do nothing
}

void BZBoard::_onUpdateBlock(BZBlock* pblock)
{
	if (_pgame->canBoom(pblock))
	{
		pblock->booom();
	}
}

void BZBoard::onUpdate()
{
	int r, c;
	for (r = 0; r < _rows; r++)
	{
		for (c = 0; c < _cols; c++)
		{
			BZBubble* pbubble = _getBubble(r, c);
			if (null != pbubble)
			{
				pbubble->onUpdate();
			}
		}
	}

	//clean up unused-blocks here
	CAObject* pobj;
	CCARRAY_FOREACH(_blocksRunning, pobj)
	{
		BZBlock* pb = (BZBlock*)pobj;
		unsigned int ref = pb->retainCount();
		_Assert(ref >= 1);
		if (ref > 1)
		{
			//this is a working block
			_onUpdateBlock(pb);
		}
		ref = pb->retainCount();
		if (ref == 1)
		{
			//this block can be recycled.
			_Assert(pb->getBubbles()->count() == 0);
			_Assert(pb->getStars() == 0);
			_blocksIdle->addObject(pb);
		}
	}

	//transf idle blocks
	CCARRAY_FOREACH(_blocksIdle, pobj)
	{
		BZBlock* pb = (BZBlock*)pobj;
		_blocksRunning->removeObject(pb);
	}
}

void BZBoard::onExit()
{
	//remove all blocks
}
