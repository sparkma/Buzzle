#if !defined(_TSSTAGE_TEST_H_)
#define _TSSTAGE_TEST_H_

#include "AStage.h"

class BZStageTest :
	public CAStage
{
public:
	BZStageTest(const char* name);
	virtual ~BZStageTest(void);
};

#endif //_TSSTAGE_TEST_H_