
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

	_score_scale = 1.0f;
	_score_dx = 32.0f;

	_nLevel = 1;
	_nLevelState = 0;
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
	int levels, int bubble_score, int level_base_score, int level_mul_score, float score_dx, float score_scale,
	const BZLevelParams& levelmin, const BZLevelParams& levelmax)
{
	_levels = levels;
	_bubble_score = bubble_score;
	_level_base_score = level_base_score;
	_level_mul_score = level_mul_score;
	_paramsPreloaded[0] = levelmin;
	_paramsPreloaded[1] = levelmax;

	_score_dx = score_dx;
	_score_scale = score_scale;

	this->_onLevelChanged();
}


void BZGameClassic::_showScore(const CCPoint& pos, int score, float scale)
{
	char sz[16];
	sprintf(sz, "%d", score);
	int i, len = strlen(sz);
	float dx = _score_dx;
	scale = scale * _score_scale;
	CCPoint posCenter = pos;
	BZBoard::getBubbleRenderPos(posCenter);
	posCenter.x -= dx * len / 2;
	for (i = 0; i < len; i++)
	{
		BZSpriteCommon* pspr = new BZSpriteCommon(layer(), "number_3");
		pspr->setScl(scale);
		char szPose[16];
		szPose[0] = sz[i];
		szPose[1] = 0;
		pspr->pushState(szPose);
		pspr->setPos(posCenter);
		posCenter.x += dx * scale;
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
	int slots[64];
	int free;
	switch (_nLevelState)
	{
	case 0:
		if (_mapProcessed < (int)_mapLevel1.length())
		{
			//use the map
			int i, c = _mapLevel1.length();
			const char* psz = _mapLevel1.c_str();
			for (i = _mapProcessed; i <= c - 2; i += 2)
			{
				free = BZBoard::getEmptyBornSlots(slots, 64);
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
					type = _types[flag - 'a'];
				}

				const char* pszStar = null;
				char szStar[16]; 
				if (star)
				{
					pszStar = _getPropStarName(szStar);
				}

				BZBubble* pb = BZBoard::createBornBubble(type.c_str(), col, pszStar);
				//pb->setState(BS_Release);
			}
			_mapProcessed = i;
			BZBoard::fallOneRow();
		}
		else
		{
			_nLevelState++; //== 1
		}
		break;
	case 1:
		free = BZBoard::getEmptyBornSlots(slots, 64);
		if (free != BZBoard::getColumns())
		{
			BZBoard::fallOneRow();
		}
		else
		{
			_timeLastRow = this->getTimeNow();
			_nLevelState++; //== 2
		}
		break;
	default:
		//else, fall some bubbles to fill the board
		_handleBornStrategyLevelN();
		break;
	}
}


//
//OB:Open Block, there are some bubble in this block can be grabed.
//HOB: Hungry Open Block, bubbles count larger or equal to 3(3+OB)
//	HOBA, 0 star in HOB, score = 2.1
//	HOBB, 1 star in HOB, score = 1.1
//
//SOB:Star Bubble, a bubble with star and it can be grabed. even it is in HOB.
//	SOBA, SOB in HOB, score = 0.3
//	SOBB, SOB in !HOB, score = 0.8
//strategy:
//	if count of HOB < 3, build more HOB
//	blockSituations[HOB.colors], max of HOB.colors is 5
//	hob_score = blockSituations[color] 	
//	sob_scores[SOB.colors], max of SOB.colors is 5
//	sob_score = sob_scores[color]
//	delta_score = hob_score - sob_score;
//	if (delta_score < 0) do nothing
//	

#define MAX_WANTS	(3.0f)
class BlockSituation
{
public:
	bool hit;
	float hobs;
	float hobs_wants;
	float sobs;

	BlockSituation()
	{
		hit = false;
		hobs = 0;
		hobs_wants = 0;
		sobs = 0;
	}

	void addWants(float w)
	{
		hit = true;
		hobs++;
		hobs_wants += w;
	}
	void addSobs(float s)
	{
		hit = true;
		sobs += s;
	}

	//1: weak color, no hobs
	//2: satisfied or hungry
	bool isWeak() const
	{
		return hobs <= 0;
	}
	float getHungry() const
	{
		_Assert(!isWeak());

		float wants = hobs_wants / hobs;
		if (wants < sobs)
			return 0.05f; //we are satisfied
		float h = (wants - sobs) / MAX_WANTS;
		_Assert(h <= 1.0f);
		return h;
	}
};

