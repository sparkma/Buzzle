
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
	//_pboard = null;
	_nStarsUsed = 3;

	_nScore = 0;

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

void BZGame::createBoard(const CCPoint& ptLeftBottom, 
						 int rows, int cols, float bubblesize,
						 float zorder)
{
	GUARD_FUNCTION();

	//_Assert(null == _pboard);
	//_pboard = new BZBoard(this);
	BZBoard::setParams(ptLeftBottom, rows, cols, bubblesize, zorder);
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

	if (_canBoom(pblock))
	{
		int score = calculateScore(pblock);
		_nScore += score;
		_onScoreChanged();

		BZBubble* pbCenter = pblock->booom();
		if (null == pbCenter)
			return null;

		CCPoint posCenter = pbCenter->getPos();

		//block do not know how to calculate the score in diff mode.
		if (canShowBoomScore())
		{
			char sz[16];
			sprintf(sz, "%d", score);
			int i, len = strlen(sz);
			float dx = 20.0f;
			BZBoard::getBubbleRenderPos(posCenter);
			posCenter.x -= dx * len / 2;
			for (i = 0; i < len; i++)
			{
				BZSpriteCommon* pspr = new BZSpriteCommon(layer(), "number_3");
				char szPose[16];
				szPose[0] = sz[i];
				szPose[1] = 0;
				pspr->switchPose(szPose);
				pspr->setPos(posCenter);
				posCenter.x += dx;
				posCenter.y += 0.0f;
				//pspr->setZOrder(120.0f);
				//***
				pspr->setVertexZ(120.0f);
				strcpy(szPose, "dead");
				pspr->pushState(szPose);
				//pspr->setDeadPose(szPose);
				layer()->addSprite(pspr);
			}
		}

		return pbCenter;
	}
	return null;
}

BZSpriteCommon* BZGame::addGlobalEffect(const CCPoint& pos_, const char* effect, const char* pose)
{
	GUARD_FUNCTION();

	BZSpriteCommon* pspr = new BZSpriteCommon(layer(), effect);

	CCPoint pos = pos_;
	pspr->setPos(pos);

	pspr->pushState(pose);
	//pspr->setDeadPose(pose);

	layer()->addSprite(pspr);

	return pspr;
}

int BZGame::getEffectedBlock(BZBubble* pbCheck, int range, BZBubble** pbEffected, int esize)
{
	int n = 0;
	int r0 = pbCheck->getIndexRow();
	int c0 = pbCheck->getIndexColumn();
	int r, c;
	for (r = r0 - range; r < r0 + range; r++)
	{
		for (c = c0 - range; c < c0 + range; c++)
		{
			int dr = r - r0;
			int dc = c - c0;
			if (dr * dr + dc * dc > range * range)
				continue;

			BZBubble* pb = BZBoard::_getBubble(r, c);
			if (null == pb)
				continue;
			if (pb->getBubbleType() != pbCheck->getBubbleType() && pb->getBlock() != pbCheck->getBlock())
			{
				if (n < esize)
				{
					pbEffected[n++] = pb;
				}
			}
		}
	}
	return n;
}

