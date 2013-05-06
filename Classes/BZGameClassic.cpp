
#include "BZGameClassic.h"
#include "AStageLayer.h"
#include "AWorld.h"


#define BUBBLE_DOODAD_1 "bubble_doodad_1";
#define BUBBLE_DOODAD_1_POSE_FMT	"b%d"
#define BUBBLE_DOODAD_1_POSE_FROM	1
#define BUBBLE_DOODAD_1_POSE_COUNT	4

#define SPRITE_BUBBLE_SCORE "number_3"

#define BUBBLE_PROP_CONNECTOR	"prop_connector"
#define BUBBLE_PROP_BOOOM		"prop_booom"
#define BUBBLE_PROP_SAMECOLOR	"prop_samecolor"
#define BUBBLE_PROP_CHANGECOLOR "prop_changecolor"

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

	memset(_scores, 0, sizeof(_scores));
	memset(_dropline_interval, 0, sizeof(_dropline_interval));
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
	int levels, int bubble_score, 
	int level_base_score, int level_mul_score, 
	float level_base_drop, float level_mul_drop,
	float score_dx, float score_scale,
	int curlevel,
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

	_scores[0] = 0;
	int level;
	for (level = 1; level < SIZE_OF_ARRAY(_scores); level++)
	{
		float u = log((float)(level * 2.732));
		_scores[level] = (int)(_scores[level - 1] + u * _level_mul_score + _level_base_score);
	}

	float base = level_base_drop; //1.8f;
	float mul = level_mul_drop; //0.9f;
	float m = log(128 * 2.732f);
	_dropline_interval[0] = 0;
	for (level = 1; level < SIZE_OF_ARRAY(_dropline_interval); level++)
	{
		float u = (float)log((float)level * 2.732);
		u = m - u;
		float s = (u * mul + base);
		_dropline_interval[level] = s;
	}

	_Assert(curlevel >= 1);
	_nLevel = curlevel;
	_nScore = _scores[_nLevel - 1];

	this->_onLevelChanged();
	this->_onScoreChanged();
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
		BZSpriteCommon* pspr = new BZSpriteCommon(layer(), SPRITE_BUBBLE_SCORE);
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
				if (star)
				{
					pszStar = BUBBLE_PROP_CONNECTOR;
				} 

				BZBubble* pb = BZBoard::createBornBubble(type.c_str(), col, pszStar);
				pb = null;
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
	//_Info("blocks = %d", _blocksRunning->count());

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

#if 0
	for (i = 0; i < allow_bubbles; i++)
	{
		_Info("blockSituations[%d]:type=%-18s,hobs=%.1f,wants=%.1f,sobs=%.1f,weak=%s", 
			i, _types[i].c_str(), 
			blockSituations[i].hobs, blockSituations[i].hobs_wants, blockSituations[i].sobs,
			blockSituations[i].isWeak() ? "true" : "false");
	}
