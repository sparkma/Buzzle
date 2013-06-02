
#include "BZGameClassic.h"
#include "BZSpriteButtonItem.h"
#include "BZSpriteStarHolder.h"

#include "AStageLayer.h"
#include "AWorld.h"
#include "AString.h"

#define BUBBLE_DOODAD_1 "bubble_doodad_1";
#define BUBBLE_DOODAD_1_POSE_FMT	"b%d"
#define BUBBLE_DOODAD_1_POSE_FROM	1
#define BUBBLE_DOODAD_1_POSE_COUNT	4

#define SPRITE_BUBBLE_SCORE "number_4"

void BZGameClassicPropManager::addPropPiece(const string& name, const CCPoint& pos)
{
	postGameEvent("prop_piece", (_difficulty + "-" + name).c_str());
	if (name == BUBBLE_PROP_BOOOM && _pbtnBoom)
	{
		_pbtnBoom->addPiece(name, pos);
	}
	if (name == BUBBLE_PROP_SAMECOLOR && _pbtnSameColor)
	{
		_pbtnSameColor->addPiece(name, pos);
	}
	if (name == BUBBLE_PROP_CHANGECOLOR && _pbtnChangeColor)
	{
		_pbtnChangeColor->addPiece(name, pos);
	}
}

void BZGameClassicPropManager::onLevelChanged(int level)
{
	_level = level;
	if (_pbtnBoom)
		_pbtnBoom->onLevelChanged(level);
	if (_pbtnSameColor)
		_pbtnSameColor->onLevelChanged(level);
	if (_pbtnChangeColor)
		_pbtnChangeColor->onLevelChanged(level);
}

void BZGameClassicPropManager::setFillRate(float r)
{
	if (_pbtnBoom)
		_pbtnBoom->setFillRate(r);
	if (_pbtnSameColor)
		_pbtnSameColor->setFillRate(r);
	if (_pbtnChangeColor)
		_pbtnChangeColor->setFillRate(r);
}

void BZGameClassicPropManager::setGameState(EGameState gs)
{
	if (_pbtnBoom)
		_pbtnBoom->setGameState(gs);
	if (_pbtnSameColor)
		_pbtnSameColor->setGameState(gs);
	if (_pbtnChangeColor)
		_pbtnChangeColor->setGameState(gs);
}

void BZGameClassicPropManager::initializeProp(const string& difficulty, 
		BZSpriteButtonItem* pbtnBoom, 
		BZSpriteButtonItem* pbtnSameColor, 
		BZSpriteButtonItem* pbtnChangeColor,
		int limitBoom, int limitSameColor, int limitChangeColor)
{
	_difficulty = difficulty;
	_pbtnBoom = (BZSpriteButtonItem*)pbtnBoom;
	_pbtnSameColor = (BZSpriteButtonItem*)pbtnSameColor;
	_pbtnChangeColor = (BZSpriteButtonItem*)pbtnChangeColor;

	_pbtnBoom->setDifficulty(difficulty);
	_pbtnSameColor->setDifficulty(difficulty);
	_pbtnChangeColor->setDifficulty(difficulty);

	_pbtnBoom->setLimitLevel(limitBoom);
	_pbtnSameColor->setLimitLevel(limitSameColor);
	_pbtnChangeColor->setLimitLevel(limitChangeColor);
}

BZGameClassic::BZGameClassic(CAStageLayer* player)
: BZGame(player)
{
	_name = "classic";

	_mapProcessed = 0;
	//_bIsHeaderlineFull = false;
	_timeLastRow = 0;

	_score_scale = 1.0f;
	_score_dx = 20.0f;

	_nLevel = 1;
	_nLevelState = 0;
	_nCombo = 0;
	_combo_rate = 0;
	_nPrebornLines = 0;

	_bubbleGenerated = 0;
	_lastStarIndex = 0;

	_limitBoom = 0;
	_limitSameColor = 0;
	_limitChangeColor = 0;

	memset(_bubblesGrabbed, 0, sizeof(_bubblesGrabbed));

	memset(_scores, 0, sizeof(_scores));
	memset(_dropline_interval, 0, sizeof(_dropline_interval));

	_stars = 0;
	memset(_psprStarHolders, 0, sizeof(_psprStarHolders));

	//_VerifyClass(this);
}

