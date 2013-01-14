
#include "BZGameTapBoom.h"
#include "AStageLayer.h"
#include "AWorld.h"


BZGameTapBoom::BZGameTapBoom(CAStageLayer* player)
: BZGame(player)
{
	_name = "tapboom";
}

BZGameTapBoom::~BZGameTapBoom()
{
}

void BZGameTapBoom::_doBornStrategy()
{
	ccTime time = _pLayer->getTimeNow();
	if (time - _timeLastBorn > _params.timeDelayBorn && _pboard->getBubblesCount() < 7 * 5)
	{
		_timeLastBorn = time;

		bool star = CAUtils::Rand() * 100.0f < _params.fPercentStarBorn;
		//select column first
		int typ = (int)CAUtils::Rand(0, (float)_params.nRangeBubbleBorn);
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
			BZBubble* pb = _pboard->createBubble(type.c_str(), ccp(slot, 0), pszStar);
		}
	}
}

