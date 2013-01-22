
#include "BZGameClassic.h"
#include "AStageLayer.h"
#include "AWorld.h"


BZGameClassic::BZGameClassic(CAStageLayer* player)
: BZGame(player)
{
	_name = "classic";

	_mapProcessed = 0;
	//_bIsHeaderlineFull = false;
	_timeLastRow = 0;

	_nLevel = 1;
	_nCombo = 0;
	_nPrebornLines = 0;

	_bubbleGenerated = 0;
	_lastStarIndex = 0;

	memset(_bubblesGrabbed, 0, sizeof(_bubblesGrabbed));
}

BZGameClassic::~BZGameClassic()
{
}

void BZGameClassic::clear()
{
	BZGame::clear();

	int r;
	for (r = 0; r < _MAX_GRABBED_BLOCKS; r++)
	{
		BZBubble* pbubble = _getGrabbedBubble(r);
		if (pbubble)
		{
			//loop-ref, block and bubble don't wanna to release first, just decrease ref-count
			pbubble->setBlock(null);
			_setGrabbedBubble(r, null);
		}
	}
}

bool BZGameClassic::_canBoom(BZBlock* pblock) const
{
	if (pblock->getStars() >= 2 && pblock->isAllStanding() && pblock->getState() == Block_Running)
	{
		return true;
	}
	return false;
}

void BZGameClassic::initLevelParams(
	int levels, int bubble_score, int level_base_score, int level_mul_score,
	const BZLevelParams& levelmin, const BZLevelParams& levelmax)
{
	_levels = levels;
	_bubble_score = bubble_score;
	_level_base_score = level_base_score;
	_level_mul_score = level_mul_score;
	_paramsPreloaded[0] = levelmin;
	_paramsPreloaded[1] = levelmax;

	this->_onLevelChanged();
}

static const char* _getPropStarName(char* szStar)
{
	int n = (int)CAUtils::Rand(0, 3.0f);
	sprintf(szStar, "prop_star%02d", n + 1);
	return szStar;
}

void BZGameClassic::_handleBornStrategyLevel1()
{
	GUARD_FUNCTION();

	_Assert(_nLevel == 1);
	if (_mapProcessed < (int)_mapLevel1.length())
	{
		//use the map
		int i, c = _mapLevel1.length();
		const char* psz = _mapLevel1.c_str();
		for (i = _mapProcessed; i <= c - 2; i += 2)
		{
			int slots[64];
			int free = BZBoard::getEmptyBornSlots(slots, 64);
			//if (free < _pboard->getColumns())
			if (free <= 0)
				break;

			int col = psz[i]; 
			col -= '1';
			if (BZBoard::_getBubble(0, col))
			{
				break;
			}
			//_Info("process level1 map: [%02d] %c%c", i, psz[i], psz[i + 1]);

			int flag = psz[i + 1];
			string type;
			bool star = false;
			if (flag >= 'A' && flag <= 'C')
			{
				flag -= 'A';
				flag += 'a';
				star = true;
			}
			if (flag >= 'a' && flag <= 'c')
			{
				type = "bubble_";
				type += _types[flag - 'a'];
			}

			const char* pszStar = null;
			char szStar[16]; 
			if (star)
			{
				pszStar = _getPropStarName(szStar);
			}

			BZBubble* pb = BZBoard::createBornBubble(type.c_str(), col, pszStar);
		}
		_mapProcessed = i;
		BZBoard::fallOneRow();
		if (!(_mapProcessed < (int)_mapLevel1.length()))
		{
			ccTime timeNow = this->getTimeNow();
			_timeLastRow = timeNow;
		}
	}
	else
	{
		//else, fall some bubbles to fill the board
		_handleBornStrategyLevelN();
	}
}

bool BZGameClassic::_generateBubble(int& col, string& type, bool& star)
{
	GUARD_FUNCTION();

	_bubbleGenerated++;
	//select column first
	int typ = (int)CAUtils::Rand(0, (float)min(BLOCK_TYPES, _params.nRangeBubbleBorn));
	_Assert(typ >= 0 && typ < BLOCK_TYPES);
	type = "bubble_";
	type += _types[typ];
	star = CAUtils::Rand() * 100.0f < _params.fPercentStarBorn;
	if (!star)
	{
#if 1
		int stars = BZBoard::getStarsCount(type.c_str());
		if (stars < _params.nMinStarsInOneBubbleType)
		{
			star = true;
		}
		else
		{

		}
#else
		//comfirm stars count per block 
#endif
	}
	return true;
}

