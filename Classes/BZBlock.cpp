
#include "BZBlock.h"
#include "BZBoard.h"
#include "BZBubble.h"
#include "AStageLayer.h"
#include "AWorld.h"
#include "AString.h"
#include "BZGame.h"

static int s_debug_id = 0;

BZBlock::BZBlock(BZBoard* pboard)
{
	_pboard = pboard;

	_state = Block_Running;
	_bDirty = false;
	_stars = 0;
	_standing = 0;

	_bubbles = CCArray::createWithCapacity(4);
	_bubbles->retain();

	autorelease();

	s_debug_id++;
	_debug_id = s_debug_id;
	_Debug("block #%02d created(%p)", _debug_id, this);
}

BZBlock::~BZBlock()
{
	_Debug("block #%02d released(%p)", _debug_id, this);

	_Assert(_bubbles);
	_bubbles->release(); 
	_bubbles = null;
}

#if 0
void BZBlock::loadData(CADataBuf& data)
{
	int n, bubblecount;
	string str;

	data >> str; _Assert(str == "blockb");
	data >> n; _Assert(n == 0x10000);
	data >> _debug_id; 
	data >> _state;
	int stars;
	data >> stars;
	data >> bubblecount;
	for (n = 0; n < bubblecount; n++)
	{
		BZBubble* pb = new BZBubble(_pboard);
		pb->loadData(data);
		this->attachBubble(pb); //will update _stars
	}
	_Assert(stars == _stars);

	data >> str; _Assert(str == "blocke");
}

void BZBlock::saveData(CADataBuf& data)
{
	data << "blockb";
	data << 0x10000;
	data << this->_debug_id;
	data << this->_state;
	data << this->_stars;
	data << this->_bubbles->count();

	CAObject* pobj;
	CCARRAY_FOREACH(_bubbles, pobj)
	{
		BZBubble* pb = (BZBubble*)pobj;
		pb->saveData(data);
	}
	data << "blocke";
}
#endif

void BZBlock::verify()
{
	_Assert(_bubbles->count() > 0 || Block_Boooming == _state);

	int nStars = 0;
	CAObject* pobj;
	CCARRAY_FOREACH(_bubbles, pobj)
	{
		BZBubble* pb = (BZBubble*)pobj;
		_Assert(pb->getBlock() == this);
		const string& pt = pb->getPropType();
		if (CAString::startWith(pt, PROP_STAR))
		{
			nStars++;
		}
	}
	_Assert(nStars == _stars);
}

void BZBlock::append(BZBlock* pblock)
{
	CAObject* pobj;
	CCArray* objs;
	
	objs = pblock->_bubbles;
	CCARRAY_FOREACH(objs, pobj)
	{
		BZBubble* pbb = (BZBubble*)pobj;
		this->attachBubble(pbb);
	}

	//_stars += pblock->getStars();

	pblock->_bubbles->removeAllObjects();
	pblock->_stars = 0;

	_bDirty = true;
}

void BZBlock::attachBubble(BZBubble* pbubble) 
{ 
	_Assert(_bubbles); 
	_bubbles->addObject(pbubble);
	pbubble->setBlock(this);
	if (pbubble->hasStar())
	{
		_stars++;
	}
	if (_bubbletype.length() <= 0)
	{
		_bubbletype = pbubble->getBubbleType();
	}
	else
	{
		_Assert(_bubbletype == pbubble->getBubbleType());
	}
	_bDirty = true;
}

void BZBlock::detachBubble(BZBubble* pbubble) 
{ 
	_Assert(_bubbles); 
	_bubbles->removeObject(pbubble);
	pbubble->setBlock(null);
	const string& pt = pbubble->getPropType();
	if (CAString::startWith(pt, PROP_STAR))
	{
		_stars--;
	}
	_bDirty = true;
}

int BZBlock::getStandingCount() const
{
	if (_bDirty)
	{
		_standing = 0;
		unsigned int i, c = _bubbles->count();
		for (i = 0; i < c; i++)
		{
			BZBubble* pbubble = (BZBubble*)_bubbles->objectAtIndex(i);
			if (BS_Standing == pbubble->getState())
			{
				_standing++;
			}
		}

	}
	return _standing;
}

bool BZBlock::isAllStanding() const
{
	return _bubbles->count() == getStandingCount();
}

#if 0
bool BZBlock::isOpen() const
{
	CAObject* pobj;
	CCARRAY_FOREACH(_bubbles, pobj)
	{
		BZBubble* pb = (BZBubble*)pobj;
		if (pb->canMove())
		{
			return true;
		}
	}
	return false;
}
#endif

void BZBlock::getStatus(int& bubbles, int& movables, int& stars, bool& hasSOB) const
{
	movables = 0;
	stars = 0;
	hasSOB = false;
	bubbles = _bubbles->count();

	//_Info("block info:bubbles=%d,type=%s", bubbles, _bubbletype.c_str());
	CAObject* pobj;
	CCARRAY_FOREACH(_bubbles, pobj)
	{
		BZBubble* pb = (BZBubble*)pobj;
		EBubbleState bs = pb->getState();
		bool hs = pb->hasStar();
		bool cm = pb->canMove();
		if (hs)
		{
			stars++;
		}
		if (cm || (bs >= BS_Born && bs <= BS_Borned))
		{
			movables++;
		}
		if (hs && cm)
		{
			hasSOB = true;
		}
		//_Info("pb(%d,%d),bs=%s", pb->getIndexRow(), pb->getIndexColumn(), BZBubble::state2str(bs));
	}
	//_Info("block info:bubbles=%d,type=%-18s, stars=%d, movables=%d, hasSOB=%d", bubbles, _bubbletype.c_str(), stars, movables, hasSOB);

	return;
}

#if 0
int BZBlock::getProps() const
{
	return 0;
}
#endif

void BZBlock::reset()
{
	_Assert(_bubbles->count() <= 0);
	_Assert(_stars == 0);
	_state = Block_Running;
	_bubbletype = "";
}

//you can call this function if _stars < 2
//calculate center bubble
BZBubble* BZBlock::booom()
{
	if (Block_Running != _state)
	{
		return null;
	}

	_Trace("block #%02d booooom", this->debug_id());
	_state = Block_Boooming;

	CCPoint pos;
	pos.x = 0;
	pos.y = 0;

	CAObject* pobj;
	CCARRAY_FOREACH(_bubbles, pobj)
	{
		BZBubble* pb = (BZBubble*)pobj;
		EBubbleState s = pb->getState();
		if (s < BS_Die)
		{
			pb->setState(BS_Die);
		}
		pos.x += pb->getPos().x;
		pos.y += pb->getPos().y;
	}

	int bc = _bubbles->count();
	pos.x /= (0.000001f + bc);
	pos.y /= (0.000001f + bc);
	float mindist = (float)0x3ffffff;
	BZBubble* pbSelected = null;
	CCARRAY_FOREACH(_bubbles, pobj)
	{
		BZBubble* pb = (BZBubble*)pobj;
		float dx = pb->getPos().x - pos.x;
		float dy = pb->getPos().y - pos.y;
		float dist2 = dx * dx + dy * dy;
		if (dist2 < mindist)
		{
			pbSelected = pb;
			mindist = dist2;
		}
	}

	return pbSelected;
}
