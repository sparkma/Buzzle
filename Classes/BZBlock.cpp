
#include "BZBlock.h"
#include "BZBlockBubble.h"
#include "BZBlockProp.h"
#include "BZDoodad.h"
#include "AStageLayer.h"
#include "AWorld.h"

BZBlock::BZBlock(BZBoard* pboard)
{
	_pboard = pboard;

	_bubbles = CCArray::arrayWithCapacity(4);
	_bubbles->retain();
	_props = CCArray::arrayWithCapacity(4);
	_props->retain();
	_doodads = CCArray::arrayWithCapacity(4);;
	_doodads->retain();
}

BZBlock::~BZBlock()
{
	_bubbles->release(); _bubbles = null;
	_props->release(); _props = null;
	_doodads->release(); _doodads = null;
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

	objs = pblock->getProps();
	CCARRAY_FOREACH(objs, pobj)
	{
		_props->addObject(pobj);
	}

	objs = pblock->getProps();
	CCARRAY_FOREACH(objs, pobj)
	{
		_doodads->addObject(pobj);
	}
}

void BZBlock::attachBubble(BZBlockBubble* pbubble) 
{ 
	_Assert(_bubbles); 
	_bubbles->addObject(pbubble);
}

void BZBlock::attachProp(BZBlockProp* pprop) 
{ 
	_Assert(_props); 
	_props->addObject(pprop);
}

void BZBlock::attachDoodad(BZDoodad* pdoodad) 
{ 
	_Assert(_doodads); 
	_doodads->addObject(pdoodad);
}