#define HOBS_BUBBLES	3

//50%
//#define PERCENT_SOLVE_HOBS	(0.5)
//20%
#define PERCENT_SOLVE_WEAK		(0.2f)
#define PERCENT_SOLVE_WEAK_STAR	(0.4f)
//==> 40%(100% - 50% - 50% * 20%) chance to random 

bool BZGameClassic::_generateBubble(int& col, string& type, bool& star)
{
	GUARD_FUNCTION();

	_bubbleGenerated++;

	int allow_bubbles = min(BLOCK_TYPES, _params.nRangeBubbleBorn);

	BlockSituation blockSituations[BLOCK_TYPES];

	unsigned int n = 0;
	CAObject* pobj;
	_Info("blocks = %d", _blocksRunning->count());

	CCARRAY_FOREACH(_blocksRunning, pobj)
	{
		BZBlock* pb = (BZBlock*)pobj;
		int bubbles;
		int movables;
		int stars;
		bool hasSOB;

		pb->getStatus(bubbles, movables, stars, hasSOB);

		int color_index = _indexOfType(pb->getBubbleType().c_str());
		if (bubbles >= HOBS_BUBBLES) //this is a HOB, 3
		{
			//hungry for stars
			//bubbles		3			4			5			8+
			//no star		2 * 0.7		2 * 0.9		2 * 1.0		2 * (3 / 2)
			//01 star		1 * 0.7	 	1 * 0.9		1 * 1.0		1 * (3 / 2)
			float fac;
			if (bubbles >= HOBS_BUBBLES && bubbles <= HOBS_BUBBLES + 2)
			{
				static float facs[] = {0.7f, 0.9f, 1.0f, 1.0f };
				fac = facs[bubbles - HOBS_BUBBLES];
			}
			else 
			{
				float delta;
				delta = (float)(bubbles - (HOBS_BUBBLES + 2));
				fac = 1.0f + delta * (((float)MAX_WANTS / 2.0f - 1.0f) / (8.0f - (HOBS_BUBBLES + 2)));
			}
			
			if (movables <= 0)
			{
				fac *= 0.7f;
			}
			float base = 2.2f - stars;
			float wants = base * fac;
			if (wants > MAX_WANTS) wants = MAX_WANTS;

			blockSituations[color_index].addWants(wants);
		}
		else if (stars > 0) //assume it is an SOB or else
		{
			//bubbles		1				2			3?
			//				1.0f			0.8f(I don't want to leave)
			_Assert(bubbles >= 1 && bubbles < HOBS_BUBBLES);
			_Assert(HOBS_BUBBLES < 6);
			float give = 1.0f - (bubbles - 1) * 0.2f;
			if (movables <= 0)
			{
				give *= 0.2f; //it is berried
			}
			blockSituations[color_index].addSobs(give);
		}
	}

	//find satisfied colors count
	int i;
	int s = 0;
	int color = -1;
	bool gen = false;
	for (i = 0; i < allow_bubbles; i++)
	{
		_Info("blockSituations[%d]:type=%-18s,hobs=%.1f,wants=%.1f,sobs=%.1f,weak=%s", 
			i, _types[i].c_str(), 
			blockSituations[i].hobs, blockSituations[i].hobs_wants, blockSituations[i].sobs,
			blockSituations[i].isWeak() ? "true" : "false");
	}

	for (i = 0; !gen && i < allow_bubbles; i++)
	{
		//we have 10% chance to gen a weak color
		if (!blockSituations[i].isWeak())
		{
			float h = blockSituations[i].getHungry();
			if (CAUtils::Rand() < h)
			{
				gen = true;
				color = i;
				star = true;
			}
		}
	}

	for (i = 0; !gen && i < allow_bubbles; i++)
	{
		//we have 10% chance to gen a weak color
		if (blockSituations[i].isWeak())
		{
			if (CAUtils::Rand() < PERCENT_SOLVE_WEAK)
			{
				//gen a weak color
				gen = true;
				color = i;
				star = CAUtils::Rand() < PERCENT_SOLVE_WEAK_STAR;
			}
		}
	}
	if (!gen)
	{
		//random
		color = (int)CAUtils::Rand(0, (float)allow_bubbles);
		star = CAUtils::Rand() < 0.2f;
	}
	
	_Assert(color >= 0 && color < allow_bubbles);
	type += _types[color];

	return true;
}