BZGameClassic::~BZGameClassic()
{
	_freeAllResources();
}

void BZGameClassic::_freeAllResources()
{
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

string BZGameClassic::debuglog()
{
	return _debug_log;
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
	float combo_rate,
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
	_combo_rate = combo_rate;

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

	_stars = 0;
	_psprStarHolders[0]->reset();
	_psprStarHolders[1]->reset();
	_psprStarHolders[2]->reset();

	this->_onLevelChanged(true);
	this->_onScoreChanged();
}


void BZGameClassic::_showScore(const CCPoint& pos, int score, float scale, bool plus)
{
	char sz[16];
	sprintf(sz, "%s%d", plus ? "+" : "", score);
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
		pspr->setVertexZ(BZBoard::getBaseZOrder() + VZ_DYN_SCORE);
		strcpy(szPose, "dead");
		pspr->pushState(szPose);
		//pspr->setDeadPose(szPose);
		layer()->addSprite(pspr);
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
				int col = psz[i]; 
				col -= '1';

				if (null != _getBornBubble(col))
				{
					//wait
					break;
				}

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
		//if some bubble is falling a bit, and blocked by some dragging block.
		//free <= 0, and BZBoard::isHeaderLineFull is FALSE!
		//we will stuck here!!!!
		if (BZBoard::isHeaderLineCanFall()) //all !null bubbles' state is bigger or equal to BS_Borned
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
		//else
		{
			//all headline is stucked. some bubbles borned, some bubble is falling
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

	_propManager.setFillRate(getFillRate());

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

//sectype may be ""
void BZGameClassic::_findBigestBlockType(string& btype, string& sectype)
{
	string types[16];
	int counts[16];
	int count = 0;

	int i, j;
	CAObject* pobj;
	//find the bigest block, and boooom them
	CCARRAY_FOREACH(_blocksRunning, pobj)
	{
		BZBlock* pb = (BZBlock*)pobj;
		string type = pb->getBubbleType();
		int c = pb->getBubbles()->count();;

		bool find = false;

		for (i = 0; i < count; i++)
		{
			if (types[i] == type)
			{
				counts[i] += c;
				find = true;
				break;
			}
		}
		if (!find)
		{
			types[count] = type;
			counts[count] = c;
			count++;
		}
	}

	for (i = 0; i < (count < 2 ? count : 2); i++)
	{
		for (j = i + 1; j < count; j++)
		{
			if (counts[i] < counts[j])
			{
				int k = counts[j];
				counts[j] = counts[i];
				counts[i] = k;
				string s = types[j];
				types[j] = types[i];
				types[i] = s;
			}
		}
	}

	if (count > 0)
		btype = types[0];
	else
		btype = "";

	if (count > 1)
		sectype = types[1];
	else
		sectype = "";

	return;
}

void BZGameClassic::_doBubbleDied(BZBubble* pbubble)
{
	GUARD_FUNCTION();

	if (pbubble->isLocked())
	{
		float scale = 0.6f;
		int ds = 100;
		if (pbubble->getPropType().length() > 0)
		{
			scale *= 1.2f;
			ds += 80;
		}
		_nScore += ds;
		_showScore(pbubble->getPos(), ds, scale);
		_onScoreChanged();
	}
	else
	{
		const string& pt = pbubble->getPropType();
		if (pt.length() > 0)
		{
			if (pt == BUBBLE_PROP_BOOOM || pt == BUBBLE_PROP_SAMECOLOR || pt == BUBBLE_PROP_CHANGECOLOR)
			{
				//this prop will die.
				CASprite* psprProp = pbubble->getSpriteProp();
				CCPoint pos = pbubble->getPos();
				BZBoard::getBubbleRenderPos(pos);
				_propManager.addPropPiece(pt, pos);
			}
		}
	}
	BZGame::_doBubbleDied(pbubble);
}

void BZGameClassic::_lockAndKill(BZBubble* pbubble, float delay)
{
	if (null == pbubble->getBlock())
		return;

	pbubble->lock(true);

	//pbubble->getBlock()->detachBubble(pbubble);
	pbubble->setState(BS_Die);

	pbubble->setDyingDelay(delay);
}

void BZGameClassic::onResume()
{
	if (_gamestate == GS_LevelUpPaused)
	{
		setState(GS_LevelUpResume);
	}
	else if (_gamestate == GS_GamePaused)
	{
		setState(GS_Running);
	}
	else
	{
		BZGame::onResume();
	}
}

void BZGameClassic::setState(EGameState to)
{
	_Info("game state is changing to from %d to %d", _gamestate, to);
	bool accept = false;
	switch (_gamestate)
	{
	case GS_Idle:
		if (to == GS_Running || to == GS_Leaving)
			accept = true;
		break;
	case GS_Running:
		accept = true;
		break;

	case GS_LevelUpPrepare:
	case GS_LevelUpWaiting:
	case GS_LevelUpPaused:
	case GS_LevelUpResume:
	case GS_LevelUped:
		if ((to >= GS_LevelUpPrepare && to <= GS_LevelUped) || to == GS_SpecEffecting)
			accept = true;
		break;
	case GS_ItemChangeColor:
	case GS_ItemSameColorBooom:
	case GS_ItemBooom:
		if (to == GS_SpecEffecting)
			accept = true;
		break;
	case GS_SpecEffecting:
		if (to == GS_Running || to == GS_LevelUpPrepare)
			accept = true;
		break;
	case GS_Leaving:
		if (to == GS_Idle)
			accept = true;
		break;
	case GS_GamePaused:
		if (to == GS_Running)
			accept = true;
		break;
	}
	if (accept)
	{
		_Info("game state is changed to %d", to);
		_propManager.setGameState(to);
		BZGame::setState(to);
	}
	else
	{
		int i;
		_Info("game state is NOT ACCEPTED: %d", to);
		for (i = 0; i < _nGameStatesCount; i++)
		{
			_Info("    old game state:%d", _oldGameStates[i]);
		}
	}
}

void BZGameClassic::onUpdate()
{
	BZGame::onUpdate();

	float delay = 0;
	switch (_gamestate)
	{
	case GS_ItemSameColorBooom:
		{
			_debug_log = "GS_ItemSameColorBooom";
			string btype, sectype;
			_findBigestBlockType(btype, sectype);

			CAObject* pobj;
			//now mcount and btype are ready
			CCARRAY_FOREACH(_blocksRunning, pobj)
			{
				BZBlock* pb = (BZBlock*)pobj;
				string type = pb->getBubbleType();
				if (type == btype)
				{
					CCArray* bubbles = pb->getBubbles();
					//bubbles->retain();
					CAObject* pbubbleobj;
					CCARRAY_FOREACH(bubbles, pbubbleobj)
					{
						BZBubble* pbubble = (BZBubble*)pbubbleobj;

						if (_isInBoard(pbubble))
						{
							delay += CAUtils::Rand(0.02f, 0.025f);
							_lockAndKill(pbubble, delay);
						}
					}
					//bubbles->release();
				}
			}
			setState(GS_SpecEffecting);
		}
		break;
	case GS_ItemChangeColor:
		{
			_debug_log = "GS_ItemChangeColor";
			string btype, sectype;
			_findBigestBlockType(btype, sectype);
			if (sectype.length() <= 0)
			{
				//only one color
				setState(GS_Running);
				break;
			}

			CAObject* pobj;
			//now mcount and btype are ready
			CCARRAY_FOREACH(_blocksRunning, pobj)
			{
				BZBlock* pb = (BZBlock*)pobj;
				string type = pb->getBubbleType();
				if (type == btype)
				{
					CCArray* bubbles = pb->getBubbles();
					//bubbles->retain();
					CAObject* pbubbleobj;
					CCARRAY_FOREACH(bubbles, pbubbleobj)
					{
						BZBubble* pbubble = (BZBubble*)pbubbleobj;
						if (_isInBoard(pbubble))
						{
							if (null == pbubble->getBlock())
								continue;

							pbubble->lock(true);
							pbubble->setRebornBubble(sectype.c_str(), null);
							pbubble->addEffect(BUBBLE_EFFECT_CHANGECOLOR, "born", true, CAUtils::Rand(0.02f, 0.06f));
							pbubble->setState(BS_Dying);
						}
					}
					//bubbles->release();
				}
			}
			setState(GS_SpecEffecting);
		}
		break;
	case GS_ItemBooom:
		{
			_debug_log = "GS_ItemBooom";
			//circle booom, center is 
			float cx = (float)(BZBoard::getColumns() - 1) / 2.0f;
			float cy = (float)BZBoard::getRows() - 1 - cx;

			int r, c;
			for (r = 0; r < BZBoard::getRows(); r++)
			{
				for (c = 0; c < BZBoard::getColumns(); c++)
				{
					//max of d2 = 3 * 3 + 3 * 3 = 18;
					float d2 = (r - cy) * (r - cy) + (c - cx) * (c - cx);
					
					if (d2 <= (cx + 0.51f) * (cx + 0.51f))
					{
						BZBubble* pbubble = BZBoard::_getBubble(r, c);
						if (null != pbubble)
						{
							//delay += CAUtils::Rand(0.01f, 0.015f);
							delay = d2 * CAUtils::Rand(0.02f, 0.025f);
							_lockAndKill(pbubble, delay);
						}
					}
				}
			}
			setState(GS_SpecEffecting);
		}
		break;

	//==> GS_LevelUpPrepare
	//pause BornStrategy, (if not in GS_Running, BornStrategy will paused )
	//remove header line
	//==> GS_LevelUpWaiting
	//wait for all bubble stand
	//==> GS_LevelUpLock 
	//lock all bubbles
	//call level up dialog 
	//==> GS_LevelUpPaused
	//onResume ==> GS_LevelUpResume;
	//booom all locked-bubbles
	//when it finished ==> GS_LevelUped;
	//resume BornStrategy
	//==> GS_Running
	case GS_LevelUpPrepare:
		{
			_debug_log = "GS_LevelUpPrepare";
			_Info("GS_LevelUpPrepare");

#if 0
			crash CODES
			BZBubble* pbubbles[24];
			int left = BZBoard::getBornBubbles(pbubbles, 24);
			int c;
			for (c = 0; c < BZBoard::getColumns(); c++)
			{
				BZBubble* pbubble = pbubbles[c];
				if (null != pbubble)
				{
					pbubble->lock(true);
					pbubble->setState(BS_DieNow);
				}
			}
			_Info("GS_LevelUpPrepare -> GS_LevelUpWaiting");
			setState(GS_LevelUpWaiting);
#else
			//ensure headline can drop down
			BZBoard::fallOneRow();

			int c;
			for (c = 0; c < BZBoard::getColumns(); c++)
			{
				BZBubble* pbubble = BZBoard::_getBubble(0, c);
				if (null != pbubble)
				{
					_lockAndKill(pbubble, 0);
				}
			}
			_Info("GS_LevelUpPrepare -> GS_LevelUpWaiting");
			setState(GS_LevelUpWaiting);
#endif
		}
		break;
	case GS_LevelUpWaiting:
		{
			_debug_log = "GS_LevelUp P/W";
			_Info("GS_LevelUpWaiting");

			BZBoard::fallOneRow();
			BZBubble* pbubbles[24];
			int left = BZBoard::getBornBubbles(pbubbles, 24);

			if (left <= 0)
			{
				bool stable = true;
				int r, c;
				for (r = 0; stable && r < BZBoard::getRows(); r++)
				{
					for (c = 0; stable && c < BZBoard::getColumns(); c++)
					{
						BZBubble* pbubble = BZBoard::_getBubble(r, c);
						if (null != pbubble)
						{
							if (!pbubble->isStoped())
							{
								stable = false;
							}
						}
					}
				}
				_Info("GS_LevelUpWaiting 2");
				if (stable)
				{
					//lock all bubbles
					int r, c;
					for (r = 0; stable && r < BZBoard::getRows(); r++)
					{
						for (c = 0; stable && c < BZBoard::getColumns(); c++)
						{
							BZBubble* pbubble = BZBoard::_getBubble(r, c);
							if (null != pbubble)
							{
								pbubble->lock(true);
								pbubble->setPose("xxxx");
							}
						}
					}

					_Info("GS_LevelUpWaiting dispatch levelup event");
					//dispatch level up event
					this->dispatchEvent(new CAEventCommand(this, ST_UserDefined, "levelup", &_nLevel));
					this->dispatchEvent(new CAEventCommand(this, ST_UserDefined, "level_stars", &_stars));
					_Info("GS_LevelUpWaiting -> GS_LevelUpPaused");
					setState(GS_LevelUpPaused);
				}
			}
		}
		break;
	case GS_LevelUpPaused:
		_debug_log = "GS_LevelUp P/W/P";
		//do nothing
		break;
	case GS_LevelUpResume:
		{
			_debug_log = "GS_LevelUp P/W/P/R";
			_Info("GS_LevelUpResume");
			//now we check if we can LU
			//if all bubbles standing
			int r, c;
			for (r = 0; r < BZBoard::getRows(); r++)
			{
				for (c = 0; c < BZBoard::getColumns(); c++)
				{
					BZBubble* pbubble = BZBoard::_getBubble(r, c);
					if (null != pbubble)
					{
						delay += CAUtils::Rand(0.02f, 0.04f);
						_lockAndKill(pbubble, delay);
					}
				}
			}
			_Info("GS_LevelUpResume -> GS_SpecEffecting");
			setState(GS_SpecEffecting);

			_stars = 0;
			_psprStarHolders[0]->reset();
			_psprStarHolders[1]->reset();
			_psprStarHolders[2]->reset();
		}
		break;
	case GS_SpecEffecting:
		{
			if (!CAString::endsWith(_debug_log, "/SE"))
				_debug_log += "/SE";
			int n = _nCounter % 10;
			if (1 != n)
			{
				break;
			}
			bool finished = true;
			int r, c;
			for (r = 0; finished && r < BZBoard::getRows(); r++)
			{
				for (c = 0; finished && c < BZBoard::getColumns(); c++)
				{
					BZBubble* pb = BZBoard::_getBubble(r, c);
					if (null != pb && pb->isLocked())
					{
						EBubbleState bs = pb->getState();
						if (bs >= BS_Boom && bs < BS_Died)
							finished = false;
					}
				}
			}
			if (finished)
			{
				setState(GS_Running);
			}
		}
		break;
	case GS_GameOver:
		_debug_log = "GS_GameOver";
		break;
	case GS_GamePaused:
		_debug_log = "GS_GamePaused";
		break;
	case GS_Running:
		_debug_log = "GS_Running";
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
					CCSize size = CAWorld::sharedWorld()->getScreenSize();
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
	if (score > 10000)
	{
		//change color will cause this score bigger than 10000 even more
		score = 10000;
	}
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
		
		_onLevelChanged(false);
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

void BZGameClassic::_onLevelChanged(bool first)
{
	GUARD_FUNCTION();

	BZGame::_onLevelChanged(first);
	BZLevelParams params;
	
	params.fDelayOneRow = _dropline_interval[_nLevel]; // _LERP_LEVEL_PARAM(fDelayOneRow);
	params.timeDelayBorn	= _LERP_LEVEL_PARAM(timeDelayBorn);
	params.nAvailableStars	= (int)_LERP_LEVEL_PARAM(nAvailableStars);
	params.nRangeBubbleBorn	= (int)_LERP_LEVEL_PARAM(nRangeBubbleBorn);
	params.fPrebornLines	= _LERP_LEVEL_PARAM(fPrebornLines);

	this->setLevelParams(params);

	_nCombo = 0;

	if (_nLevel == 1)
	{
		_nPrebornLines = 0;
	}
	else
	{
		//calculate next level born strategy
		//how many lines to push down?
		_nPrebornLines = (int)params.fPrebornLines;
	}

	if (!first)
	{
		setState(GS_LevelUpPrepare);
	}

	_propManager.onLevelChanged(_nLevel);
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

void BZGameClassic::initializeProps(const string& difficulty, 
	BZSpriteButtonItem* pbtnBoom, 
	BZSpriteButtonItem* pbtnSameColor, 
	BZSpriteButtonItem* pbtnChangeColor,
	int limitBoom, int limitSameColor, int limitChangeColor)
{
	_propManager.initializeProp(difficulty, pbtnBoom, pbtnSameColor, pbtnChangeColor,
		_limitBoom = limitBoom, _limitSameColor = limitSameColor, _limitChangeColor = limitChangeColor);
}

void BZGameClassic::initializeStarHolder(BZSpriteStarHolder* p1, BZSpriteStarHolder* p2, BZSpriteStarHolder* p3)
{
	_Assert(p1);
	_Assert(p2);
	_Assert(p3);
	_psprStarHolders[0] = p1;
	_psprStarHolders[1] = p2;
	_psprStarHolders[2] = p3;
}

int BZGameClassic::_calcComboScore(int score)
{
	return (int)(score * _combo_rate * _nCombo);
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
	_showScore(posCenter, score, 0.9f);

	//CAStringMap<CAInteger> props;
	int nScorePlus = _calcComboScore(score);

	if (_nCombo < 12)
		_nCombo++;

	//bubble will add this effect inside
	//pbCenter->addEffect("effect_booom_back", "stand", true);
	CCArray* _bubbles = pblock->getBubbles();


	int bc = _bubbles->count();
	const char* prop = null;
	//const char* effect = null;
	//const char* props[] = {BUBBLE_PROP_BOOOM, BUBBLE_PROP_SAMECOLOR, BUBBLE_PROP_CHANGECOLOR, BUBBLE_PROP_STAR};
	//int available = 0;
	if (_nLevel < _limitChangeColor)
	{
		//do not generate any prop
		//available = 0;
	}
	else if (_nLevel < _limitSameColor)
	{
		//available = 1;
		if (bc < 5)
		{
		}
		else if (bc >= 5 && bc <= 6)
		{
			prop = BUBBLE_PROP_CHANGECOLOR;
		}
		else
		{
			prop = BUBBLE_PROP_STAR;
		}
	}
	else if (_nLevel < _limitBoom)
	{
		//available = 2;
		if (bc < 5)
		{
		}
		else if (bc >= 5 && bc < 7)
		{
			prop = BUBBLE_PROP_CHANGECOLOR;
		}
		else if (bc >= 7 && bc < 9)
		{
			prop = BUBBLE_PROP_SAMECOLOR;
		}
		else
		{
			prop = BUBBLE_PROP_STAR;
		}
	}
	else
	{
		//available = 3;
		if (bc < 5)
		{
		}
		else if (bc >= 5 && bc < 7)
		{
			prop = BUBBLE_PROP_CHANGECOLOR;
		}
		else if (bc >= 7 && bc < 9)
		{
			prop = BUBBLE_PROP_SAMECOLOR;
		}
		else if (bc >= 9 && bc < 11)
		{
			prop = BUBBLE_PROP_BOOOM;
		}
		else
		{
			prop = BUBBLE_PROP_STAR;
		}
	}

	if (null != prop)
	{
		if (prop == BUBBLE_PROP_STAR)
		{
			if (_stars < 3)
			{
				_stars++;
				CCPoint p = posCenter;
				BZBoard::getBubbleRenderPos(p);
				_psprStarHolders[_stars - 1]->addStar(prop, p);
			}
		}
		else
		{
			pbCenter->setRebornBubble(pblock->getBubbleType().c_str(), prop);
		}
	}

	posCenter = pbCenter->getPos();
	posCenter.y += 1.0f;
	_nScore += nScorePlus;
	_onScoreChanged();

	if (nScorePlus > 0)
	{
		_showScore(posCenter, nScorePlus, 1.1f + 0.07f * _nCombo, true);
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
		// some bubbles may be falling, but stucked by bubble below it
		// for these bubble, we give them a way to fix their state
		//!!! if (!BZBoard::isHeaderLineFull())
		//!!!	return false;
		if (BZBoard::getBornBubbles(null, 0) != BZBoard::_cols)
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
				//this may be null, I do NOT know why!!!!!
				//_Assert(null != pb);
				if (null == pb || pb->getState() != BS_Standing)
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
		//_Assert(null == _bubblesGrabbed[finger]);
		if (null != _bubblesGrabbed[finger])
			_bubblesGrabbed[finger]->release();
		//_bubblesGrabbed[finger] = null;
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
			_nCombo = 0;
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
