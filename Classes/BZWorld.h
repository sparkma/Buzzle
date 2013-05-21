#if !defined(_TSWORLD_H_)
#define _TSWORLD_H_

#include "AWorld.h"

class BZWorld :
	public CAWorld
{
public:
	BZWorld(void);
	virtual ~BZWorld(void);

	static int getStars(const char* mode, const char* diff, int* stars, int scount);
	static void saveRecord(const char* mode, const char* diff, int level, int stars, int score);
};

#endif //_TSWORLD_H_