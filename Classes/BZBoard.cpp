
#include "BZBoard.h"
#include "BZGame.h"
#include "AStageLayer.h"
#include "AWorld.h"

#include "AString.h"

#define DEFAULT_ACCELERATION (169.0f)

/// Game
BZBoard::BZBoard(BZGame* pgame)
{
	GUARD_FUNCTION();

	_pgame = pgame;
	
	_zorder = 0;
	_rows = -1;
	_cols = -1;

	memset(_aryBubblesBorned, 0, sizeof(_aryBubblesBorned));
	memset(_bubblesGrabbed, 0, sizeof(_bubblesGrabbed));

	_blocksRunning = CCArray::createWithCapacity(40);
	_blocksRunning->retain();
	_blocksIdle = CCArray::createWithCapacity(40);
	_blocksIdle->retain();
#if 0
	_bubblesPropStack = CCArray::createWithCapacity(4);
	_bubblesPropStack->retain();
#endif
	//_blocksWillBeRemoved = CCArray::create(40);
	//_blocksWillBeRemoved->retain();
}

BZBoard::~BZBoard()
{
	clear();
}

void BZBoard::clear()
{
	int r, c;
	BZBubble* pbubble;

	for (r = 0; r < _rows; r++)
	{
		for (c = 0; c < _cols; c++)
		{
			pbubble = _getBubble(r, c);
			if (pbubble)
			{
				//reduce block counter
				//loop-ref, block and bubble don't wanna to release first, just decrease ref-count
				pbubble->setBlock(null);
				_setBubble(r, c, null);
			}
		}
	}

	for (r = 0; r < _MAX_GRABBED_BLOCKS; r++)
	{
		pbubble = _getGrabbedBubble(r);
		if (pbubble)
		{
			//loop-ref, block and bubble don't wanna to release first, just decrease ref-count
			pbubble->setBlock(null);
			_setGrabbedBubble(r, null);
		}
	}

	for (c = 0; c < _cols; c++)
	{
		pbubble = _getBornBubble(c);
		if (pbubble)
		{
			//loop-ref, block and bubble don't wanna to release first, just decrease ref-count
			pbubble->setBlock(null);
			_setBornBubble(c, null);
		}
	}

	_blocksIdle->release();
	_blocksIdle = null;

	_blocksRunning->release();
	_blocksRunning = null;

#if 0
	_bubblesPropStack->release();
	_bubblesPropStack = null;
#endif

}

void BZBoard::loadData(CADataBuf& data)
{
	int n, blockcount, bubblecount;
	string str;

	data >> str;	_Assert(str == "boardb");
	data >> n;		_Assert(n == 0x10000);
	data >> n;		_Assert(n == _rows);
	data >> n;		_Assert(n == _cols);
	data >> blockcount;
	data >> bubblecount;
	for (n = 0; n < blockcount; n++)
	{
		BZBlock* pblock = _newBlockHolder();
		pblock->loadData(data);
	}
	data >> str; _Assert(str == "boarde");

	//force bubbles update
}

