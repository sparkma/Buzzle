
#include "BZStagePlayLayerGame.h"
#include "AWorld.h"
#include "AStage.h"
#include "AString.h"
#include "BZSpriteCommon.h"
#include "BZSpriteButton.h"

BZStagePlayLayerGame::BZStagePlayLayerGame(CAStage* pstage, CAStageLayer* playerParent) : CAStageLayer(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
#if defined(_DEBUG_INFO)
	_pInfo = null;
#endif
}

BZStagePlayLayerGame::~BZStagePlayLayerGame(void)
{
	_Trace("%s destroyed", __FUNCTION__);

#if defined(_DEBUG_INFO)
	if (_pInfo)
	{
		_pInfo->release();
		_pInfo = null;
	}
#endif
}

bool BZStagePlayLayerGame::checkCondition(CAState* from, const CATransition& trans)
{
	//static condition "true" "false" check
	if (CAMachine::checkCondition(from, trans))
		return true;

	return false;
}

void BZStagePlayLayerGame::onStateBegin(CAState* from, void* param) 
{
	const string& fname = from->getFullName();
	if (0) ;
	else if (CAString::startWith(fname, "root.init"))		//_onStateBeginInit(from);
	{
		_Assert(this->_getSubLayer("prepare")	->getCurrentState()->getFullName() == "root.idle");
		_Assert(this->_getSubLayer("play")		->getCurrentState()->getFullName() == "root.idle");
		_Assert(this->_getSubLayer("shop")		->getCurrentState()->getFullName() == "root.idle");
	}
	else if (fname == "root.prepare")
	{
		CAStageLayer* pl = this->_getSubLayer("game.prepare");
		_Assert(pl);
		pl->setConditionResult("root.idle@user.show", true);
		_pstage->setFocus(pl);
	}
	else if (fname == "root.play")
	{
		CAStageLayer* pl = this->_getSubLayer("game.play");
		_Assert(pl);
		pl->setConditionResult("root.idle@user.show", true);
		_pstage->setFocus(pl);
	}
	else if (fname == "root.shop")
	{
		CAStageLayer* pl = this->_getSubLayer("game.shop");
		_Assert(pl);
		pl->setConditionResult("root.idle@user.show", true);
		_pstage->setFocus(pl);
	}
	else
	{
	}
};

void BZStagePlayLayerGame::onStateEnd(CAState* from, void* param) 
{
	//nothing to do here
};

void BZStagePlayLayerGame::onEnter()
{
	GUARD_FUNCTION();

	CAStageLayer::onEnter();

#if defined(_DEBUG_INFO)
	CCSize size = CAWorld::getScreenSize();
	size.width *= 0.9f;
	size.height = 18 * 6;
    _pInfo = CCLabelTTF::labelWithString(" ", size, kCCTextAlignmentLeft, "Arial", 16);
	_pInfo->setAnchorPoint(ccp(0.0f, 0.0f));
	_pInfo->setPosition( ccp(0, 20) );
	ccColor3B c;
	c.r = 255; c.g = c.b = 0;
	_pInfo->setColor(c);
	_pInfo->setVertexZ(999);
	//_pInfo->setOpacity(0x80);
	this->stage()->addChild(_pInfo, 10000);
	_pInfo->retain();
#endif
	//_pstage->setFocus(this);
}

#if defined(_DEBUG_INFO)
static ETouchState _old_touch_state = (ETouchState)-1;
#endif

string BZStagePlayLayerGame::debuglog() 
{ 
	string ret = "";

	CAStageLayer* pl;
	
	ret += "game:";
	char sz[1024];
	sprintf(sz, "fps:%.1f;", stage()->fps());
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
	
	pl = this->_getSubLayer("game.prepare");
	ret += "prepare:";
	ret += pl ? pl->debuglog() : "";
	ret += "\n";

	pl = this->_getSubLayer("game.play");
	ret += "play:";
	ret += pl ? pl->debuglog() : "";
	ret += "\n";

	pl = this->_getSubLayer("game.shop");
	ret += "shop:";
	ret += pl ? pl->debuglog() : "";
	ret += "\n";

	return ret; 
}

void BZStagePlayLayerGame::onUpdate() 
{
	CAStageLayer::onUpdate();

#if defined(_DEBUG_INFO)
	_pInfo->setString(debuglog().c_str());
#endif
};

void BZStagePlayLayerGame::onExit()
{
	CAStageLayer::onExit();
#if defined(_DEBUG_INFO)
	if (_pInfo)
	{
		_pInfo->release();
		_pInfo = null;
	}
#endif
}

void BZStagePlayLayerGame::onEvent(CAEvent* pevt)
{
	CAStageLayer::onEvent(pevt);

	switch (pevt->type())
	{
	case ET_Touch:
		{
			CAEventTouch* ptouch = (CAEventTouch*)pevt;
#if defined(_DEBUG_INFO)
			_old_touch_state = ptouch->state();
#endif
			//CCLOGINFO("ots = %d", _old_touch_state);
			switch (ptouch->state())
			{
			case kTouchStateGrabbed:
				{
				}
				break;
			case kTouchStateUngrabbed:
				{
					int a = 0;
				}
				break;
			}
		}
		break;
	case ET_Command:
		{
			CAEventCommand* pec = (CAEventCommand*)pevt;
			//some player tell us that we should navigate to shop ui
			const string& command = pec->command();
			string result = this->getLeafState()->getFullName();
			result += "@";
			result += command;
			this->setConditionResult(result.c_str(), true);
		}
		break;
	}
}

