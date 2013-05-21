#include "BZWorld.h"
#include "AUserData.h"

BZWorld::BZWorld(void)
{
}

BZWorld::~BZWorld(void)
{
}

int BZWorld::getStars(const char* mode, const char* diff, int* stars, int scount)
{
	char szKey[128];

	memset(stars, 0, sizeof(int) * scount);
	sprintf(szKey, "%s-%s-level", mode, diff);
	int mlevel = CAUserData::sharedUserData().getInteger(szKey, 0);
	int level;
	for (level = 0; level < min(mlevel, scount); level++)
	{
		sprintf(szKey, "%s-%s-%d-stars", mode, diff, level);
		stars[level] = CAUserData::sharedUserData().getInteger(szKey, 0);
	}
	return level;
}

void BZWorld::saveRecord(const char* mode, const char* diff, int level, int stars, int score)
{
	char szKey[128];

	sprintf(szKey, "%s-%s-%d-stars", mode, diff, level);
	CAUserData::sharedUserData().setInteger(szKey, stars);
	sprintf(szKey, "%s-%s-%d-score", mode, diff, level);
	CAUserData::sharedUserData().setInteger(szKey, score);
	sprintf(szKey, "%s-%s-level", mode, diff);
	int mlevel = CAUserData::sharedUserData().getInteger(szKey, 0);
	if (level + 1 > mlevel)
	{
		CAUserData::sharedUserData().setInteger(szKey, level + 1);
	}
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
string getConfig()
{
	return "mg=bool:false";
}

void postGameEvent(const char* key, const char* value)
{
	//_Info("Game Event:%s=%s", key, value);
}

string getLanguage()
{
	return "cn";
}
#endif