void BZBoard::saveData(CADataBuf& data)
{
	data << "boardb";
	data << 0x10000;
	data << this->_rows;
	data << this->_cols;
	data << this->_blocksRunning->count();
	data << this->getBubblesCount();

	CAObject* pobj;
	CCARRAY_FOREACH(_blocksRunning, pobj)
	{
		BZBlock* pb = (BZBlock*)pobj;
		pb->saveData(data);
	}
	data << "boarde";
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

unsigned int BZBoard::getStarsCount(const char* type) const
{
	unsigned int nStars = 0;
	int r, c;
	BZBubble* pbubble;

	for (r = 0; r < _rows; r++)
	{
		for (c = 0; c < _cols; c++)
		{
			pbubble = _getBubble(r, c);
			if (pbubble)
			{
				const string& pt = pbubble->getPropType();
				if (CAString::startWith(pt, PROP_STAR))
				{
					nStars++;
				}
			}
		}
	}
	return nStars;
}

unsigned int BZBoard::getBubblesCount() const
{
	unsigned int nb = 0;
	int r, c;
	BZBubble* pbubble;

	for (r = 0; r < _rows; r++)
	{
		for (c = 0; c < _cols; c++)
		{
			pbubble = _getBubble(r, c);
			if (pbubble)
			{
				nb++;
			}
		}
	}
	return nb;
}

#if 0
void BZBoard::getCounts(int& bubblecount, int& blockcount, int& stars, int& props)
{
	bubblecount = 0;
	blockcount = 0;
	stars = 0;
	props = 0;

	unsigned int n = 0;
	CAObject* pobj;
	CCARRAY_FOREACH(_blocksRunning, pobj)
	{
		BZBlock* pb = (BZBlock*)pobj;
		bubblecount += pb->getBubbles()->count();
		blockcount++;
		stars += pb->getStars();
		props += pb->getProps();
	}
	return;
}

unsigned int BZBoard::getStarsCount(const char* type) const
{
	unsigned int n = 0;
	CAObject* pobj;
	CCARRAY_FOREACH(_blocksRunning, pobj)
	{
		BZBlock* pb = (BZBlock*)pobj;
		if (pb->getBubbles()->count() > 0 && 
			(null == type || pb->getBubbleType() == type))
		{
			n += pb->getStars();
		}
	}
	return n;
}

unsigned int BZBoard::getBubblesCount() const
{
	some borned bubble in running blocks 
	unsigned int n = 0;
	CAObject* pobj;
	CCARRAY_FOREACH(_blocksRunning, pobj)
	{
		BZBlock* pb = (BZBlock*)pobj;
		n += pb->getBubbles()->count();
	}
	return n;
}
#endif

void BZBoard::setParams(const CCPoint& ptLeftBottom, 
						int rows, int cols, float bubblesize,
						float zorder)
{
	_rows = rows;
	_cols = cols;
	_zorder = zorder;

	_ptLeftBottom = ptLeftBottom;
	CAWorld::percent2view(_ptLeftBottom);
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
		_Assert(_rows <= 32 && _cols <= 32);
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

bool BZBoard::verifyBubble(BZBubble* pbubble)
{
	if (pbubble->getPos().y < -0.5f)
		return true;
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
	pos.x += this->_ptLeftBottom.x;
	pos.y = this->_ptLeftBottom.y + this->_fBubbleSize * (this->_rows - 1) - pos.y;
}

//screen position to block position
void BZBoard::_sp2bp(CCPoint& pos) const
{
	pos.y = this->_ptLeftBottom.y + this->_fBubbleSize * (this->_rows -1 ) - pos.y;
	pos.x -= this->_ptLeftBottom.x;
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

	//for borning bubbles
	//we remove _Assert(_IS_IN_BOARD(r, c));
	if (_IS_IN_BOARD(r, c))
	{
		BZBubble* pb = _getBubble(r, c);
		_Assert(null == pb || pb == pbubble);
		if (null == pb)
		{
			_removeFromBornedLine(pbubble);

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
	else
	{
		//it can be falling
		//_Assert(pbubble->getState() == BS_Born);
	}
}

BZBlock* BZBoard::_newBlockHolder()
{
	GUARD_FUNCTION();

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
	}
	pblock->release();

	return pblock;
}

void BZBoard::_doBlockBlend(BZBubble* pbubble)
{
	//_Trace("bubble #%02d block blending", pbubble->debug_id());

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
	//_Trace("bubble #%02d change pose to %s", pbubble->debug_id(), pose.c_str());
	pbubble->setPose(pose);
}

//when a bubble leaves, all bubbles in this block leave from this block
//redo block blending and pose blending
void BZBoard::_doLeaveBlock(BZBubble* pbubble)
{
	BZBlock* pblock = pbubble->getBlock();
	CCArray* pbubbles = pblock->getBubbles();
	unsigned i, c = pbubbles->count();
	if (c == 1)
		return;
	CCArray* pbubblesDetached = CCArray::createWithCapacity(c);
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
		BZBlock* pbn = _newBlockHolder();
		pbn->attachBubble(pb);
	}
	pbubblesDetached->release();
}

void BZBoard::_doBubbleDied(BZBubble* pbubble)
{
	//do not release bubble here, we are in block::onUpdate loop
	BZBlock* pblock = pbubble->getBlock();
	//_Trace("bubble #%02d (%d,%d) is died, leave block #%02d", 
	//	pbubble->debug_id(), 
	//	pbubble->getIndexRow(), pbubble->getIndexColumn(),
	//	pblock->debug_id());
	pbubble->setState(BS_NA);
	
	//for effect killed bubbles
	pblock->detachBubble(pbubble); //ref = 1

	_setBubble(pbubble->getIndexRow(), pbubble->getIndexColumn(), null); //ref = 0

	//mv from onBlockUpdate
	pbubble->detach(_pgame->layer());

	pbubble->try2Reborn();

	//make neighbours redo poselending
	int r, c;
	int	dr[4] = { 0,	+1,			0,			-1};
	int	dc[4] = {-1,	0,			+1,			0};
	BZBubble* pbCheck;

	r = pbubble->getIndexRow();
	c = pbubble->getIndexColumn();

	int i;
	for (i = 0; i < 4; i++)
	{
		pbCheck = getBubble(r + dr[i], c + dc[i]);
		if (null != pbCheck && pbCheck->getBlock() != pbubble->getBlock())
		{
			if (pbCheck->getState() > BS_PoseBlend && pbCheck->getState() < BS_Die)
			{
				pbCheck->setState(BS_PoseBlend);
			}
		}
	}
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
	case BS_Borned:
		break;
	case BS_Fall:
	case BS_Drag:
		_doLeaveBlock(pbubble);
		break;
	case BS_Gen:
	case BS_Gening:
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
		_doBubbleDied(pbubble);
		break;
	default:
		_Assert(false);
		break;
	}
}

void BZBoard::getBubbleRenderPos(CCPoint& pos) const
{ 
	_bp2sp(pos); 
}

int BZBoard::getEmptyBornSlots(int* slots, int scount) const
{
	_Assert(_cols <= scount);
	memset(slots, 0, scount * sizeof(int));

	int freed = 0;
	int i;
	for (i = 0; i < _cols; i++)
	{
		//if (null == _getBubble(0, i))
		if (null == _getBornBubble(i))
		{
			slots[freed++] = i;
		}
	}
	return freed;
}

bool BZBoard::isHeaderLineFull() const
{
	int c;
	for (c = 0; c < _cols; c++)
	{
		BZBubble* pbubble = _getBornBubble(c);
		if (null == pbubble)
		{
			return false;
		}
	}
	return true;
}

void BZBoard::fallOneRow()
{
	int c;
	for (c = 0; c < _cols; c++)
	{
		BZBubble* pb0 = _getBornBubble(c);
		BZBubble* pb1 = _getBubble(0, c);
		if (null != pb0 && null == pb1)
		{
			if (pb0->getState() == BS_Borned)
			{
				//when bubble position falled into board, it will be managed
				pb0->setState(BS_Release);
			}
		}
	}
}


void BZBoard::_setBornBubble(int col, BZBubble* pbubble)
{
	_Assert(col >= 0 && col < _cols);
	_Assert(col < SIZE_OF_ARRAY(_aryBubblesBorned));

	if (_aryBubblesBorned[col])
	{
		_aryBubblesBorned[col]->release();
		_aryBubblesBorned[col] = null;
	}
	if (pbubble)
	{
		pbubble->retain();
		_aryBubblesBorned[col] = pbubble;
	}
}

BZBubble* BZBoard::_getBornBubble(int col) const
{
	_Assert(col >= 0 && col < _cols);
	_Assert(col < SIZE_OF_ARRAY(_aryBubblesBorned));

	return _aryBubblesBorned[col];
}

void BZBoard::_removeFromBornedLine(BZBubble* pbubble)
{
	int c;
	for (c = 0; c < _cols; c++)
	{
		BZBubble* pb0 = _aryBubblesBorned[c];
		if (pb0 == pbubble)
		{
			_setBornBubble(c, null);
			return;
		}
	}
}


BZBubble* BZBoard::createBornBubble(const char* bubble, int col, const char* prop, const char* doodad,
	BZBlock* pholder)
{
	GUARD_FUNCTION();

	CCPoint pos = ccp(col, -1.2f);
	_Assert(col >= 0 && col < _cols);
	_Assert(_getBornBubble(col) == null);
	BZBubble* pb = createBubble1(bubble, pos, prop, doodad, null);
	_setBornBubble(col, pb);
	return pb;
}

BZBubble* BZBoard::createBubble1(
	const char* bubble, const CCPoint& pos, const char* prop, const char* doodad,
	BZBlock* pholder)
{
	GUARD_FUNCTION();

	BZBubble* pb = new BZBubble(this);
	pb->initialize(bubble, prop, doodad, _zorder);
	return _bindBubble(pb, pos, pholder, BS_Born);
}

BZBubble* BZBoard::_bindBubble(BZBubble* pb, const CCPoint& pos, BZBlock* pholder, EBubbleState state)
{
	GUARD_FUNCTION();

	BZBlock* pblock;
	
	if (null != pholder)
	{
		pblock = pholder;
	}
	else
	{
		pblock = _newBlockHolder();
	}
	pblock->attachBubble(pb);

	pb->setState(state);
	pb->setInitialPosition(pos);

	return pb;
}

#if 0
void BZBoard::pushPropBubble(const CCPoint& pos, const char* type, const char* prop)
{
	GUARD_FUNCTION();

	BZBubble* pb = new BZBubble(this);
	pb->initialize(type, prop, null, _zorder);
	pb->setPendingPosition(pos);

	_bubblesPropStack->addObject(pb);
}
#endif

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
		//_Trace("bubble #%02d (%d,%d) is grabbed", pbubble->debug_id(),
		//	pbubble->getIndexRow(), pbubble->getIndexColumn());
		if (pbubble->canMove())
		{
			pbubble->setState(BS_Drag);
		}
		_setGrabbedBubble(ptouch->fingler(), pbubble);
	}
}

