
#include "BZSpriteStarHolder.h"
#include "AStageLayer.h"
#include "AWorld.h"
#include "AUserData.h"

BZSpriteStarHolder::BZSpriteStarHolder(CAStageLayer* player, const char* name) : BZSpriteCommon(player, name)
{
	_pieces = CCArray::create();
	_pieces->retain();

	_timeFilling = 0;

	_setState(ESHS_Blank);
}

BZSpriteStarHolder::~BZSpriteStarHolder(void)
{
	if (_pieces)
	{
		_pieces->release();
		_pieces = null;
	}
}

void BZSpriteStarHolder::reset()
{
	this->setState("off");
	_setState(ESHS_Blank);
}

void BZSpriteStarHolder::addStar(const string& name, const CCPoint& pos)
{
	_Assert(ESHS_Blank == _item_state);

	CAStageLayer* pl = this->layer();
	CASprite* pspr = CAWorld::sharedWorld()->createSprite(pl, name.c_str());
	pspr->setVertexZ(this->getVertexZ() + 10.0f);
	pspr->setPos(pos);
	pspr->setState("fly");
	pl->addSprite(pspr);
	_pieces->addObject(pspr);

	CCPoint pp = pspr->getPos();
	_Info("star pos (%.2f,%.2f), inpos (%.2f,%.2f)", pp.x, pp.y, pos.x, pos.y);

	_setState(ESHS_Filling);
}

void BZSpriteStarHolder::_setState(EStarHolderState s)
{ 
	_item_state = s;
}

void BZSpriteStarHolder::onEnter()
{
	BZSpriteCommon::onEnter();
}

void BZSpriteStarHolder::onUpdate()
{
	BZSpriteCommon::onUpdate();

	switch (_getState())
	{
	case ESHS_Blank:
		break;
	case ESHS_Filling:
		{
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

				_Info("star (%.2f) (%.2f,%.2f) (%.2f,%.2f) (%.2f,%.2f)", p0, pos.x, pos.y, posBegin.x, posBegin.y, posTo.x, posTo.y);

				pspr->setPos(pos);
				if (pspr->isAnimationDone())
				{
					pspr->setPos(posTo);
					pspr->killMyself();
					psprsRemoved[n++] = pspr;

					this->setState("on");
					_setState(ESHS_Filled);
				}
			}
			while (n > 0)
			{
				CASprite* pspr = psprsRemoved[n - 1];
				_pieces->removeObject(pspr);
				n--;
			}
		}
		break;
	case ESHS_Filled:
		break;
	}
}


#include "AWorld.h"
REG_CLASS(BZSpriteStarHolder);
