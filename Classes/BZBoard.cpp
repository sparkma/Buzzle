
#include "BZBoard.h"
#include "BZGame.h"
#include "AStageLayer.h"
#include "AWorld.h"

#define DEFAULT_ACCELERATION (69.0f)

/// Game
BZBoard::BZBoard(BZGame* pgame)
{
	GUARD_FUNCTION();

	_pgame = pgame;
	
	_rows = -1;
	_cols = -1;

	memset(_bubblesGrabbed, 0, sizeof(_bubblesGrabbed));

	_blocksRunning = CCArray::arrayWithCapacity(40);
	_blocksRunning->retain();
	_blocksWillBeAdded = CCArray::arrayWithCapacity(40);
	_blocksWillBeAdded->retain();
	_blocksWillBeRemoved = CCArray::arrayWithCapacity(40);
	_blocksWillBeRemoved->retain();
}

BZBoard::~BZBoard()
{
	int r, c;
	for (r = 0; r < _rows; r++)
	{
		for (c = 0; c < _cols; c++)
		{
			_setBubble(r, c, null);
		}
	}

	for (r = 0; r < _MAX_GRABBED_BLOCKS; r++)
	{
		_setGrabbedBubble(r, null);
	}

	_removeBlocks();
	_addBlocks();

	_blocksWillBeAdded->release();
	_blocksWillBeAdded = null;
	_blocksWillBeRemoved->release();
	_blocksWillBeRemoved = null;

	_blocksRunning->release();
	_blocksRunning = null;
}

void BZBoard::_addBlock(BZBlock* pblock)
{
	_blocksWillBeAdded->addObject(pblock);
}

void BZBoard::_addBlocks()
{
	CAObject* pobj;
	CCARRAY_FOREACH(_blocksWillBeAdded, pobj)
	{
		BZBlock* pb = (BZBlock*)pobj;
		_blocksRunning->addObject(pb);
	}
	_blocksWillBeAdded->removeAllObjects();
}

void BZBoard::_removeBlock(BZBlock* pblock)
{
	_blocksWillBeRemoved->addObject(pblock);
}

void BZBoard::_removeBlocks()
{
	CAObject* pobj;
	CCARRAY_FOREACH(_blocksWillBeRemoved, pobj)
	{
		BZBlock* pb = (BZBlock*)pobj;
		_blocksRunning->removeObject(pb);
	}
	_blocksWillBeRemoved->removeAllObjects();
}

//for debugging
string BZBoard::debuglog()
{
	char sz[128];
	sprintf(sz, "blocks=%d(-%d,+%d),bubbles=%d", 
		_blocksRunning->count(), _blocksWillBeRemoved->count(), _blocksWillBeAdded->count(),
		this->getBubblesCount());
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
						int rows, int cols, float bubblesize)
{
	_rows = rows;
	_cols = cols;

	_ptLeftTop = ptLeftTop;
	CAWorld::percent2view(_ptLeftTop);
	CAWorld::percent2view(bubblesize, true);
	_fBubbleSize = bubblesize;

#if defined(_DEBUG)
	memset(_bubblesInBoards, 0, sizeof(_bubblesInBoards));
#else
	_bubblesInBoards = new BZBlockBubble* [rows * cols];
	memset(_bubblesInBoards, 0, sizeof(BZBlockBubble*) * rows * cols);
#endif
}


#define _IS_IN_BOARD(_row_, _col_)	(((_row_) >= 0 && (_row_) < _rows) && ((_col_) >= 0 && (_col_) < _cols))

BZBlockBubble* BZBoard::_getBubble(int r, int c) const
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

