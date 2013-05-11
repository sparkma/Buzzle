
#include "BZSpriteButtonItem.h"
#include "AStageLayer.h"
#include "AWorld.h"
#include "AUserData.h"

#define NAME_PP "flying__pieces"

BZSpriteButtonItem::BZSpriteButtonItem(CAStageLayer* player, const char* name) : BZSpriteButton(player, name)
{
	_pieces = CCArray::create();
	_pieces->retain();
	_pcount = 0;
	//_onPieceCountChanged();
}

BZSpriteButtonItem::~BZSpriteButtonItem(void)
{
	if (_pieces)
	{
		_pieces->release();
		_pieces = null;
	}
}

void BZSpriteButtonItem::setDifficulty(const string& diff)
{
	_difficulty = diff;
	string key = this->getModName() + diff;
	_pcount = CAUserData::sharedUserData().getInteger(key.c_str(), 0);
	_onPieceCountChanged();
}

void BZSpriteButtonItem::_onPieceCountChanged()
{
#if defined(_DEBUG)
#else
	this->enable(_pcount == 5);
#endif

	string key = this->getModName() + _difficulty;
	_Assert(_difficulty.length());
	_Assert(key.length());
	{
		CAUserData::sharedUserData().setInteger(key.c_str(), _pcount);
	}

	char sz[16];
	
	int c = _pcount;
	if (c < 0) c = 0;
	else if (c > 5) c = 5;

	sprintf(sz, "6%d", c);
	this->setState(sz);
}

void BZSpriteButtonItem::_onClick()
{
	BZSpriteButton::_onClick();
	_pcount = 0;
	_onPieceCountChanged();
}

void BZSpriteButtonItem::addPiece(const string& name, const CCPoint& pos)
{
	CAStageLayer* pl = this->layer();
	CASprite* pspr = CAWorld::sharedWorld().createSprite(pl, name.c_str());
	pspr->setGroupName(NAME_PP);
	pspr->setVertexZ(this->getVertexZ() + 10.0f);
	pspr->setPos(pos);
	pspr->setState("fly");
	pl->addSprite(pspr);
	_pieces->addObject(pspr);
}

void BZSpriteButtonItem::onUpdate()
{
	BZSpriteButton::onUpdate();

	int n = 0;
	CASprite* psprsRemoved[256];
	CAObject* pobj;
	CCARRAY_FOREACH(_pieces, pobj)
	{
		CASprite* pspr = (CASprite*)pobj;
		float p0 = pspr->getAnimationPercent(this->layer()->getTimeNow());
		float p = p0 + 0.1f;
		if (p > 1.0f) p = 1.0f;

		CCPoint pos;
		CCPoint posBegin = pspr->getPos();
		CCPoint posTo = getPos();
		pos.x = posBegin.x + (posTo.x - posBegin.x) * p;
		pos.y = posBegin.y + (posTo.y - posBegin.y) * p;

		_Info("piece(%.2f) %.2f,%.2f", p0, pos.x, pos.y);

		pspr->setPos(pos);
		if (pspr->isAnimationDone())
		{
			if (_pcount < 5) _pcount++;
			_onPieceCountChanged();

			pspr->setPos(posTo);
			pspr->killMyself();
			psprsRemoved[n++] = pspr;
		}
	}
	while (n > 0)
	{
		CASprite* pspr = psprsRemoved[n - 1];
		_pieces->removeObject(pspr);
		n--;
	}
}


#include "AWorld.h"
REG_CLASS(BZSpriteButtonItem);
