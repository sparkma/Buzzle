
#include "BZStageTestLayerTestBubbles.h"
#include "AWorld.h"
#include "AStage.h"
#include "BZSpriteCommon.h"

BZStageTestLayerTestBubbles::BZStageTestLayerTestBubbles(CAStage* pstage, CAStageLayer* playerParent) : CAStageLayer(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
}

BZStageTestLayerTestBubbles::~BZStageTestLayerTestBubbles(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStageTestLayerTestBubbles::onEnter()
{
	GUARD_FUNCTION();

	CAStageLayer::onEnter();

	_pstage->setFocus(this);
}

void BZStageTestLayerTestBubbles::onUpdate() 
{
	CAStageLayer::onUpdate();
};

void BZStageTestLayerTestBubbles::onExit()
{
	CAStageLayer::onExit();
}

void BZStageTestLayerTestBubbles::onEvent(CAEvent* pevt)
{
	CAStageLayer::onEvent(pevt);

	switch (pevt->type())
	{
	case ET_Touch:
		{
			CAEventTouch* ptouch = (CAEventTouch*)pevt;
			const CCPoint& pos = ptouch->pt();
			if (pos.y > CAWorld::sharedWorld().getScreenSize().height * 0.8f)
			{
				_removeAllSprites();
			}
			else //if (pos.y  < CAWorld::sharedWorld().getScreenSize().height * 0.2f)
			{
				_createSprite(pos);
			}
			switch (ptouch->state())
			{
			case kTouchStateGrabbed:
				{
				}
				break;
			case kTouchStateUngrabbed:
				{
				}
				break;
			}
		}
		break;
	}
}

void BZStageTestLayerTestBubbles::_removeAllSprites()
{
	CCArray* psprs = _getNamedSprites();
	CAObject* pobj;
	CCARRAY_FOREACH(psprs, pobj)
	{
		CASprite* pspr = (CASprite*)pobj;
		this->removeSprite(pspr);
	}
}

void BZStageTestLayerTestBubbles::_createSprite(const CCPoint& pos)
{
	string pose = "fall0";
	
	BZSpriteCommon* pspr = new BZSpriteCommon(this, "spark0");
	pspr->setState(pose);
	pspr->setDeadPose(pose);

	pspr->setPos(pos);

	this->addSprite(pspr);
}