bool BZBoard::_hasBeenOccupied(int r, int c, BZBubble* pbExclude)
{
	if (!_IS_IN_BOARD(r, c))
		return true;
	BZBubble* pb = _getBubble(r, c);
	if (null == pb)
		return false;
	if (null != pbExclude && pb == pbExclude)
		return false;
	return true;
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

		CCPoint posC = pos;
		int r, c;
		int r0 = _ROW(pos.y);
		int c0 = _COL(pos.x);
		_Debug("pos.x=%.2f pos.y=%.2f r0=%d,c0=%d", pos.x, pos.y, r0, c0);

		float minx, maxx;
		float miny, maxy;
		//check left
		r = _ROW(pos.y + 0.0f);	c = _COL(pos.x - 1.0f);	if (_hasBeenOccupied(r, c, pbubble)) { minx = (float)c0; _Debug("minx inb=%s, bubble(%d,%d)=%s", _IS_IN_BOARD(r,c) ? "true" : "false", r, c, _getBubble(r, c) ? "true" : "false"); } else { minx = -10000; }
		//check right
		r = _ROW(pos.y + 0.0f);	c = _COL(pos.x + 1.0f);	if (_hasBeenOccupied(r, c, pbubble)) { maxx = (float)c0; _Debug("maxx inb=%s, bubble(%d,%d)=%s", _IS_IN_BOARD(r,c) ? "true" : "false", r, c, _getBubble(r, c) ? "true" : "false"); } else { maxx = 10000; }
		//check u
		r = _ROW(pos.y - 1.0f);	c = _COL(pos.x + 0.0f);	if (_hasBeenOccupied(r, c, pbubble)) { miny = (float)r0; _Debug("miny inb=%s, bubble(%d,%d)=%s", _IS_IN_BOARD(r,c) ? "true" : "false", r, c, _getBubble(r, c) ? "true" : "false"); } else { miny = -10000; }
		//check doen
		r = _ROW(pos.y + 1.0f);	c = _COL(pos.x + 0.0f);	if (_hasBeenOccupied(r, c, pbubble)) { maxy = (float)r0; _Debug("maxy inb=%s, bubble(%d,%d)=%s", _IS_IN_BOARD(r,c) ? "true" : "false", r, c, _getBubble(r, c) ? "true" : "false"); } else { maxy = 10000; }
		_Debug("minx=%.2f maxx=%.2f miny=%.2f maxy=%.2f", minx, maxx, miny, maxy);

		if (pos.x < minx) { _Debug("minx pos.x %.2f->%2f ", pos.x, minx); pos.x = minx; }
		if (pos.x > maxx) { _Debug("maxx pos.x %.2f->%2f ", pos.x, maxx); pos.x = maxx; }
		if (pos.y < miny) { _Debug("miny pos.y %.2f->%2f ", pos.y, miny); pos.y = miny; }
		if (pos.y > maxy) { _Debug("maxy pos.y %.2f->%2f ", pos.y, maxy); pos.y = maxy; }

		r = _ROW(pos.y);
		c = _COL(pos.x);
		_Debug("pos.x=%.2f pos.y=%.2f r=%d c=%d", pos.x, pos.y, r, c);
		if (!_hasBeenOccupied(r, c, pbubble))
		{
			pbubble->setDraggingPos(pos);
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
		if (BS_Dragging == s || BS_Drag == s)
		{
			pbubble->setState(BS_Fall);
		}
		_setGrabbedBubble(ptouch->fingler(), null);
	}
	else
	{
		pbubble = null;
	}
}

