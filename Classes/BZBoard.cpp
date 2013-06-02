
#include "BZBoard.h"
#include "BZGame.h"
#include "AStageLayer.h"
#include "AWorld.h"

#include "AString.h"

#define DEFAULT_ACCELERATION (169.0f)
#define _Z_GLOBAL_EFFECT	1

BZBoard::BZBoard(CAStageLayer* player)
{
	GUARD_FUNCTION();

	_pLayer = player;
	//_pgame = pgame;
	
	//_zorder = player->getBaseZOrder() + 1;

	_rows = -1;
	_cols = -1;

	_nBubbleCount = 0;
	_fBubbleXPercentSize = 0;
	_fBubbleSize = 0;

	memset(_aryBubblesBorned, 0, sizeof(_aryBubblesBorned));
#if defined(_DEBUG)
	memset(_bubblesInBoards, 0, sizeof(_bubblesInBoards));
#else
	_bubblesInBoards = null;
#endif
	//_bubblesAlloced = CCArray::createWithCapacity(40);;

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

	int a;
	a = sizeof(CAObject);
	a = sizeof(CAEventDispatcher);
	//_VerifyClass(this);
}

BZBoard::~BZBoard()
{
	_freeAllResources();
}

void BZBoard::_freeAllResources()
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

	//_bubblesAlloced->release();
	//_bubblesAlloced = null;
}

#if 0
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
#endif

ccTime BZBoard::getTimeNow() const
{ 
	_Assert(_pLayer);
	return _pLayer->getTimeNow(); 
}

//for debugging
string BZBoard::debuglog()
{
	char sz[128];
	sprintf(sz, "blocks=%d(-%d),bubbles=%d", 
		_blocksRunning->count(), _blocksIdle->count(), this->getBubblesCount());
	return sz;
}

#if 0
ccTime BZBoard::getTimeNow() const
{ 
	_Assert(_pgame);
	return _pgame->getTimeNow(); 
}
#endif

float BZBoard::getBaseZOrder() const
{ 
	_Assert(_pLayer); 
	return _pLayer->getBaseZOrder() + 1.0f; 
}


void BZBoard::setParams(const CCPoint& ptBoardAnchor, 
						int rows, int cols, float bubblesize)
{
	_rows = rows;
	_cols = cols;

	_ptLeftBottom = ptBoardAnchor;
	//_ptLeftBottom.y = 1.0f - _ptLeftBottom.y;

	CAWorld::percent2view(_ptLeftBottom);
	_fBubbleXPercentSize = bubblesize;
	CAWorld::percent2view(bubblesize, true);
	_fBubbleSize = bubblesize;
	_ptLeftBottom.y -= bubblesize * rows;
	_ptLeftBottom.x -= 0; //bubblesize * cols / 2;

#if defined(_DEBUG)
	memset(_bubblesInBoards, 0, sizeof(_bubblesInBoards));
#else
	_bubblesInBoards = new BZBubble* [rows * cols];
	memset(_bubblesInBoards, 0, sizeof(BZBubble*) * rows * cols);
#endif
}


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

