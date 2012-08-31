#if !defined(_BZGAME_MODE_CLASSIC_H_)
#define _BZGAME_MODE_CLASSIC_H_

#include "BZGame.h"

class BZGameClassic : public BZGame
{
protected:
	int			_level;
	//[0] level 1 params
	//[1] level 128 params
	BZLevelParams	_paramsPreloaded[2];

	virtual void _doBornStrategy();
	virtual bool _canBoom(BZBlock* pblock) const;
public:
	BZGameClassic(CAStageLayer* player);
	virtual ~BZGameClassic();

	virtual bool canShowBoomScore() const { return true; }
	virtual int calculateScore(BZBlock* pblock) const;
	virtual void onEvent(CAEvent* pevt);
};

#endif //_BZGAME_MODE_CLASSIC_H_
