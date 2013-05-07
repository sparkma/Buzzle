#if !defined(_BZGAME_MODE_CLASSIC_H_)
#define _BZGAME_MODE_CLASSIC_H_

#include "BZGame.h"

#define _MAX_GRABBED_BLOCKS 4

class BZGameClassic : public BZGame
{
protected:
	int				_level_base_score;
	int				_level_mul_score;
	int				_bubble_score;

	int				_levels;
	//[0] min level
	//[1] max level
	BZLevelParams	_paramsPreloaded[2];
	int				_nPrebornLines;

	float			_score_dx;
	float			_score_scale;
	int _calculateScore(BZBlock* pblock) const;
	void _showScore(const CCPoint& pos, int score, float scale = 1.0f);

	int				_nLevel;
	int				_nCombo;

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
	virtual void _onLevelChanged();

	int _scores[256];
	int _getLevelScore(int level) const;
	float _dropline_interval[256];

	void _addFireEffectOn(BZBubble* pb);

	virtual BZBubble* _onUpdateBlock(BZBlock* pblock);
	void _findBigestBlockType(string& btype, string& sectype);
	void _lockAndKill(BZBubble* pbubble, float delay);
	virtual void _doBubbleDied(BZBubble* pbubble);
public:
	BZGameClassic(CAStageLayer* player);
	virtual ~BZGameClassic();

	void initLevelParams(int levels, int bubble_score, 
		int level_base_score, int level_mul_score,
		float level_base_drop, float level_mul_drop,
		float score_dx, float score_sacle,
		int curlevel,
		const BZLevelParams& levelmin, const BZLevelParams& levelmax);

	void setLevel1Map(const string& map)
	{
		_mapLevel1 = map;
	}

	virtual void clear();

	int getLevel() const { return _nLevel; }
	virtual float getLevelPercent() const;

	virtual void onBlockStarsChanged(BZBlock* pblock, int stars);

	virtual void onUpdate();
	virtual bool onEvent(const CAEvent* pevt);

	virtual bool isGameOver() const;
};

#endif //_BZGAME_MODE_CLASSIC_H_