//10 level, one column reach born line, program crashed
void BZGameClassic::_handleBornStrategyLevelN()
{
	GUARD_FUNCTION();

	_Assert(0 != (_nLevel % 10));

	ccTime timeNow = this->getTimeNow();
	int bubbles = BZBoard::getBubblesCount();
	
	int free = 0;
	int slots[64];
	free = BZBoard::getEmptyBornSlots(slots, 64);
	
	float fDelay;

	if (free <= 0)
	{
		if (BZBoard::isHeaderLineFull())
		{
			fDelay = _params.fDelayOneRow;
			if (_nPrebornLines > 0)
			{
				fDelay = fDelay / 120.0f; //speed up 120 times
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
		return;
	}

	bool gen = false;

	fDelay = _params.timeDelayBorn;
	if (timeNow - _timeLastBorn > fDelay && free > 0)
	{
		gen = true;
	}

	if (gen)
	{
		int rand = (int)CAUtils::Rand(0, (float)free);
		int col = slots[rand];

		string type = "";
		bool star = false;
		if (_generateBubble(col, type, star))
		{
			_timeLastBorn = timeNow;
			const char* pszStar = null;
			char szStar[16]; 
			if (star)
			{
				pszStar = _getPropStarName(szStar);
			}

			BZBubble* pb = BZBoard::createBornBubble(type.c_str(), col, pszStar);
		}
	}
}

void BZGameClassic::_handleBornStrategyLevelN10()
{
	GUARD_FUNCTION();

	bool bRainfall = true;

	ccTime timeNow = this->getTimeNow();
	int bubbles = BZBoard::getBubblesCount();
	
	int free = 0;
	int slots[64];
	free = BZBoard::getEmptyBornSlots(slots, 64);
	
	BZBoard::fallOneRow();
	
	float fDelay = _params.fDelayOneRow / 7.0f;
	if (timeNow - _timeLastBorn > fDelay && free > 0)
	{
		int rand = (int)CAUtils::Rand(0, (float)free);
		int col = slots[rand];

		string type = "";
		bool star = false;
		if (_generateBubble(col, type, star))
		{
			_timeLastBorn = timeNow;
			const char* pszStar = null;
			char szStar[16]; 
			if (star)
			{
				pszStar = _getPropStarName(szStar);
			}

			BZBubble* pb = BZBoard::createBornBubble(type.c_str(), col, pszStar);
		}
	}
}

void BZGameClassic::_doBornStrategy()
{
	GUARD_FUNCTION();

	if (1 == _nLevel)
	{
		_handleBornStrategyLevel1();
	}
	else if (0 == (_nLevel % 5))
	{
		_handleBornStrategyLevelN10();
	}
	else
	{
		_handleBornStrategyLevelN();
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
				{
#if defined(_TEST)
					_handleBornStrategyLevelN();
#endif
					CCSize size = CAWorld::sharedWorld().getScreenSize();
					if ((ptouch->pt().x > size.width * 0.8f) &&
						(ptouch->pt().y > size.height * 0.9f))
					{
						_doBornStrategy();
					}
					_onTouchGrabbed(ptouch);
				}
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

int BZGameClassic::_calculateScore(BZBlock* pblock) const
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
		_nLevelState = 0;
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
	params.timeDelayBorn	= _LERP_LEVEL_PARAM(timeDelayBorn);
	params.nAvailableStars	= (int)_LERP_LEVEL_PARAM(nAvailableStars);
	params.nRangeBubbleBorn	= (int)_LERP_LEVEL_PARAM(nRangeBubbleBorn);
	params.fPrebornLines	= _LERP_LEVEL_PARAM(fPrebornLines);

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

	int score = _calculateScore(pblock);
	_nScore += score;
	_onScoreChanged();

	//block do not know how to calculate the score in diff mode.
	CCPoint posCenter = pbCenter->getPos();
	_showScore(posCenter, score);

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
					_showScore(pbEffected[i]->getPos(), 120, 0.8f);
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
					_showScore(pbE->getPos(), 120, 0.8f);
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
					_showScore(pbE->getPos(), 120, 0.8f);
				}
			}
		}
		else if (prop == "prop_bomb_wheel")
		{
			pbCenter->addEffect("effect_wheel", "b1", false);
			_showScore(pbCenter->getPos(), 120, 0.8f);
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
