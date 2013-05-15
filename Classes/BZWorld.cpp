#include "BZWorld.h"

BZWorld::BZWorld(void)
{
}

BZWorld::~BZWorld(void)
{
}

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


