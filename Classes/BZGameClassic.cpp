
#include "BZGameClassic.h"
#include "AStageLayer.h"
#include "AWorld.h"


BZGameClassic::BZGameClassic(CAStageLayer* player)
: BZGame(player)
{
}

BZGameClassic::~BZGameClassic()
{
}

bool BZGameClassic::canBoom(BZBlock* pblock) const
{
	if (pblock->getStars() >= 2 && pblock->isAllStanding())
	{
		return true;
	}
	return false;
}

void BZGameClassic::_doBornStrategy()
{
	ccTime time = _pLayer->getTimeNow();
	if (time - _timeLastBorn > _params._fDelayBlockBorn && _pboard->getBubblesCount() < 7 * 5)
	{
		_timeLastBorn = time;

		bool star = CAUtils::Rand() * 100.0f < _params._fPercentStar;
		//select column first
		int typ = (int)CAUtils::Rand(0, (float)_params._fRangeblockBorn);
		_Assert(typ >= 0 && typ < BLOCK_TYPES);
		string type = "bubble_";
		type += _types[typ];

		//how many free slots 
		int free = 0;
		int slots[64];
		free = _pboard->getEmptyBornSlots(slots, 64);

		//create a block+group+props if there is a free slot
		if (free > 0)
		{
			//rand a slot
			int rand = (int)CAUtils::Rand(0, (float)free);
			int slot = slots[rand];
			const char* pszStar = null;
			char szStar[16];
			if (star)
			{
				int n = (int)CAUtils::Rand(0, 3.0f);
				sprintf(szStar, "star%d", n);
				pszStar = szStar;
			}
			BZBubble* pb = _pboard->createBubble(0, slot, type.c_str(), pszStar);
		}
	}
}

void BZGameClassic::onEvent(CAEvent* pevt)
{
	BZGame::onEvent(pevt);

	switch (pevt->type())
	{
	case ET_Touch:
		{
			CAEventTouch* ptouch = (CAEventTouch*)pevt;
			_Assert(ptouch->fingler() >= 0 && ptouch->fingler() < 5);
			switch (ptouch->state())
			{
			case kTouchStateGrabbed:
				{
					CCSize size = CAWorld::sharedWorld().getScreenSize();
					if ((ptouch->pt().x > size.width * 0.8f) &&
						(ptouch->pt().y > size.height * 0.9f))
					{
						_doBornStrategy();
					}
				}
				break;
			}
		}
		break;
	}
}