void BZGameClassic::_handleBornStrategyLevelN()
{
	GUARD_FUNCTION();

	ccTime time = _pLayer->getTimeNow();
	int bubbles = BZBoard::getBubblesCount();
	if (time - _timeLastBorn > _params.timeDelayBorn)
	{
		//how many free slots 
		int free = 0;
		int slots[64];
		free = BZBoard::getEmptyBornSlots(slots, 64);
		if (free > 0)
		{
			int rand = (int)CAUtils::Rand(0, (float)free);
			int col = slots[rand];

			string type = "";
			bool star = false;
			if (_generateBubble(col, type, star))
			{
				_timeLastBorn = time;
				const char* pszStar = null;
				char szStar[16]; 
				if (star)
				{
					pszStar = _getPropStarName(szStar);
				}

				BZBubble* pb = BZBoard::createBornBubble(type.c_str(), col, pszStar);
				bool bRainfall = (0 == (_nLevel % 10));
				if (1 == _nLevel && _mapProcessed < (int)_mapLevel1.length())
				{
					BZBoard::fallOneRow();
					bRainfall = true;
				}
				pb->setRainfallMode(bRainfall);
			}
		}
		else
		{
			//header line is full
			//_bIsHeaderlineFull = true;

			bool bRainfall = (0 == (_nLevel % 10));
			if (!bRainfall && BZBoard::isHeaderLineFull())
			{
				ccTime timeNow = this->getTimeNow();
				float fDelay = _params.fDelayOneRow;
				if (_nPrebornLines > 0)
				{
					fDelay = _params.fDelayOneRow / 120.0f; //speed up 16 times
					if (fDelay < 0.2f) fDelay = 0.1f;
					//不可以是0， 否则会淹没出生行，造 成丢失
				}
				if (timeNow - _timeLastRow > fDelay)
				{
					if (_nPrebornLines > 0) _nPrebornLines--;
					_Info("_nPrebornLines = %d", _nPrebornLines);
					BZBoard::fallOneRow();
					_timeLastRow = timeNow;
				}
			}
		}
	}
}

void BZGameClassic::_doBornStrategy()
{
	GUARD_FUNCTION();

	switch (_nLevel)
	{
	case 1:
		_handleBornStrategyLevel1();
		break;
	default:
		_handleBornStrategyLevelN();
		break;
	}
}

bool BZGameClassic::onEvent(const CAEvent* pevt)
{
	bool ret = BZGame::onEvent(pevt);

	switch (pevt->type())
	{
	case ET_Touch:
		{
			CAEventTouch* ptouch = (CAEventTouch*)pevt;
			_Assert(ptouch->fingler() >= 0 && ptouch->fingler() < 5);
			switch (ptouch->state())
			{
			case kTouchStateGrabbed:
				_onTouchGrabbed(ptouch);
				break;
			case kTouchStateMoving:
				_onTouchMoving(ptouch);
				break;
			case kTouchStateUngrabbed:
				_onTouchUngrabbed(ptouch);
				break;
			}
		}
		break;
	}
	return true;
}

int BZGameClassic::calculateScore(BZBlock* pblock) const
{
	int c = pblock->getBubbles()->count();
	int score = c * c * _bubble_score;	
	return score;
}

void BZGameClassic::_onScoreChanged()
{
	//score 2 level
	int levelScore = _nLevel * _nLevel * _level_mul_score + _level_base_score;
	if (_nScore > levelScore)
	{
		_nLevel++;
		_onLevelChanged();
	}
}

float BZGameClassic::getLevelPercent() const
{
	int levelScore0;
	int levelScore1;

	if (_nLevel > 1)
	{
		levelScore0 = (_nLevel - 1) * (_nLevel - 1) * _level_mul_score + _level_base_score;
	}
	else
	{
		levelScore0 = 0;
	}
	levelScore1 = (_nLevel - 0) * (_nLevel - 0) * _level_mul_score + _level_base_score;

	float p = (float)(_nScore - levelScore0) / (float)(levelScore1 - levelScore0);
	return p;
}

#define _LERP_LEVEL_PARAM(param) \
		((float)_paramsPreloaded[0].param + ((float)_paramsPreloaded[1].param - (float)_paramsPreloaded[0].param) * (_nLevel - 1) / (float)_levels)

