
#include "BZGame.h"
#include "AStageLayer.h"
#include "AWorld.h"
#include "AString.h"
#include "AUserData.h"
#include "ADataBuf.h"

/// Game
BZGame::BZGame(CAStageLayer* player) : BZBoard(player)
{
	GUARD_FUNCTION();

	_name = "na";

	_state = GS_Idle;
	_nCounter = 0;

	_nScore = 0;
	_typesCount = 0;
	if (_typesCount < BLOCK_TYPES) _types[_typesCount++] = "bubble_a";	else _Assert(false);
	if (_typesCount < BLOCK_TYPES) _types[_typesCount++] = "bubble_b";	else _Assert(false);
	if (_typesCount < BLOCK_TYPES) _types[_typesCount++] = "bubble_c";	else _Assert(false);
	if (_typesCount < BLOCK_TYPES) _types[_typesCount++] = "bubble_d";	else _Assert(false);
	if (_typesCount < BLOCK_TYPES) _types[_typesCount++] = "bubble_e";	else _Assert(false);

	//_types[_typesCount++] = "bird";

	_timeLastBorn = 0;
}

BZGame::~BZGame()
{
	//if (_pboard)
	//{
	//	_pboard->release();
	//	_pboard = null;
	//}
}

//for debugging
string BZGame::debuglog()
{
	return BZBoard::debuglog();
}

int BZGame::_indexOfType(const char* type) const
{
	int i;
	for (i = 0; i < _typesCount; i++)
	{
		if (_types[i] == type)
			return i;
	}
	_Assert(false);
	return 0;
}

void BZGame::createBoard(const CCPoint& ptBoardAnchor, int rows, int cols, float bubblesize)
{
	GUARD_FUNCTION();
	BZBoard::setParams(ptBoardAnchor, rows, cols, bubblesize);
}

void BZGame::_onDetachBubbleSprite(BZBubble* pbubble)
{
	pbubble->detach(layer());
}

void BZGame::onEnter()
{
	_timeLastBorn = 0;

	//_params.timeDelayBorn = 0.2f;
	//_params.fPercentStarBorn = 45.0f;
	//_params.nRangeBubbleBorn = 3;

	//later, we will load this from xml
	_state = GS_Running;
}

void BZGame::onUpdate()
{
	_nCounter++;
	//
	if (GS_Running == _state)
	{
		_doBornStrategy();
	}
	//update something
	BZBoard::onUpdate();
}

bool BZGame::onEvent(const CAEvent* pevt)
{
	//_Assert(_pboard);
	return BZBoard::onEvent(pevt);
}

void BZGame::onExit()
{
	BZBoard::onExit();
}

void BZGame::clear()
{
	BZBoard::clear();
}

#if 0
void BZGame::loadData()
{
	GUARD_FUNCTION();

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
	//buf >> _nLevel;
	buf >> _nScore;

	//_Assert(null != _pboard);
	BZBoard::loadData(buf);
}

void BZGame::saveData()
{
	CADataBuf buf;
	
	buf << "1.0";
	buf << _name;
	buf << (this->getTimeNow() - _timeLastBorn);
	buf << _lastBubble;
	//buf << _nLevel;
	buf << _nScore;

	BZBoard::saveData(buf);

	string data = CAString::bin2str(buf.buf(), buf.posWrite());
	CAUserData::sharedUserData().setString(_name.c_str(), data);
}
#endif

BZBubble* BZGame::_onUpdateBlock(BZBlock* pblock)
{
	GUARD_FUNCTION();

	pblock->onUpdate();
	if (_canBoom(pblock))
	{
		return pblock->booom();
	}
	return null;
}