void BZBoard::_setBubble(int r, int c, BZBlockBubble* pbubble)
{
	_Assert(null == pbubble || null == _getBubble(r, c));
	if (pbubble) pbubble->retain();
#if defined(_DEBUG)
	_Assert(_rows <= 32 && _cols <= 32);
	if (_bubblesInBoards[r][c]) _bubblesInBoards[r][c]->release();
	_bubblesInBoards[r][c] = pbubble;
#else
	if (_bubblesInBoards[r * _cols + c]) _bubblesInBoards[r * _cols + c]->release();
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

bool BZBoard::verifyBubble(BZBlockBubble* pbubble)
{
	return _getBubble(pbubble->getIndexRow(), pbubble->getIndexColumn()) == pbubble;
}

EBubbleBlockerType BZBoard::getBubbleBlocker(BZBlockBubble* pbubble, CCPoint& pos)
{
	int r = pbubble->getIndexRow();
	int c = pbubble->getIndexColumn();

	if (!(r >= _rows - 1))
	{
		int i;
		for (i = r + 1; i < _rows; i++)
		{
			BZBlockBubble* pblockBottom = _getBubble(i, c);
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

void BZBoard::onBubblePositionChanged(BZBlockBubble* pbubble, const CCPoint& posOld, const CCPoint& posNew)
{
	//update blocks
 	int r = _ROW(posNew.y);
	int c = _COL(posNew.x);

	_Assert(_IS_IN_BOARD(r, c));

	BZBlockBubble* pb = _getBubble(r, c);
	_Assert(null == pb || pb == pbubble);
	if (null == pb)
	{
		//uodate block
		int or = _ROW(posOld.y);
		int oc = _COL(posOld.x);
		if (_IS_IN_BOARD(or, oc))
		{
			_Assert(pbubble == _getBubble(or, oc));
			_setBubble(or, oc, null);
		}
		_setBubble(r, c, pbubble);
	}
}

void BZBoard::onBlockStateChanged(BZBlock* pblock)
{
	EBlockState s = pblock->getState();
	switch (s)
	{
	case Block_Running:
		if (pblock->getBubbles()->count() <= 0)
		{
			_Debug("block #%02d removed. C (%p)", pblock->debug_id(), pblock);
			_removeBlock(pblock);
		}
		else
		{
			if (pblock->getStars() >= 2)
			{
				pblock->booom();
			}
		}
		break;

	case Block_Boooming:
		if (pblock->getBubbles()->count() <= 0)
		{
			_Debug("block #%02d removed. C (%p)", pblock->debug_id(), pblock);
			_removeBlock(pblock);
		}
		break;

	case Block_Died:
		{
			break;
			CCArray* pbubbles_ = pblock->getBubbles();
			
			//CCArray* pbubbles = CCArray::arrayWithArray(pbubbles_); not working BZBlockBubble can not be copied.
			CCArray* pbubbles = CCArray::arrayWithCapacity(pbubbles_->count());
			
			CAObject* pobj;
			CCARRAY_FOREACH(pbubbles_, pobj)
			{
				pbubbles->addObject(pobj);
			}

			CCARRAY_FOREACH(pbubbles, pobj)
			{
				BZBlockBubble* pb = (BZBlockBubble*)pobj;
				_Assert(BS_Died == pb->getState());

				pblock->detachBubble(pb);
				pb->detach(_pgame->layer());

				_setBubble(pb->getIndexRow(), pb->getIndexColumn(), null);
			}
			//_removeBlock(pblock);
		}
		break;
	}
}

void BZBoard::onBubbleStateChanged(BZBlockBubble* pbubble, EBubbleState state)
{
	switch (state)
	{
	case BS_Die:
	case BS_Drag:
		{
			//remove neighbours of me
			pbubble->setNeighbour(N_TOP, null);
			pbubble->setNeighbour(N_LEFT, null);
			pbubble->setNeighbour(N_BOTTOM, null);
			pbubble->setNeighbour(N_RIGHT, null);
		}
		break;
	case BS_Fall:
		{
			//remove neighbours of me
			pbubble->setNeighbour(N_TOP, null);
			pbubble->setNeighbour(N_LEFT, null);
			pbubble->setNeighbour(N_BOTTOM, null);
			pbubble->setNeighbour(N_RIGHT, null);
			
			BZBlock* pbold = pbubble->getBlock();
			if (pbold->getBubbles()->count() > 1)
			{
				//new a block for this bubble
				BZBlock* pblock = new BZBlock(this);
				pbold->detachBubble(pbubble);
				_Assert(pbold->getBubbles()->count() > 0);
				//if (pbold->getBubbles()->count() <= 0)
				//{
				//	_removeBlock(pbold);
				//	_Debug("block #%02d removed.F (%p)", pbold->debug_id(), pbold);
				//}
				pblock->attachBubble(pbubble);
				_addBlock(pblock);
			}
		}
		break;
	case BS_Died:
		{
			//do not release bubble here, we are in block::onUpdate loop
			_Trace("bubble #%02d (%d,%d) is died", pbubble->debug_id(), 
				pbubble->getIndexRow(), pbubble->getIndexColumn());
		}
		break;
	case BS_Remove:
		{
			//bubble has been refrenced by block and board._bubblesInBoards, m_uRefrence is 2
			BZBlock* pblock = pbubble->getBlock();
			pbubble->detach(_pgame->layer());
			pblock->detachBubble(pbubble); //ref = 1
			_setBubble(pbubble->getIndexRow(), pbubble->getIndexColumn(), null); //ref = 0
		}
		break;
	case BS_Standing:
		//find neighbours here, and blend with them
		{
			int r = pbubble->getIndexRow();
			int c = pbubble->getIndexColumn();
			//test top
			int				dr[4] = { 0,	+1,			0,			-1};
			int				dc[4] = {-1,	0,			+1,			0};
			EBubbleNeighbour	nb[4] = {N_LEFT,N_BOTTOM,	N_RIGHT,	N_TOP};
			BZBlockBubble*		pbubbleNeighbour;

			const string& type = pbubble->getBubbleType();
			int i;
			for (i = 0; i < 4; i++)
			{
				pbubbleNeighbour = this->_getBubble(r + dr[i], c + dc[i]);
				if (null != pbubbleNeighbour && BS_Standing == pbubbleNeighbour->getState())
				{	
					pbubble->setNeighbour(nb[i], pbubbleNeighbour);
					
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
					}
				}
			}
		}
		break;
	}
}

BZBlockBubble* BZBoard::createBubble(int row, int col, const char* bubble, const char* prop)
{
	BZBlockBubble* pb = new BZBlockBubble(this);
	pb->initialize(bubble, prop);
	BZBlock* pblock = new BZBlock(this);
	pblock->attachBubble(pb);
	pb->setInitialPosition(ccp(col, row));
	_addBlock(pblock);
	return pb;
}

BZBlockBubble* BZBoard::_getGrabbedBubble(int finger)
{
	if (finger >= 0 && finger < _MAX_GRABBED_BLOCKS)
	{
		return _bubblesGrabbed[finger];
	}
	return null;
}

void BZBoard::_setGrabbedBubble(int finger, BZBlockBubble* pbubble)
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

BZBlockBubble* BZBoard::_getBubbleByPoint(const CCPoint& pos)
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
	BZBlockBubble* pbubble = _getBubbleByPoint(ptouch->pt());
	//block could be null
	if (null != pbubble)
	{
		if (pbubble->canMove())
		{
			_Trace("bubble #%02d (%d,%d) is grabbed", pbubble->debug_id(),
				pbubble->getIndexRow(), pbubble->getIndexColumn());
			pbubble->setState(BS_Drag);
			_setGrabbedBubble(ptouch->fingler(), pbubble);
		}
	}
}

void BZBoard::_onTouchMoving(CAEventTouch* ptouch)
{
	//find if this finger has grabbed a block?
	BZBlockBubble* pbubble = _getGrabbedBubble(ptouch->fingler());
	if (null == pbubble)
	{
		//we can try grab another one in moving state
		//_onTouchGrabbed(ptouch);
	}
	else
	{
		//move the grabbed block
		CCPoint pos = ptouch->pt();
		_sp2bp(pos);
		int r = _ROW(pos.y);
		int c = _COL(pos.x);
		if (_IS_IN_BOARD(r, c))
		{
			BZBlockBubble* pbHere = _getBubble(r, c);
			if (null == pbHere || pbubble == pbHere)
			{
				pbubble->setDraggingPos(pos);
			}
		}
	}
}

void BZBoard::_onTouchUngrabbed(CAEventTouch* ptouch)
{
	BZBlockBubble* pbubble = _getGrabbedBubble(ptouch->fingler());
	if (pbubble)
	{
		pbubble->setState(BS_Fall);
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


void BZBoard::onUpdate()
{
	//_Trace("board update begin");
	_removeBlocks();
	_addBlocks();
	verify();

	CAObject* pobj;
	CCARRAY_FOREACH(_blocksRunning, pobj)
	{
		BZBlock* pb = (BZBlock*)pobj;
		pb->onUpdate();
	}
	//_Trace("board update end");
}

void BZBoard::onExit()
{
	//remove all blocks
}
