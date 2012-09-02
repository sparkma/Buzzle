#if !defined(_BZGAME_MODE_CLASSIC_H_)
#define _BZGAME_MODE_CLASSIC_H_

#include "BZGame.h"

class BZGameClassic : public BZGame
{
protected:
	int				_level;

	int				_level_base_score;
	int				_level_mul_score;
	int				_bubble_score;

	int				_levels;
	//[0] min level
	//[1] max level
	BZLevelParams	_paramsPreloaded[2];

	int				_mapProcessed;
	string			_mapLevel1;

	virtual void _doBornStrategy();

	void _handleBornStrategyLevel1();
	void _handleBornStrategyLevelN(int rows);

	virtual bool _canBoom(BZBlock* pblock) const;
	virtual void _onScoreChanged();
	virtual void _onLevelChanged();
public:
	BZGameClassic(CAStageLayer* player);
	virtual ~BZGameClassic();

	void initLevelParams(int levels, int bubble_score, int level_base_score, int level_mul_score,
		const BZLevelParams& levelmin, const BZLevelParams& levelmax);

	void setLevel1Map(const string& map)
	{
		_mapLevel1 = map;
	}

	virtual float getLevelPercent() const;

	virtual bool canShowBoomScore() const { return true; }
	virtual int calculateScore(BZBlock* pblock) const;
	virtual void onEvent(CAEvent* pevt);
};

#endif //_BZGAME_MODE_CLASSIC_H_
