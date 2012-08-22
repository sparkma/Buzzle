#if !defined(_BZGAME_MODE_TAPBOOM_H_)
#define _BZGAME_MODE_TAPBOOM_H_

#include "BZGame.h"

class BZGameTapBoom : public BZGame
{
protected:
	int			_level;
	//[0] level 1 params
	//[1] level 128 params
	BZLevelParams	_paramsPreloaded[2];

	virtual void _doBornStrategy();
public:
	BZGameTapBoom(CAStageLayer* player);
	virtual ~BZGameTapBoom();
};

#endif //_BZGAME_MODE_TAPBOOM_H_
