
#include "BZGameClassic.h"
#include "AStageLayer.h"
#include "AWorld.h"


BZGameClassic::BZGameClassic(CAStageLayer* player)
: BZGame(player)
{
	_name = "classic";
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

void BZGameClassic::_doBornStrategy()
{
	ccTime time = _pLayer->getTimeNow();
	if (time - _timeLastBorn > _params.timeDelayBorn && _pboard->getBubblesCount() < 7 * 9)
	{
		_timeLastBorn = time;

		bool star = CAUtils::Rand() * 100.0f < _params.fPercentStarBorn;
		if (!star)
		{
			int bubblecount, blockcount, stars, props;
			_pboard->getCounts(bubblecount, blockcount, stars, props);
			if ((float)stars / (float)bubblecount < _params.fMinPercentStar / 100.0f)
			{
				if (CAUtils::Rand() * 100.0f < 90.0f)
				{
					star = true;
				}
			}
		}
		//select column first
		int typ = (int)CAUtils::Rand(0, (float)_params.nRangeBubbleBorn);
		_Assert(typ >= 0 && typ < BLOCK_TYPES);
		string type = "bubble_";
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
		}
	}
}

void BZGameClassic::onEvent(CAEvent* pevt)
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
	
	params.fMinPercentStar	= _LERP_LEVEL_PARAM(fMinPercentStar);
	params.fPercentStarBorn	= _LERP_LEVEL_PARAM(fPercentStarBorn);
	params.nRangeBubbleBorn	= (int)_LERP_LEVEL_PARAM(nRangeBubbleBorn);
	params.timeDelayBorn	= _LERP_LEVEL_PARAM(timeDelayBorn);

	this->setLevelParams(params);
}