void BZGameClassic::_onLevelChanged()
{
	GUARD_FUNCTION();

	BZGame::_onLevelChanged();
	BZLevelParams params;
	
	params.fDelayOneRow = _LERP_LEVEL_PARAM(fDelayOneRow);
	params.nMinStarsInOneBubbleType	= (int)_LERP_LEVEL_PARAM(nMinStarsInOneBubbleType);
	params.fPercentStarBorn	= _LERP_LEVEL_PARAM(fPercentStarBorn);
	params.nRangeBubbleBorn	= (int)_LERP_LEVEL_PARAM(nRangeBubbleBorn);
	params.fPrebornLines	= _LERP_LEVEL_PARAM(fPrebornLines);
	params.timeDelayBorn	= _LERP_LEVEL_PARAM(timeDelayBorn);

	this->setLevelParams(params);

	//this->dispatchEvent(new CAEventCommand(this, ST_UserDefined, "levelup", &_nLevel));

	if (_nLevel > 1)
	{
		CCPoint pos = ccp(0.5, 0.7);
		CAWorld::percent2view(pos);
		BZSpriteCommon* pspr = addGlobalEffect(pos, "levelup", "fadein");
		pspr->pushState("stand");
		pspr->pushState("fadeout");

		//remove all bubbles
		int r, c;
		for (r = 0; r < BZBoard::getRows(); r++)
		{
			for (c = 0; c < BZBoard::getColumns(); c++)
			{
				BZBubble* pb = BZBoard::_getBubble(r, c);
				if (null != pb)
				{
					_addFireEffectOn(pb);
					pb->setState(BS_Die);
				}
			}
		}
		//calculate next level born strategy
		//how many lines to push down?
		_nPrebornLines = (int)params.fPrebornLines;
	}
}

void BZGameClassic::_addFireEffectOn(BZBubble* pb)
{
	GUARD_FUNCTION();

	int rand;
	rand = (int)CAUtils::Rand(0, (float)4.0f);
	char szPose[8];
	_Assert(rand >= 0 && rand < 4);
	sprintf(szPose, "b%d", rand + 1);
	pb->addEffect("effect_fire", szPose, true);
}

BZBubble* BZGameClassic::_onUpdateBlock(BZBlock* pblock)
{
	GUARD_FUNCTION();

	BZBubble* pbCenter = BZGame::_onUpdateBlock(pblock);
	if (null == pbCenter)
	{
		return null;
	}

	int i;
	int rand;

	//CAStringMap<CAInteger> props;

	CCArray* _bubbles = pblock->getBubbles();
	CAObject* pobj;
	CCARRAY_FOREACH(_bubbles, pobj)
	{
		BZBubble* pb = (BZBubble*)pobj;

		//collect prop here
		const string& prop = pb->getPropType();

		if (prop == "prop_bomb_oooo")
		{
			//fire block edge
			_addFireEffectOn(pbCenter);
			_Info("effect FIRE");

			CCObject* psub;
			CCARRAY_FOREACH(_bubbles, psub)
			{
				BZBubble* pbCheck = (BZBubble*)psub;
				BZBubble* pbEffected[256];
				int e = getEffectedBlock(pbCheck, 1, pbEffected, SIZE_OF_ARRAY(pbEffected));
				for (i = 0; i < e; i++)
				{
					_addFireEffectOn(pbEffected[i]);
					pbEffected[i]->setState(BS_Die);
				}
			}
		}
		else if (prop == "prop_bomb_oxox")
		{
			CCPoint pos = pb->getPos();
			//6 ==> 2.5, 7 ==> 3
			//(n - 1) / 2
			pos.x = ((float)BZBoard::getColumns() - 1.0f) / 2.0f;
			BZBoard::getBubbleRenderPos(pos);
			addGlobalEffect(pos, "effect_light", "b1");
			_Info("effect LIGHT Horz");
			for (i = 0; i < BZBoard::getColumns(); i++)
			{
				BZBubble* pbE = BZBoard::_getBubble(pb->getIndexRow(), i);
				if (null != pbE && pbE->getBlock() != pb->getBlock())
				{
					_addFireEffectOn(pbE);
					pbE->setState(BS_Die);
				}
			}
		}
		else if (prop == "prop_bomb_xoxo")
		{
			CCPoint pos = pb->getPos();
			//6 ==> 2.5, 7 ==> 3
			//(n - 1) / 2
			pos.y = ((float)BZBoard::getRows() - 1.0f) / 2.0f;
			BZBoard::getBubbleRenderPos(pos);
			addGlobalEffect(pos, "effect_light", "b2");
			_Info("effect LIGHT VERT");
			for (i = 0; i < BZBoard::getRows(); i++)
			{
				BZBubble* pbE = BZBoard::_getBubble(i, pb->getIndexColumn());
				if (null != pbE && pbE->getBlock() != pb->getBlock())
				{
					_addFireEffectOn(pbE);
					pbE->setState(BS_Die);
				}
			}
		}
		else if (prop == "prop_bomb_wheel")
		{
			pbCenter->addEffect("effect_wheel", "b1", false);
			_Info("effect WHEEL");
		}
		else if (prop == "prop_star01") {}
		else if (prop == "prop_star02") {}
		else if (prop == "prop_star03") {}
		else if (prop == "") {}
		else
		{
			_Assert(false);
		}

		/*
		if (prop.size() > 0)
		{
			CAInteger* pi = props.objectForKey(prop);
			if (null == pi)
			{
				pi = CAInteger::create(1);
				props.setObject(pi, prop);
			}
			pi->set(pi->get() + 1);
		}
		*/
		//prcocess props and common-effects
		for (i = 0; i < 2; i++)
		{
			rand = (int)CAUtils::Rand(0, 7);
			char szMod[32];
			sprintf(szMod, "effect_boom%02d", rand + 1);
			char szPose[32];
			rand = (int)CAUtils::Rand(0, 3);
			sprintf(szPose, "b%d", rand + 1);
			string pose = szPose;

			pb->addEffect(szMod, szPose, true);
		}
	}

	//CCARRAY_FOREACH(_bubbles, pobj)
	//{
	//	BZBubble* pb = (BZBubble*)pobj;
	//}

	int bc = _bubbles->count();
	const char* prop = null;
	switch (bc)
	{
	case 2:
	case 3:
	case 4:
		break;
	case 5:
		//effect 0-6, 3-4
		prop = "prop_bomb_oxox";
		break;
	case 6:
	case 7:
		//effect 0-9, 3-6
		prop = "prop_bomb_xoxo";
		break;
	case 8:
	case 9:
	default:
		rand = (int)CAUtils::Rand(0, 2);
		_Assert(rand >= 0 && rand < 2);
		if (rand)
		{
			//r=1
			//effect 8-22 8-13
			prop = "prop_bomb_oooo";
		}
		else
		{
			//r=2.5
			//effect 0-20 3-20
			prop = "prop_bomb_wheel";
		}
		break;
	}
	if (null != prop)
	{
		pbCenter->setRebornBubble(pblock->getBubbleType().c_str(), prop);
	}
	return pbCenter;
}

