
#include "BZStageCommon.h"
#include "AWorld.h"
#include "BZStageCommon.h"

BZStageCommon::BZStageCommon(const char* name) : CAStage(name)
{
	_debug_info = true;
	_pInfo = null;
	_counter = 0;
}

BZStageCommon::~BZStageCommon(void)
{
}

void BZStageCommon::onEnter()
{
	GUARD_FUNCTION();

	CAStage::onEnter();

	if (_debug_info)
	{
		CCSize size = CAWorld::getScreenSize();
		size.width *= 0.9f;
#if defined(_DEBUG)
		size.height = 18 * 9;
#else
		size.height = 18 * 4;
#endif
		//_pInfo = CCLabelTTF::labelWithString(" ", size, kCCTextAlignmentLeft, "Arial", 16);
		_pInfo = CCLabelTTF::create(" ", "Arial", 12.0f, size, kCCTextAlignmentLeft);
		_pInfo->setAnchorPoint(ccp(0.0f, 0.0f));
		_pInfo->setPosition( ccp(0, 20) );
		_pInfo->setVertexZ(99);
		ccColor3B cc;
		cc.r = 255; cc.g = 0; cc.b = 0;
		_pInfo->setColor(cc);
		_pInfo->setOpacity(0x9b);
		this->addChild(_pInfo, 10000);
		_pInfo->retain();
	}
}

string BZStageCommon::status() 
{ 
	string ret = "";

	if (!_debug_info)
		return ret;

	ret += "game:";
	char sz[1024];
	sprintf(sz, "fps:%.1f;", this->fps());
	//ret += sz;
#if defined(_DEBUG)
	char szMem[128];
	unsigned int alloced = gGetMemSizeAlloced();
	unsigned int maxalloced = gGetMemMaxSizeAlloced();
	sprintf(szMem, " mem:%d/%d", alloced, maxalloced);
	strcat(sz, szMem);
#endif
	ret += sz;
	ret += "\n";

	CAStageLayer* player = getFocus();
	if (null != player)
	{
		ret += player->_sprite_container_debug_log(player->name().c_str(), false);
	}

	return ret;
}

void BZStageCommon::onUpdate() 
{
	CAStage::onUpdate();

	_counter++;
	if (_debug_info && null != _pInfo && 1 == (_counter % 10))
	{
		_pInfo->setString(status().c_str());
	}
};

void BZStageCommon::onExit()
{
	CAStage::onExit();
	if (_pInfo)
	{
		_pInfo->release();
		_pInfo = null;
	}
}

/*
void BZStageCommon::move2Layer(BZStageCommon* playerFrom, const char* layerTo)
{
	BZStageCommon* playerTo = (BZStageCommon*)this->getSubLayer(layerTo);
	playerFrom->hide();
	playerTo->setFrom(playerFrom->name());
	playerTo->show();
}

void BZStageCommon::back2Layer(BZStageCommon* playerFrom, const char* layerTo)
{
	BZStageCommon* playerTo = (BZStageCommon*)this->getSubLayer(layerTo);
	playerFrom->hide();
	playerTo->show();
}

void BZStageCommon::pushLayer(BZStageCommon* playerFrom, const char* layerTo)
{
	BZStageCommon* playerTo = (BZStageCommon*)this->getSubLayer(layerTo);
	playerFrom->pause();
	playerTo->show();
}

void BZStageCommon::popLayer(BZStageCommon* playerFrom, const char* layerTo)
{
	BZStageCommon* playerTo = (BZStageCommon*)this->getSubLayer(layerTo);
	playerFrom->pause();
	playerTo->show();
}
*/

REG_CLASS(BZStageCommon);
