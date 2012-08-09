
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
	_params._fPercentStar = 25.0f;
	_params._fRangeblockBorn = 3.0f;

	//later, we will load this from xml
	int n = 0;
	_types[n++] = "yellow";
	_types[n++] = "pink";
	_types[n++] = "blue";
}

/*
BZBlock* BZGame::_createUnmanagedBlock(const char* type)
{
	GUARD_FUNCTION();
	//block_xxxxxxx.spr
	string name = "block_";
	name += type;
	BZBlock* pblock = new BZBlock(this);
	pblock->setBlock(name.c_str());
	pblock->setFallingAcceleration(DEFAULT_ACCELERATION);
	pblock->setState(BS_Born);

	return pblock;
}
*/

void BZGame::_doBornStrategy()
{
#if defined(_DEBUG) && 0
	static int blocks = 0;
	if (blocks < 4) 
		;
	else
		return;
	ccTime time = _pLayer->getTimeNow();
	if (time - _timeLastBorn > _params._fDelayBlockBorn)
	{
		string type = _types[0];

		BZBlock* pblock = _createUnmanagedBlock(type.c_str());

		int slots[] = {6, 7, 8, 7};
		int slot = slots[blocks];

		_setBlock(0, slot, pblock);
		pblock->setInitialPosition(ccp(slot, 0));

		pblock->onUpdate();	//update real position of this block
		pblock->attachTo(_pLayer);
	
		blocks++;
	}
	return;
#else
	ccTime time = _pLayer->getTimeNow();
	if (time - _timeLastBorn > _params._fDelayBlockBorn && _pboard->getBubblesCount() < 5 * 8)
	{
		_timeLastBorn = time;

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
			BZBlockBubble* pb = _pboard->createBubble(0, slot, type.c_str(), pszStar);
			pb->setState(BS_Born);
		}
	}
#endif
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
