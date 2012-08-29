
#include "BZGame.h"
#include "AStageLayer.h"
#include "AWorld.h"
#include "AString.h"
#include "AUserData.h"
#include "ADataBuf.h"

/// Game
BZGame::BZGame(CAStageLayer* player)
{
	GUARD_FUNCTION();

	_pLayer = player;
	_name = "na";
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

void BZGame::createBoard(const CCPoint& ptLeftTop, 
						 int rows, int cols, float bubblesize,
						 float zorder)
{
	GUARD_FUNCTION();

	_Assert(null == _pboard);
	_pboard = new BZBoard(this);
	_pboard->setParams(ptLeftTop, rows, cols, bubblesize, zorder);
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

void BZGame::loadData()
{
}

void BZGame::saveData()
{
	CADataBuf buf;
	
	buf << "1.0";
	buf << _name;
	buf << _timeLastBorn;
	buf << _lastBubble;
	buf << _level;
	buf << _nScores;

	_pboard->saveData(buf);

	string data = CAString::bin2str(buf.buf(), buf.posWrite());
	CAUserData::sharedUserData().setString(_name.c_str(), data);
}

/*
#if defined(_DEBUG)
	{
		string str;
		_pboard->saveDataIntoString(str);
		string str64 = CAString::bin2str(str.c_str(), str.size(), 64);
		char sz[8192];
		unsigned int o = CAString::str2bin(str64, sz, sizeof(sz));
		sz[o] = 0;
		string str2 = sz;
		bool be = str == str2;
		_Assert(be);
		str = "";
	}
#endif
*/