
#include "BZGame.h"
#include "AStageLayer.h"
#include "AWorld.h"


/// Game
BZGame::BZGame(CAStageLayer* player)
{
	GUARD_FUNCTION();

	_pLayer = player;
	_pboard = null;

	_nScores = 0;
	_timeLastBorn = 0;
}

BZGame::~BZGame()
{
	if (_pboard)
	{
		_pboard->release();
		_pboard = null;
	}
}

//for debugging
string BZGame::debuglog()
{
	return _pboard ? _pboard->debuglog() : "";
}

void BZGame::createBoard(const CCPoint& ptLeftTop, int rows, int cols, float bubblesize)
{
	GUARD_FUNCTION();

	_Assert(null == _pboard);
	_pboard = new BZBoard(this);
	_pboard->setParams(ptLeftTop, rows, cols, bubblesize);
}

ccTime BZGame::getTimeNow() const
{ 
	_Assert(_pLayer);
	return _pLayer->getTimeNow(); 
}

void BZGame::onEnter()
{
	_timeLastBorn = 0;

	_params._fDelayBlockBorn = 0.2f;
	_params._fPercentStar = 50.0f;
	_params._fRangeblockBorn = 3.0f;

	//later, we will load this from xml
	int n = 0;
	_types[n++] = "yellow";
	_types[n++] = "pink";
	_types[n++] = "blue";
}

void BZGame::_bornOne()
{
	bool star = CAUtils::Rand() * 100.0f < _params._fPercentStar;
	//select column first
	int typ = (int)CAUtils::Rand(0, (float)_params._fRangeblockBorn);
	_Assert(typ >= 0 && typ < BLOCK_TYPES);
	string type = "block_";
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
		pb->setState(BS_Born);
	}
}

void BZGame::_doBornStrategy()
{
	ccTime time = _pLayer->getTimeNow();
	if (time - _timeLastBorn > _params._fDelayBlockBorn && _pboard->getBubblesCount() < 1)
	{
		_timeLastBorn = time;
		_bornOne();
	}
}

void BZGame::onUpdate()
{
	//
	_doBornStrategy();
	//update something
	_pboard->onUpdate();
}

void BZGame::onEvent(CAEvent* pevt)
{
	_Assert(_pboard);
	_pboard->onEvent(pevt);
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
						_bornOne();
					}
				}
				break;
			}
		}
		break;
	}
}

void BZGame::onExit()
{
	_pboard->onExit();
}

BZGameClassic::BZGameClassic(CAStageLayer* player)
: BZGame(player)
{
}

BZGameClassic::~BZGameClassic()
{
}
