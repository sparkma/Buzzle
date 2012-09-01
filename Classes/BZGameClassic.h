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

	virtual void _doBornStrategy();
	virtual bool _canBoom(BZBlock* pblock) const;
	virtual void _onScoreChanged();
	virtual void _onLevelChanged();
public:
	BZGameClassic(CAStageLayer* player);
	virtual ~BZGameClassic();

	void initLevelParams(int levels, int bubble_score, int level_base_score, int level_mul_score,
		const BZLevelParams& levelmin, const BZLevelParams& levelmax);

	virtual float getLevelPercent() const;

	virtual bool canShowBoomScore() const { return true; }
	virtual int calculateScore(BZBlock* pblock) const;
	virtual void onEvent(CAEvent* pevt);
};

#endif //_BZGAME_MODE_CLASSIC_H_