bool BZGameClassic::isGameOver() const
{
	int rows = BZBoard::getRows();
	int cols = BZBoard::getColumns();
	unsigned int bc = BZBoard::getBubblesCount();
	if (rows * cols == bc)
	{
		//if all bubbles are standing?
		int r, c;
		for (r = 0; r < rows; r++)
		{
			for (c = 0; c < cols; c++)
			{
				BZBubble* pb = BZBoard::_getBubble(r, c);
				_Assert(null != pb);
				if (pb->getState() != BS_Standing)
					return false;
			}
		}
		return true;
	}
	return false;
}


BZBubble* BZGameClassic::_getGrabbedBubble(int finger)
{
	if (finger >= 0 && finger < _MAX_GRABBED_BLOCKS)
	{
		return _bubblesGrabbed[finger];
	}
	return null;
}

void BZGameClassic::_setGrabbedBubble(int finger, BZBubble* pbubble)
{
	if (finger >= 0 && finger < _MAX_GRABBED_BLOCKS)
	{
	}
	else
	{
		return;
	}
	if (pbubble)
	{
		_Assert(null == _bubblesGrabbed[finger]);
		_bubblesGrabbed[finger] = pbubble;
		pbubble->retain();
	}
	else
	{
		if (null != _bubblesGrabbed[finger])
		{
			_bubblesGrabbed[finger]->release();
			_bubblesGrabbed[finger] = null;
		}
	}
}


void BZGameClassic::_onTouchGrabbed(CAEventTouch* ptouch)
{
	BZBubble* pbubble = _getBubbleByPoint(ptouch->pt());
	//block could be null
	if (null != pbubble)
	{
		//_Trace("bubble #%02d (%d,%d) is grabbed", pbubble->debug_id(),
		//	pbubble->getIndexRow(), pbubble->getIndexColumn());
		if (pbubble->canMove())
		{
			pbubble->setState(BS_Drag);
		}
		_setGrabbedBubble(ptouch->fingler(), pbubble);
	}
}

