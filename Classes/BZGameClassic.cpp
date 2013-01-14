
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
	if (pblock->getStars() >= 2 && pblock->isAllStanding())
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
	if (_nLevel == 1 && _mapProcessed < (int)_mapLevel1.length())
	{
		//use the map
		int i, c = _mapLevel1.length();
		const char* psz = _mapLevel1.c_str();
		for (i = _mapProcessed; i <= c - 2; i += 2)
		{
			int col = psz[i]; 
			col -= '1';
			if (_pboard->getBubbleByGridPos(0, col))
			{
				break;
			}
			_Info("process level1 map: [%02d] %c%c", i, psz[i], psz[i + 1]);

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

			BZBubble* pb = _pboard->createBubble(type.c_str(), ccp(col, 0), pszStar);
		}
		_mapProcessed = i;
		if (!(_mapProcessed < (int)_mapLevel1.length()))
		{
			ccTime timeNow = this->getTimeNow();
			_timeLastRow = timeNow;
		}
		return;
	}
	else
	{
		//else, fall some bubbles to fill the board
		_handleBornStrategyLevelN(3);
	}
}

void BZGameClassic::_handleBornStrategyLevelN(int rows)
{
	rows = 11;
	ccTime time = _pLayer->getTimeNow();
	int bubbles = _pboard->getBubblesCount();
	if (time - _timeLastBorn > _params.timeDelayBorn && bubbles < 7 * rows)
	{
		_timeLastBorn = time;

		//select column first
		int typ = (int)CAUtils::Rand(0, (float)min(BLOCK_TYPES, _params.nRangeBubbleBorn));
		_Assert(typ >= 0 && typ < BLOCK_TYPES);
		string type = "bubble_";
		type += _types[typ];

		bool star = CAUtils::Rand() * 100.0f < _params.fPercentStarBorn;
		if (!star)
		{
			int stars = _pboard->getStarsCount(type.c_str());
			if (stars < _params.nMinStarsInOneBubbleType)
			{
				star = true;
			}
		}

		//how many free slots 
		int free = 0;
		int slots[64];
		free = _pboard->getEmptyBornSlots(slots, 64);

		//create a block+group+props if there is a free slot
		if (free > 0)
		{
			//_bIsHeaderlineFull = false;

			//rand a slot
			int rand = (int)CAUtils::Rand(0, (float)free);
			int slot = slots[rand];
			const char* pszStar = null;
			char szStar[16]; 
			if (star)
			{
				pszStar = _getPropStarName(szStar);
			}
			else
			{
				//sprintf(szStar, "prop_bomb_ooox");
				//pszStar = szStar;
			}
			BZBubble* pb = _pboard->createBubble(type.c_str(), ccp(slot, 0), pszStar);
			bool bRainfall = (0 == (_nLevel % 10));
			pb->setRainfallMode(bRainfall);
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
	switch (_nLevel)
	{
	case 1:
		_handleBornStrategyLevel1();
		break;
	default:
		{
			int rows = _nLevel + 2;
			if (rows > _pboard->getRows()) rows = _pboard->getRows();
			_handleBornStrategyLevelN(rows);
		}
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
	BZGame::_onLevelChanged();
	BZLevelParams params;
	
	params.fDelayOneRow = _LERP_LEVEL_PARAM(fDelayOneRow);
	params.nMinStarsInOneBubbleType	= (int)_LERP_LEVEL_PARAM(nMinStarsInOneBubbleType);
	params.fPercentStarBorn	= _LERP_LEVEL_PARAM(fPercentStarBorn);
	params.nRangeBubbleBorn	= (int)_LERP_LEVEL_PARAM(nRangeBubbleBorn);
	params.timeDelayBorn	= _LERP_LEVEL_PARAM(timeDelayBorn);

	this->setLevelParams(params);
}

BZBubble* BZGameClassic::boomBlock(BZBlock* pblock)
{
	BZBubble* pbCenter = BZGame::boomBlock(pblock);
	if (null == pbCenter)
	{
		return null;
	}

	int i;

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
			pbCenter->addEffect("effect_fire", "b1", false);
		}
		else if (prop == "prop_bomb_oxox")
		{
			CCPoint pos = pb->getPos();
			pos.x = (float)_pboard->getColumns() / 2.0f;
			pos.y += 0.5f;
			//_pboard->getBubbleRenderPos(pos);
			_addGlobalEffect(pos, "effect_light", "b1");
		}
		else if (prop == "prop_bomb_xoxo")
		{
			CCPoint pos = pb->getPos();
			pos.x += 0.5f;
			pos.y = (float)_pboard->getRows() / 2.0f;
			//_pboard->getBubbleRenderPos(pos);
			_addGlobalEffect(pos, "effect_light", "b2");
		}
		else if (prop == "prop_bomb_wheel")
		{
			pbCenter->addEffect("effect_wheel", "b1", false);
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
	}

	//prcocess props and common-effects
	CCARRAY_FOREACH(_bubbles, pobj)
	{
		BZBubble* pb = (BZBubble*)pobj;
		for (i = 0; i < 2; i++)
		{
			int rand;
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

	int bc = _bubbles->count();
	const char* prop = null;
	switch (bc)
	{
	case 2:
	case 3:
	case 4:
		break;
	case 5:
	case 6:
		prop = "prop_bomb_oooo";
		break;
	case 7:
	case 8:
		prop = "prop_bomb_oxox";
		break;
	case 9:
	case 10:
		prop = "prop_bomb_xoxo";
		break;
	case 11:
	case 12:
	case 13:
	default:
		prop = "prop_bomb_wheel";
		break;
	}
	if (null != prop)
	{
		pbCenter->setRebornBubble(pblock->getBubbleType().c_str(), prop);
	}
	return pbCenter;
}
