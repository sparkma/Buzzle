#if !defined(_TSSTAGE_PLAY_H_)
#define _TSSTAGE_PLAY_H_

#include "AStage.h"

class BZStagePlay :
	public CAStage
{
public:
	BZStagePlay(const char* name);
	virtual ~BZStagePlay(void);

	virtual void onEnter();
};

#endif //_TSSTAGE_PLAY_H_