bool BZGameClassic::_hasBeenOccupied(int r, int c, BZBubble* pbExclude)
{
	if (!_IS_IN_BOARD(r, c))
		return true;
	BZBubble* pb = _getBubble(r, c);
	if (null == pb)
		return false;
	if (null != pbExclude && pb == pbExclude)
		return false;
	return true;
}

void BZGameClassic::_onTouchMoving(CAEventTouch* ptouch)
{
	//find if this finger has grabbed a block?
	BZBubble* pbubble = _getGrabbedBubble(ptouch->fingler());
	if (null == pbubble)
	{
		//we can try grab another one in moving state
		//_onTouchGrabbed(ptouch);
	}
	else
	{
		EBubbleState s = pbubble->getState();
		if (BS_Dragging != s)
			return;

		//move the grabbed block
		CCPoint pos = ptouch->pt();
		_sp2bp(pos);

		CCPoint posC = pos;
		int r, c;
		int r0 = _ROW(pos.y);
		int c0 = _COL(pos.x);
		_Debug("pos.x=%.2f pos.y=%.2f r0=%d,c0=%d", pos.x, pos.y, r0, c0);

		float minx, maxx;
		float miny, maxy;
		//check left
		r = _ROW(pos.y + 0.0f);	c = _COL(pos.x - 1.0f);	if (_hasBeenOccupied(r, c, pbubble)) { minx = (float)c0; _Debug("minx inb=%s, bubble(%d,%d)=%s", _IS_IN_BOARD(r,c) ? "true" : "false", r, c, _getBubble(r, c) ? "true" : "false"); } else { minx = -10000; }
		//check right
		r = _ROW(pos.y + 0.0f);	c = _COL(pos.x + 1.0f);	if (_hasBeenOccupied(r, c, pbubble)) { maxx = (float)c0; _Debug("maxx inb=%s, bubble(%d,%d)=%s", _IS_IN_BOARD(r,c) ? "true" : "false", r, c, _getBubble(r, c) ? "true" : "false"); } else { maxx = 10000; }
		//check u
		r = _ROW(pos.y - 1.0f);	c = _COL(pos.x + 0.0f);	if (_hasBeenOccupied(r, c, pbubble)) { miny = (float)r0; _Debug("miny inb=%s, bubble(%d,%d)=%s", _IS_IN_BOARD(r,c) ? "true" : "false", r, c, _getBubble(r, c) ? "true" : "false"); } else { miny = -10000; }
		//check doen
		r = _ROW(pos.y + 1.0f);	c = _COL(pos.x + 0.0f);	if (_hasBeenOccupied(r, c, pbubble)) { maxy = (float)r0; _Debug("maxy inb=%s, bubble(%d,%d)=%s", _IS_IN_BOARD(r,c) ? "true" : "false", r, c, _getBubble(r, c) ? "true" : "false"); } else { maxy = 10000; }
		_Debug("minx=%.2f maxx=%.2f miny=%.2f maxy=%.2f", minx, maxx, miny, maxy);

		if (pos.x < minx) { _Debug("minx pos.x %.2f->%2f ", pos.x, minx); pos.x = minx; }
		if (pos.x > maxx) { _Debug("maxx pos.x %.2f->%2f ", pos.x, maxx); pos.x = maxx; }
		if (pos.y < miny) { _Debug("miny pos.y %.2f->%2f ", pos.y, miny); pos.y = miny; }
		if (pos.y > maxy) { _Debug("maxy pos.y %.2f->%2f ", pos.y, maxy); pos.y = maxy; }

		r = _ROW(pos.y);
		c = _COL(pos.x);
		_Debug("pos.x=%.2f pos.y=%.2f r=%d c=%d", pos.x, pos.y, r, c);
		if (!_hasBeenOccupied(r, c, pbubble))
		{
			pbubble->setDraggingPos(pos);
		}
	}
}

void BZGameClassic::_onTouchUngrabbed(CAEventTouch* ptouch)
{
	BZBubble* pbubble = _getGrabbedBubble(ptouch->fingler());
	if (pbubble)
	{
		BZBubble* pbubbleThisPos = _getBubbleByPoint(ptouch->pt());
		if (pbubble == pbubbleThisPos)
		{
			onBubbleClicked(pbubble);
		}
		EBubbleState s = pbubble->getState();
		if (BS_Dragging == s || BS_Drag == s)
		{
			pbubble->setState(BS_Fall);
		}
		_setGrabbedBubble(ptouch->fingler(), null);
	}
	else
	{
		pbubble = null;
	}
}
