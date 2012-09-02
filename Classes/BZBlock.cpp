
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
	_bDirty = false;
	_stars = 0;
	_standing = 0;

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
	const string& pt = pbubble->getPropType();
	if (CAString::startWith(pt, PROP_STAR))
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

int BZBlock::getProps() const
{
	return 0;
}

void BZBlock::reset()
{
	_Assert(_bubbles->count() <= 0);
	_Assert(_stars == 0);
	_state = Block_Running;
	_bubbletype = "";
}

//you can call this function if _stars < 2
void BZBlock::booom()
{
	if (Block_Running != _state)
	{
		return;
	}

	_Trace("block #%02d booooom", this->debug_id());
	_state = Block_Boooming;

	CCPoint pos;
	pos.x = pos.y = 0;
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

		string pose = "fall0";
		
		BZSpriteCommon* pspr = new BZSpriteCommon(_pboard->game()->layer(), "spark0");
		pspr->setState(pose);
		pspr->setDeadPose(pose);
		CCPoint pos = pb->getPos();
		_pboard->getBubbleRenderPos(pos);
		pspr->setPos(pos);

		_pboard->game()->layer()->addSprite(pspr);
	}

	pos.x /= (_bubbles->count() + 0.0001f);
	pos.y /= (_bubbles->count() + 0.0001f);

	//block do not know how to calculate the score in diff mode.
	if (_pboard->game()->canShowBoomScore())
	{
		int score = _pboard->game()->calculateScore(this);
		char sz[16];
		sprintf(sz, "%d", score);
		int i, len = strlen(sz);
		float dx = 20.0f;
		_pboard->getBubbleRenderPos(pos);
		pos.x -= dx * len / 2;
		for (i = 0; i < len; i++)
		{
			BZSpriteCommon* pspr = new BZSpriteCommon(_pboard->game()->layer(), "number_3");
			char szPose[16];
			szPose[0] = sz[i];
			szPose[1] = 0;
			pspr->switchPose(szPose);
			pspr->setPos(pos);
			pos.x += dx;
			pos.y += 0.0f;
			pspr->setZOrder(120.0f);
			strcpy(szPose, "splash");
			pspr->setState(szPose);
			pspr->setDeadPose(szPose);
			_pboard->game()->layer()->addSprite(pspr);
		}
	}
}
