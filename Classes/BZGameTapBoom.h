#if !defined(_BZGAME_MODE_TAPBOOM_H_)
#define _BZGAME_MODE_TAPBOOM_H_

#include "BZGame.h"

typedef enum enumTapBoomState
{
	TBS_NA = 0,
	TBS_InitializeMap,
}
TTapBoomState;
class BZGameTapBoom : public BZGame
{
protected:
	int				_nLevel;
	TTapBoomState	_state;
	string			_map;

	void _initializeMap();
	virtual void _doBornStrategy();
public:
	BZGameTapBoom(CAStageLayer* player);
	virtual ~BZGameTapBoom();

	void createLevel(int level);
};

#endif //_BZGAME_MODE_TAPBOOM_H_
