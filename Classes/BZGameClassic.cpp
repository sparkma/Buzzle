
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
}

BZGameClassic::~BZGameClassic()
{
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
			int free = _pboard->getEmptyBornSlots(slots, 64);
			//if (free < _pboard->getColumns())
			if (free <= 0)
				break;

			int col = psz[i]; 
			col -= '1';
			if (_pboard->getBubbleByGridPos(0, col))
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

			BZBubble* pb = _pboard->createBornBubble(type.c_str(), col, pszStar);
		}
		_mapProcessed = i;
		_pboard->fallOneRow();
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

	//select column first
	int typ = (int)CAUtils::Rand(0, (float)min(BLOCK_TYPES, _params.nRangeBubbleBorn));
	_Assert(typ >= 0 && typ < BLOCK_TYPES);
	type = "bubble_";
	type += _types[typ];
	star = CAUtils::Rand() * 100.0f < _params.fPercentStarBorn;
	if (!star)
	{
		int stars = _pboard->getStarsCount(type.c_str());
		if (stars < _params.nMinStarsInOneBubbleType)
		{
			star = true;
		}
	}
	return true;
}

void BZGameClassic::_handleBornStrategyLevelN()
{
	GUARD_FUNCTION();

	ccTime time = _pLayer->getTimeNow();
	int bubbles = _pboard->getBubblesCount();
	if (time - _timeLastBorn > _params.timeDelayBorn)
	{
		//how many free slots 
		int free = 0;
		int slots[64];
		free = _pboard->getEmptyBornSlots(slots, 64);
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

				BZBubble* pb = _pboard->createBornBubble(type.c_str(), col, pszStar);
				bool bRainfall = (0 == (_nLevel % 10));
				if (1 == _nLevel && _mapProcessed < (int)_mapLevel1.length())
				{
					_pboard->fallOneRow();
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
			if (!bRainfall)
			{
				_Assert(_pboard->isHeaderLineFull());
				ccTime timeNow = this->getTimeNow();
				if (timeNow - _timeLastRow > _params.fDelayOneRow)
				{
					_pboard->fallOneRow();
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

void BZGameClassic::onEvent(const CAEvent* pevt)
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
	params.timeDelayBorn	= _LERP_LEVEL_PARAM(timeDelayBorn);

	this->setLevelParams(params);
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

BZBubble* BZGameClassic::boomBlock(BZBlock* pblock)
{
	GUARD_FUNCTION();

	BZBubble* pbCenter = BZGame::boomBlock(pblock);
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
			pos.x = (float)_pboard->getColumns() / 2.0f;
			addGlobalEffect(pos, "effect_light", "b1");
			_Info("effect LIGHT Horz");
			for (i = 0; i < _pboard->getColumns(); i++)
			{
				BZBubble* pbE = _pboard->getBubble(pb->getIndexRow(), i);
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
			pos.y = (float)_pboard->getRows() / 2.0f;
			addGlobalEffect(pos, "effect_light", "b2");
			_Info("effect LIGHT VERT");
			for (i = 0; i < _pboard->getRows(); i++)
			{
				BZBubble* pbE = _pboard->getBubble(i, pb->getIndexColumn());
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
	int r = _pboard->getRows();
	int c = _pboard->getColumns();
	unsigned int bc = _pboard->getBubblesCount();
	if (r * c == bc)
	{
		//if all bubbles are standing?
		for (r = 0; r < _pboard->getRows(); r++)
		{
			for (c = 0; c < _pboard->getColumns(); c++)
			{
				BZBubble* pb = _pboard->getBubble(r, c);
				_Assert(null != pb);
				if (pb->getState() != BS_Standing)
					return false;
			}
		}
		return true;
	}
	return false;
}

