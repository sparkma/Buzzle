#if !defined(_BZGAME_MODE_CLASSIC_H_)
#define _BZGAME_MODE_CLASSIC_H_

#include "BZGame.h"

#define _MAX_GRABBED_BLOCKS 4

#define BUBBLE_PROP_CONNECTOR	"prop_connector"
#define BUBBLE_PROP_BOOOM		"prop_booom"
#define BUBBLE_PROP_SAMECOLOR	"prop_samecolor"
#define BUBBLE_PROP_CHANGECOLOR "prop_changecolor"
#define BUBBLE_PROP_STAR		"prop_star"

#define BUBBLE_EFFECT_CHANGECOLOR		"effect_changecolor"


class BZSpriteButtonItem;
class BZGameClassicPropManager
{
private:
	int _level;
	string _difficulty;
	BZSpriteButtonItem* _pbtnBoom;
	BZSpriteButtonItem* _pbtnSameColor;
	BZSpriteButtonItem* _pbtnChangeColor;
public:
	BZGameClassicPropManager()
	{
		_level = 0;
		_pbtnBoom = null;
		_pbtnSameColor = null;
		_pbtnChangeColor = null;
	};

	void initializeProp(const string& difficulty, 
		BZSpriteButtonItem* pbtnBoom, 
		BZSpriteButtonItem* pbtnSameColor, 
		BZSpriteButtonItem* pbtnChangeColor,
		int limitBoom, int limitSameColor, int limitChangeColor);

	void addPropPiece(const string& name, const CCPoint& pos);
	void onLevelChanged(int level);
	void setFillRate(float r);
	void setGameState(EGameState gs);
};

class BZSpriteStarHolder;
class BZGameClassic : public BZGame
{
protected:
	BZGameClassicPropManager	_propManager;

	int				_stars;
	BZSpriteStarHolder*			_psprStarHolders[3];

	int				_level_base_score;
	int				_level_mul_score;
	int				_bubble_score;

	string			_debug_log;

	int				_levels;
	//[0] min level
	//[1] max level
	BZLevelParams	_paramsPreloaded[2];
	int				_nPrebornLines;

	float			_score_dx;
	float			_score_scale;
	int _calculateScore(BZBlock* pblock) const;
	void _showScore(const CCPoint& pos, int score, float scale = 1.0f, bool plus = false);

	int				_nLevel;
	int				_nCombo;
	float _combo_rate;
	int _calcComboScore(int score);

	//bool			_bIsHeaderlineFull;
	ccTime			_timeLastRow;

	int				_mapProcessed;
	string			_mapLevel1;

	int				_bubbleGenerated;
	int				_lastStarIndex;
	virtual void _doBornStrategy();

	//we can grab 4 bubbles at same time
	BZBubble* _bubblesGrabbed[_MAX_GRABBED_BLOCKS];
	BZBubble* _getGrabbedBubble(int finger);
	bool _hasBeenOccupied(int r, int c, BZBubble* pbExclude = null);
	void _setGrabbedBubble(int finger, BZBubble* pbubble);
	void _onTouchUngrabbed(CAEventTouch* ptouch);
	void _onTouchMoving(CAEventTouch* ptouch);
	void _onTouchGrabbed(CAEventTouch* ptouch);

	int _nLevelState;
	void _handleBornStrategyLevel1();
	void _handleBornStrategyLevelN();
	void _handleBornStrategyLevelN10();
	bool _generateBubble(int& col, string& type, bool& star);

	virtual bool _canBoom(BZBlock* pblock) const;
	virtual void _onScoreChanged();
	virtual void _onLevelChanged(bool first);

	int _scores[256];
	int _getLevelScore(int level) const;
	float _dropline_interval[256];

	void _addFireEffectOn(BZBubble* pb);

	virtual BZBubble* _onUpdateBlock(BZBlock* pblock);
	void _findBigestBlockType(string& btype, string& sectype);
	void _lockAndKill(BZBubble* pbubble, float delay);
	virtual void _doBubbleDied(BZBubble* pbubble);

	int _limitBoom;
	int _limitSameColor;
	int _limitChangeColor;

	void _freeAllResources();
public:
	BZGameClassic(CAStageLayer* player);
	virtual ~BZGameClassic();

	void initLevelParams(int levels, int bubble_score, 
		int level_base_score, int level_mul_score,
		float level_base_drop, float level_mul_drop,
		float score_dx, float score_sacle,
		float combo_rate,
		int curlevel,
		const BZLevelParams& levelmin, const BZLevelParams& levelmax);

	void initializeProps(const string& difficulty, 
		BZSpriteButtonItem* pbtnBoom, 
		BZSpriteButtonItem* pbtnSameColor, 
		BZSpriteButtonItem* pbtnChangeColor,
		int limitBoom, int limitSameColor, int limitChangeColor);

	void initializeStarHolder(BZSpriteStarHolder* p1, BZSpriteStarHolder* p2, BZSpriteStarHolder* p3);

	void setLevel1Map(const string& map)
	{
		_mapLevel1 = map;
	}

	virtual string debuglog();

	virtual void setState(EGameState s);

	int getLevel() const { return _nLevel; }
	virtual float getLevelPercent() const;
	int getStars() const { return _stars; }

	virtual void onBlockStarsChanged(BZBlock* pblock, int stars);

	virtual void onUpdate();
	virtual bool onEvent(const CAEvent* pevt);
	virtual void onResume();

	virtual bool isGameOver() const;
};

#endif //_BZGAME_MODE_CLASSIC_H_
