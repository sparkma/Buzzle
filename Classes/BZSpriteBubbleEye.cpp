
#include "BZSpriteBubbleEye.h"
#include "AStageLayer.h"

typedef enum enumEyeState
{
	ES_NA = -1,
	ES_Open = 0,
	ES_Lazy,
	ES_Blink,
	ES_Close,
}
EEyeState;

BZSpriteBubbleEye::BZSpriteBubbleEye(CAStageLayer* player, const char* name) : BZSpriteCommon(player, name)
{
	if (player)
	{
		_lastTime = (int)(player->getTimeNow() * 1000);
	}
	_interval = 0;
	_estate = ES_NA;
}

BZSpriteBubbleEye::~BZSpriteBubbleEye(void)
{
}

void BZSpriteBubbleEye::onStateChanged(const string& olds, const string& news)
{
	BZSpriteCommon::onStateChanged(olds, news);
}

void BZSpriteBubbleEye::onUpdate()
{
	BZSpriteCommon::onUpdate();

	int time = (int)(layer()->getTimeNow() * 1000);
	int diff = time - _lastTime;
	if (diff < _interval)
		return;

	_lastTime = time;
	float r = CAUtils::Rand();
	switch (_estate)
	{
	case ES_NA:
		_interval = 1;
		_estate = ES_Open;
		break;
	case ES_Open:
		setState("opening");
		_interval = (int)(CAUtils::Rand(3.0f, 7.0f) * 1000); 
		if (r > 0.3f) _estate = ES_Blink;
		else _estate = ES_Lazy;
		break;
	case ES_Blink:
		setState("blink");
		_interval = 1000; //(CAUtils::Rand(3.0f, 7.0f) * 1000); 
		if (r < 0.3f) _estate = ES_Blink;
		else if (r < 0.8f) _estate = ES_Open;
		else _estate = ES_Lazy;
		break;
	case ES_Lazy:
		setState("lazy");
		_interval = (int)(CAUtils::Rand(1.0f, 2.0f) * 1000); 
		if (r < 0.5f) _estate = ES_Close;
		else _estate = ES_Open;
		break;
	case ES_Close:
		setState("close");
		_interval = 200;
		if (r < 0.2f) _estate = ES_Lazy;
		else _estate = ES_Open;
		break;
	}
}

#include "AWorld.h"
REG_CLASS(BZSpriteBubbleEye);
