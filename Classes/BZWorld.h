#if !defined(_TSWORLD_H_)
#define _TSWORLD_H_

#include "AWorld.h"

class BZWorld :
	public CAWorld
{
public:
	BZWorld(void);
	virtual ~BZWorld(void);

	virtual CAStage* createStage(const char* name);
	virtual CAStageLayer* createLayer(CAStage* pstage, CAStageLayer* playerParent, const char* name);
	virtual CASprite* createSprite(CAStageLayer* player, const char* name);
};

#endif //_TSWORLD_H_