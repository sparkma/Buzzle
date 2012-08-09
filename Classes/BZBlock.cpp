
#include "BZBlock.h"
#include "BZBoard.h"
#include "BZBlockBubble.h"
#include "AStageLayer.h"
#include "AWorld.h"
#include "AString.h"

#define PROP_STAR	"star"
#define PROP_BOMB	"bomb"

BZBlock::BZBlock(BZBoard* pboard)
{
	_pboard = pboard;

	_stars = 0;

	_bubbles = CCArray::arrayWithCapacity(4);
	_bubbles->retain();
}

BZBlock::~BZBlock()
{
	_Assert(_bubbles);
	_bubbles->release(); 
	_bubbles = null;
}

void BZBlock::append(BZBlock* pblock)
{
	CAObject* pobj;
	CCArray* objs;
	
	objs = pblock->_bubbles;
	CCARRAY_FOREACH(objs, pobj)
	{
		_bubbles->addObject(pobj);
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
