
#include "BZBlock.h"
#include "BZBoard.h"
#include "BZBlockBubble.h"
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

	_bubblesDied = CCArray::create(4);
	_bubblesDied->retain();

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

	_Assert(_bubblesDied);
	_bubblesDied->release(); 
	_bubblesDied = null;
}

void BZBlock::verify()
{
	_Assert(_bubbles->count() > 0 || Block_Boooming == _state);

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
	_pboard->onBlockStateChanged(this);

	pblock->_bubbles->removeAllObjects();
	pblock->_stars = 0;
	pblock->_pboard->onBlockStateChanged(pblock);
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
	_pboard->onBlockStateChanged(this);
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
	_pboard->onBlockStateChanged(this);
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
		BZBlockBubble* pb = (BZBlockBubble*)pobj;
		EBubbleState s = pb->getState();
		if (s < BS_Die)
		{
			pb->setState(BS_Die);
		}
	}
}

void BZBlock::onUpdate()
{
	//_Trace("block #%02d update enter", debug_id());
	if (Block_Running != _state && Block_Boooming != _state)
		return;

	_Assert(_bubblesDied->count() == 0);

	CAObject* pobj;
	CCARRAY_FOREACH(_bubbles, pobj)
	{
		BZBlockBubble* pb = (BZBlockBubble*)pobj;
		pb->onUpdate();
		if (BS_Died == pb->getState())
		{
			_bubblesDied->addObject(pb);
		}
	}
	CCARRAY_FOREACH(_bubblesDied, pobj)
	{
		BZBlockBubble* pbubble = (BZBlockBubble*)pobj;
		pbubble->setState(BS_Remove);
	}
	_bubblesDied->removeAllObjects();
	//_Trace("block #%02d update leave", debug_id());
}

void BZBlock::onBubblePositionChanged(BZBlockBubble* pbubble, const CCPoint& posOld, const CCPoint& posNew)
{
	_Assert(this->_pboard);
	this->_pboard->onBubblePositionChanged(pbubble, posOld, posNew);
}
