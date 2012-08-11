
#include "BZBlock.h"
#include "BZBoard.h"
#include "BZBubble.h"
#include "AStageLayer.h"
#include "AWorld.h"
#include "AString.h"
#include "BZGame.h"

#define PROP_STAR	"star"
#define PROP_BOMB	"bomb"

static int s_debug_id = 0;

BZBlock::BZBlock(BZBoard* pboard)
{
	_pboard = pboard;

	_state = Block_Running;
	_stars = 0;

	_bubbles = CCArray::create(4);
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
}

void BZBlock::attachBubble(BZBubble* pbubble) 
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
}

void BZBlock::reset()
{
	_Assert(_bubbles->count() <= 0);
	_Assert(_stars == 0);
	_state = Block_Running;
}

//you can call this function if _stars < 2
void BZBlock::booom()
{
	if (Block_Running != _state)
		return;

	_Trace("block #%02d booooom", this->debug_id());
	_state = Block_Boooming;

	CAObject* pobj;
	CCARRAY_FOREACH(_bubbles, pobj)
	{
		BZBubble* pb = (BZBubble*)pobj;
		EBubbleState s = pb->getState();
		if (s < BS_Die)
		{
			pb->setState(BS_Die);
		}
	}
}

#if 0
void BZBlock::onUpdate()
{
	//_Trace("block #%02d update enter", debug_id());
	if (Block_Running != _state && Block_Boooming != _state)
		return;

	_Assert(_bubblesDied->count() == 0);

	CAObject* pobj;
	CCARRAY_FOREACH(_bubbles, pobj)
	{
		BZBubble* pb = (BZBubble*)pobj;
		pb->onUpdate();
		if (BS_Died == pb->getState())
		{
			_bubblesDied->addObject(pb);
		}
	}
	CCARRAY_FOREACH(_bubblesDied, pobj)
	{
		BZBubble* pbubble = (BZBubble*)pobj;
		pbubble->setState(BS_Remove);
	}
	_bubblesDied->removeAllObjects();
	//_Trace("block #%02d update leave", debug_id());
}

void BZBlock::onBubblePositionChanged(BZBubble* pbubble, const CCPoint& posOld, const CCPoint& posNew)
{
	_Assert(this->_pboard);
	this->_pboard->onBubblePositionChanged(pbubble, posOld, posNew);
}
#endif
