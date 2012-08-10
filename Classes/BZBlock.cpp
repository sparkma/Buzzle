
#include "BZBlock.h"
#include "BZBoard.h"
#include "BZBlockBubble.h"
#include "AStageLayer.h"
#include "AWorld.h"
#include "AString.h"

#define PROP_STAR	"star"
#define PROP_BOMB	"bomb"

static int s_debug_id = 0;

BZBlock::BZBlock(BZBoard* pboard)
{
	_pboard = pboard;

	_stars = 0;

	_bubbles = CCArray::arrayWithCapacity(4);
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

void BZBlock::verify()
{
	_Assert(_bubbles->count() > 0);

	int nStars = 0;
	CAObject* pobj;
	CCARRAY_FOREACH(_bubbles, pobj)
	{
		BZBlockBubble* pb = (BZBlockBubble*)pobj;
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
		BZBlockBubble* pbb = (BZBlockBubble*)pobj;
		pbb->setBlock(this);
		_bubbles->addObject(pbb);
	}

	_stars += pblock->getStars();
}

void BZBlock::attachBubble(BZBlockBubble* pbubble) 
{ 
	_Assert(_bubbles); 
	_bubbles->addObject(pbubble);
	pbubble->setBlock(this);
	const string& pt = pbubble->getPropType();
	if (CAString::startWith(pt, PROP_STAR))
	{
		_stars++;
	}
}

void BZBlock::detachBubble(BZBlockBubble* pbubble) 
{ 
	_Assert(_bubbles); 
	_bubbles->removeObject(pbubble);
	pbubble->setBlock(null);
	const string& pt = pbubble->getPropType();
	if (CAString::startWith(pt, PROP_STAR))
	{
		_stars--;
	}
}

void BZBlock::onUpdate()
{
	CAObject* pobj;
	CCARRAY_FOREACH(_bubbles, pobj)
	{
		BZBlockBubble* pb = (BZBlockBubble*)pobj;
		pb->onUpdate();
	}
}

void BZBlock::onBubblePositionChanged(BZBlockBubble* pbubble, const CCPoint& posOld, const CCPoint& posNew)
{
	_Assert(this->_pboard);
	this->_pboard->onBubblePositionChanged(pbubble, posOld, posNew);
}