void BZBoard::onEvent(const CAEvent* pevt)
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
	_Assert(_pgame);
	_pgame->boomBlock(pblock);
}

void BZBoard::onUpdate()
{
	int r, c;
	BZBubble* pbubble;

	for (c = 0; c < _cols; c++)
	{
		pbubble = _getBornBubble(c);
		if (pbubble)
		{
			pbubble->onUpdate();
		}
	}
	for (r = 0; r < _rows; r++)
	{
		for (c = 0; c < _cols; c++)
		{
			pbubble = _getBubble(r, c);
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

#if 0
	CCArray* pary = CCArray::createWithCapacity(1);
	CCARRAY_FOREACH(_bubblesPropStack, pobj)
	{
		BZBubble* pb = (BZBubble*)pobj;
		CCPoint posNew = pb->getPendingPosition();
 		int r = _ROW(posNew.y);
		int c = _COL(posNew.x);

		_Assert(_IS_IN_BOARD(r, c));

		BZBubble* pbE = _getBubble(r, c);
		if (null == pbE)
		{
			//now we can create this prop bubble
			pary->addObject(pb);
		}
	}

	CCARRAY_FOREACH(pary, pobj)
	{
		BZBubble* pb = (BZBubble*)pobj;
		pb->retain();
		_bubblesPropStack->removeObject(pb);
		_bindBubble(pb, pb->getPendingPosition(), null, BS_Gen);
		pb->release();
	}
#endif
}

void BZBoard::onExit()
{
	//remove all blocks
}