#endif

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
			if (star)
			{
				pszStar = BUBBLE_PROP_CONNECTOR;
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
			if (star)
			{
				pszStar = BUBBLE_PROP_CONNECTOR;
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

void BZGameClassic::onUpdate()
{
	BZGame::onUpdate();
	switch (_state)
	{
	case GS_LevelUp:
		{		
			int r, c;
			for (r = 0; r < BZBoard::getRows(); r++)
			{
				for (c = 0; c < BZBoard::getColumns(); c++)
				{
					BZBubble* pb = BZBoard::_getBubble(r, c);
					if (null != pb)
					{
						pb->setPose("xxxx");
						pb->lock(true);
					}
				}
			}
			_state = GS_LevelUping;
		}
		break;
	case GS_LevelUping:
		//remove all bubbles
		{
			int n = _nCounter % 10;
			if (1 != n)
			{
				break;
			}
			int booomed = 0;
			bool finished = true;
			int r, c;
			for (r = 0; booomed < 2 && r < BZBoard::getRows(); r++)
			{
				for (c = 0; booomed < 2 && c < BZBoard::getColumns(); c++)
				{
					BZBubble* pb = BZBoard::_getBubble(r, c);
					if (null != pb && pb->getState() < BS_Die)
					{
						_nScore += 100;
						_onScoreChanged();
						pb->setState(BS_Die);
						pb->addEffect("effect_booom_back", "stand", true);
						booomed++;
						finished = false;
					}
				}
			}
			if (finished)
			{
				_state = GS_LevelUpEffecting;
			}
		}
		break;
	case GS_LevelUpEffecting:
		if (BZBoard::getBubblesCount() <= 0)
		{
			//level uped!
			this->dispatchEvent(new CAEventCommand(this, ST_UserDefined, "leveluped"));
			_state = GS_Running;
		}
		break;
	case GS_Running:
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
				{
#if defined(_TEST)
					_handleBornStrategyLevelN();
#endif
#if 0
					CCSize size = CAWorld::sharedWorld().getScreenSize();
					if ((ptouch->pt().x > size.width * 0.8f) &&
						(ptouch->pt().y > size.height * 0.9f))
					{
						_doBornStrategy();
					}
#endif
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

int BZGameClassic::_getLevelScore(int level) const
{
	if (level > SIZE_OF_ARRAY(_scores) - 1)
		level = SIZE_OF_ARRAY(_scores) - 1;
	_Assert(level >= 1);
	return _scores[level];
}

void BZGameClassic::_onScoreChanged()
{
	//score 2 level
	this->dispatchEvent(new CAEventCommand(this, ST_UserDefined, "scoreup"));

	int levelScore = _getLevelScore(_nLevel);
	if (_nScore > levelScore)
	{
		_nLevel++;
		_nLevelState = 0;

		CCPoint pos = ccp(0.5, 0.7);
		CAWorld::percent2view(pos);
		
		_onLevelChanged();
	}
}

float BZGameClassic::getLevelPercent() const
{
	int levelScore0;
	int levelScore1;

	if (_nLevel > 1)
	{
		levelScore0 = _getLevelScore(_nLevel - 1);
	}
	else
	{
		levelScore0 = 0;
	}
	levelScore1 = _getLevelScore(_nLevel);

	float p = (float)(_nScore - levelScore0) / (float)(levelScore1 - levelScore0);
	if (p > 1.0f) p = 1.0f;
	else if (p < 0.0f) p = 1.0f;

	return p;
}

#define _LERP_LEVEL_PARAM(param) \
	((float)_paramsPreloaded[0].param + ((float)_paramsPreloaded[1].param - (float)_paramsPreloaded[0].param) * ((_nLevel > _levels) ? (_levels) : (_nLevel -1)) / (float)_levels)

void BZGameClassic::_onLevelChanged()
{
	GUARD_FUNCTION();

	BZGame::_onLevelChanged();
	BZLevelParams params;
	
	params.fDelayOneRow = _dropline_interval[_nLevel]; // _LERP_LEVEL_PARAM(fDelayOneRow);
	params.timeDelayBorn	= _LERP_LEVEL_PARAM(timeDelayBorn);
	params.nAvailableStars	= (int)_LERP_LEVEL_PARAM(nAvailableStars);
	params.nRangeBubbleBorn	= (int)_LERP_LEVEL_PARAM(nRangeBubbleBorn);
	params.fPrebornLines	= _LERP_LEVEL_PARAM(fPrebornLines);

	this->setLevelParams(params);

	if (_nLevel > 1)
	{
		this->dispatchEvent(new CAEventCommand(this, ST_UserDefined, "levelup", &_nLevel));
		this->_state = GS_LevelUp;

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

	//block do not know how to calculate the score in diff mode.
	CCPoint posCenter = pbCenter->getPos();
	_showScore(posCenter, score);

	int rand;

	//CAStringMap<CAInteger> props;
	int nScorePlus = 0;

	pbCenter->addEffect("effect_booom_back", "stand", true);

	CCArray* _bubbles = pblock->getBubbles();
	CAObject* pobj;
	CCARRAY_FOREACH(_bubbles, pobj)
	{
		BZBubble* pb = (BZBubble*)pobj;

		//collect prop here
		const string& prop = pb->getPropType();

		if (prop == BUBBLE_PROP_BOOOM)
		{
			_Info("effect BOOOM");

			/*
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
					nScorePlus += 120;
				}
			}
			*/
		}
		else if (prop == BUBBLE_PROP_CHANGECOLOR)
		{
			CCPoint pos = pb->getPos();
			//6 ==> 2.5, 7 ==> 3
			//(n - 1) / 2
			pos.x = ((float)BZBoard::getColumns() - 1.0f) / 2.0f;
			BZBoard::getBubbleRenderPos(pos);
			/*
			addGlobalEffect(pos, "effect_light", "b1");
			_Info("effect LIGHT Horz");
			for (i = 0; i < BZBoard::getColumns(); i++)
			{
				BZBubble* pbE = BZBoard::_getBubble(pb->getIndexRow(), i);
				if (null != pbE && pbE->getBlock() != pb->getBlock())
				{
					_addFireEffectOn(pbE);
					pbE->setState(BS_Die);
					nScorePlus += 120;
				}
			}
			*/
		}
		else if (prop == BUBBLE_PROP_SAMECOLOR)
		{
			CCPoint pos = pb->getPos();
			//6 ==> 2.5, 7 ==> 3
			//(n - 1) / 2
			pos.y = ((float)BZBoard::getRows() - 1.0f) / 2.0f;
			BZBoard::getBubbleRenderPos(pos);
			/*
			addGlobalEffect(pos, "effect_light", "b2");
			_Info("effect LIGHT VERT");
			for (i = 0; i < BZBoard::getRows(); i++)
			{
				BZBubble* pbE = BZBoard::_getBubble(i, pb->getIndexColumn());
				if (null != pbE && pbE->getBlock() != pb->getBlock())
				{
					_addFireEffectOn(pbE);
					pbE->setState(BS_Die);
					nScorePlus += 120;
				}
			}
			*/
		}
		else if (prop == BUBBLE_PROP_CONNECTOR) {}
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
	case 6:
	case 7:
	case 8:
	case 9:
	default:
		rand = (int)CAUtils::Rand(0, 6);
		_Assert(rand >= 0 && rand < 6);
		if (rand < 3) prop = BUBBLE_PROP_CHANGECOLOR;
		else if (rand < 5) prop = BUBBLE_PROP_SAMECOLOR;
		else  prop = BUBBLE_PROP_BOOOM;
		break;
	}
	if (null != prop)
	{
		pbCenter->setRebornBubble(pblock->getBubbleType().c_str(), prop);
	}

	posCenter = pbCenter->getPos();
	posCenter.y += 1.0f;
	_nScore += nScorePlus;
	_onScoreChanged();

	if (nScorePlus > 0)
	{
		_showScore(posCenter, nScorePlus, 0.8f);
	}

	return pbCenter;
}

void BZGameClassic::onBlockStarsChanged(BZBlock* pblock, int stars)
{
	const char* doodad = null;
	if (1 != stars)
	{
		return;
	}
	doodad = BUBBLE_DOODAD_1;
	
	int rand;
	rand = (int)CAUtils::Rand(0, (float)4.0f);
	char szPose[16];
	_Assert(rand >= 0 && rand < BUBBLE_DOODAD_1_POSE_COUNT);
	sprintf(szPose, BUBBLE_DOODAD_1_POSE_FMT, rand + BUBBLE_DOODAD_1_POSE_FROM);
	pblock->resetBubblesDoodad(doodad, szPose);
}

bool BZGameClassic::isGameOver() const
{
	int rows = BZBoard::getRows();
	int cols = BZBoard::getColumns();
	unsigned int bc = BZBoard::getBubblesCount();
	if (rows * cols == bc)
	{
		if (!BZBoard::isHeaderLineFull())
			return false;
		//if all blocks can not boooom
		CAObject* pobj;
		CCARRAY_FOREACH(_blocksRunning, pobj)
		{
			BZBlock* pb = (BZBlock*)pobj;
			if (_canBoom(pb))
				return false;

			//there are header line block here
			//if (!pb->isAllStanding())
			//	return false;
			
			if (pb->getState() != Block_Running)
				return false;
		}

		//if all bubbles in board are standing?
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

		pbubble->setDraggingPos(pos, ptouch->time());
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
			pbubble->setState(BS_DragRelease);
		}
		_setGrabbedBubble(ptouch->fingler(), null);
	}
	else
	{
		pbubble = null;
	}
}
