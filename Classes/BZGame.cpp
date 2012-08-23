
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
