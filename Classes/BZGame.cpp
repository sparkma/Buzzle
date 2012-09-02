
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
	_nStarsUsed = 3;

	_nScore = 0;
	_nLevel = 1;

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

void BZGame::createBoard(const CCPoint& ptLeftBottom, 
						 int rows, int cols, float bubblesize,
						 float zorder)
{
	GUARD_FUNCTION();

	_Assert(null == _pboard);
	_pboard = new BZBoard(this);
	_pboard->setParams(ptLeftBottom, rows, cols, bubblesize, zorder);
}

ccTime BZGame::getTimeNow() const
{ 
	_Assert(_pLayer);
	return _pLayer->getTimeNow(); 
}

void BZGame::onEnter()
{
	_timeLastBorn = 0;

	//_params.timeDelayBorn = 0.2f;
	//_params.fPercentStarBorn = 45.0f;
	//_params.nRangeBubbleBorn = 3;

	//later, we will load this from xml
	int n = 0;
	_types[n++] = "yellow";
	_types[n++] = "pink";
	_types[n++] = "blue";
	_types[n++] = "green";
	_types[n++] = "purple";
	//_types[n++] = "bird";
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

void BZGame::clear()
{
	_pboard->clear();
}

void BZGame::loadData()
{
	string str = CAUserData::sharedUserData().getString(_name.c_str());
	size_t len = str.length();
	len = (len + 16) * 3 / 4;
	unsigned char* pb = new unsigned char[len];
	unsigned int olen = CAString::str2bin(str, pb, len);
	_Assert(olen <= len);
	CADataBuf buf(pb, len);
	delete [] pb;

	ccTime time;
	buf >> str;	_Assert(str == "1.0");
	buf >> str; _Assert(str == _name);
	buf >> time; _timeLastBorn = this->getTimeNow() - time;
	buf >> _lastBubble;
	buf >> _nLevel;
	buf >> _nScore;

	_Assert(null != _pboard);
	_pboard->loadData(buf);
}

void BZGame::saveData()
{
	CADataBuf buf;
	
	buf << "1.0";
	buf << _name;
	buf << (this->getTimeNow() - _timeLastBorn);
	buf << _lastBubble;
	buf << _nLevel;
	buf << _nScore;

	_pboard->saveData(buf);

	string data = CAString::bin2str(buf.buf(), buf.posWrite());
	CAUserData::sharedUserData().setString(_name.c_str(), data);
}

bool BZGame::boomBlock(BZBlock* pblock)
{
	if (_canBoom(pblock))
	{
		int score = calculateScore(pblock);
		_nScore += score;
		_onScoreChanged();
		pblock->booom();

		return true;
	}
	return false;
}