EBubbleBlockerType BZBoard::getBubbleBlocker(int r, int c, CCPoint& pos, BZBubble** ppBubble)
{
	//_Assert(null == _getBubble(r, c));
	if (c < 0 || c >= _cols)
		return BT_Invalid;

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
				if (ppBubble) *ppBubble = pbubbleBottom;
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
	//CCSize size = CAWorld::sharedWorld()->getScreenSize();
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
			EBubbleState bs = pbubble->getState();
			if (bs > BS_Dived)
			{
				_removeFromBornedLine(pbubble);
			}

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
	_Debug("bubble #%02d(r=%d) (%d,%d) is died, leave block #%02d", 
		pbubble->debug_id(), 
		pbubble->retainCount(),
		pbubble->getIndexRow(), pbubble->getIndexColumn(),
		pblock->debug_id());
	pbubble->setState(BS_NA);
	
	//for effect killed bubbles
	pblock->detachBubble(pbubble); //ref = 1

	_setBubble(pbubble->getIndexRow(), pbubble->getIndexColumn(), null); //ref = 0
	_removeFromBornedLine(pbubble);

	//mv from onBlockUpdate
	_onDetachBubbleSprite(pbubble);

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
		pbCheck = _getBubble(r + dr[i], c + dc[i]);
		if (null != pbCheck && pbCheck->getBlock() != pbubble->getBlock())
		{
			if (pbCheck->getState() > BS_PoseBlend && pbCheck->getState() < BS_Boom)
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
	case BS_Dive:
	case BS_Diving:
		break;
	case BS_Dived:
		//_removeFromBornedLine(pbubble);
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
	case BS_Reborn:
	case BS_DragRelease:
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
	case BS_Boom:
	case BS_Die:
	case BS_DieNow:
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

int BZBoard::getBornBubbles(BZBubble** slots, int scount) const
{
	if (null != slots)
	{
		_Assert(_cols <= scount);
		memset(slots, 0, scount * sizeof(BZBubble*));
	}
	int i, n = 0;
	for (i = 0; i < _cols; i++)
	{
		BZBubble* pb = _getBornBubble(i);
		if (null != pb) n++;
		if (null != slots)
		{
			slots[i] = pb;
		}
	}
	return n;
}

bool BZBoard::isHeaderLineCanFall() const
{
	int c;
	for (c = 0; c < _cols; c++)
	{
		BZBubble* pbubble = _getBornBubble(c);
		if (null != pbubble)
		{
			if (pbubble->getState() < BS_Borned)
				return false;
		}
	}
	return true;
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
		if (pbubble->getState() != BS_Borned)
			return false;
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
				//pb0->setState(BS_Release);
				pb0->setState(BS_Dive);
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
	pb->initialize(bubble, prop, doodad);
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

BZBubble* BZBoard::_getBubbleByPoint(const CCPoint& pos)
{
	CCPoint p = pos;
	
	_sp2bp(p);

#if 1
	int r = _ROW(p.y);
	int c = _COL(p.x);
	int testr, testc;
	float d2 = 1000000;
	BZBubble* pbSelected = null;
	int sr, sc;
	sr = r;
	sc = c;
	for (testr = -1; testr <= 1; testr++)
	{
		for (testc = -1; testc <= 1; testc++)
		{
			int tr = r + testr;
			int tc = c + testc;
			BZBubble* pb = _getBubble(tr, tc);
			if (pb)
			{
				float dx = p.x - (float)tc;
				float dy = p.y - (float)tr;
				float d = dx * dx + dy * dy;
				if (d < d2 && d < 1.0f)
				{
					pbSelected = pb;
					d2 = d;
					sr = tr;
					sc = tc;
				}
			}
		}
	}
	if (pbSelected)
	{
		_Info("testing bubble %d,%d", sr, sc);
	}
	return pbSelected;
#else
#if 0
	int sr = -1;
	int sc = -1;

	int max = 0;
	int n = 0;
	int selected = -1;
	int x, y;

	BZBubble* pb[_T_POINTS];
	int count[_T_POINTS];
	memset(pb, 0, sizeof(pb));
	memset(count, 0, sizeof(count));

	for (y = -_TEST_POINTS; y <= _TEST_POINTS; y++)
	{
		for (x = -_TEST_POINTS; x <= _TEST_POINTS; x++)
		{
			int r = _ROW(p.y + 0.5f / (float)_TEST_POINTS * y);
			int c = _COL(p.x + 0.5f / (float)_TEST_POINTS * x);
			BZBubble* p = _getBubble(r, c);

			if (p)
			{
				int set = -1;
				int i;
				bool find = false;
				for (i = 0; i < n; i++)
				{
					if (pb[i] == p)
					{
						count[i]++;
						set = i;
						find = true;
						break;
					}
				}
				if (!find)
				{
					pb[n] = p;
					count[n] = 1;
					set = n;
					n++;
				}
				_Assert(set != -1);
				if (count[set] > max)
				{
					max = count[set];
					selected = set;
					sr = r;
					sc = c;
				}
			}
		}
	}

	if (selected >= 0 && selected < _T_POINTS)
	{
		_Info("testing bubble %d,%d", sr, sc);
		return pb[selected];
	}
	return null;
#endif

	int r = _ROW(p.y);
	int c = _COL(p.x);
	//_Trace("trying find block at %d,%d", r, c);
	//_getBubble will check bounds
	return _getBubble(r, c);
#endif
}

bool BZBoard::hasBeenOccupied(int r, int c, BZBubble* pbExclude)
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


bool BZBoard::onEvent(const CAEvent* pevt)
{
	switch (pevt->type())
	{
	case ET_Touch:
		{
		}
		break;
	case ET_Command:
		{
		}
		break;
	}
	return false;
}

void BZBoard::onEnter()
{
	//do nothing
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

	_nBubbleCount = 0;
	for (r = 0; r < _rows; r++)
	{
		for (c = 0; c < _cols; c++)
		{
			pbubble = _getBubble(r, c);
			if (null != pbubble)
			{
				_nBubbleCount++;
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

int BZBoard::getEffectedBlock(BZBubble* pbCheck, int range, BZBubble** pbEffected, int esize)
{
	int n = 0;
	int r0 = pbCheck->getIndexRow();
	int c0 = pbCheck->getIndexColumn();
	int r, c;
	for (r = r0 - range; r < r0 + range; r++)
	{
		for (c = c0 - range; c < c0 + range; c++)
		{
			int dr = r - r0;
			int dc = c - c0;
			if (dr * dr + dc * dc > range * range)
				continue;

			BZBubble* pb = _getBubble(r, c);
			if (null == pb)
				continue;
			if (pb->getBubbleType() != pbCheck->getBubbleType() && pb->getBlock() != pbCheck->getBlock())
			{
				if (n < esize)
				{
					pbEffected[n++] = pb;
				}
			}
		}
	}
	return n;
}

BZSpriteCommon* BZBoard::addGlobalEffect(const CCPoint& pos_, const char* effect, const char* pose)
{
	GUARD_FUNCTION();

	BZSpriteCommon* pspr = new BZSpriteCommon(layer(), effect);

	CCPoint pos = pos_;
	pspr->setPos(pos);
	pspr->setVertexZ(getBaseZOrder() + VZ_G_EFFECTS); //defined in bubble
	pspr->pushState(pose);

	layer()->addSprite(pspr);

	return pspr;
}

