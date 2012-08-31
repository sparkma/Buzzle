
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
		if (!pspr->isDied())
		{
			this->removeSprite(pspr);
		}
	}
}

void BZStageTestLayerTestBubbles::_createSprite(const CCPoint& pos_)
{
#if 0
	string pose = "fall0";
	
	BZSpriteCommon* pspr = new BZSpriteCommon(this, "spark0");
	pspr->setState(pose);
	pspr->setDeadPose(pose);

	pspr->setPos(pos);

	this->addSprite(pspr);
#else
	int score = 1234567890;

	CCPoint pos = pos_;

	char sz[16];
	sprintf(sz, "%d", score);
	int i, len = strlen(sz);
	float dx = 20.0f;
	pos.x -= dx * len / 2;
	for (i = 0; i < len; i++)
	{
		BZSpriteCommon* pspr = new BZSpriteCommon(this, "number_3");
		char szPose[16];
		szPose[0] = sz[i];
		szPose[1] = 0;
		pspr->switchPose(szPose); //make this pose frames work
		CCPoint posR = pos;
		//_pboard->getBubbleRenderPos(posR);
		pspr->setPos(posR);
		pos.x += dx;
		pos.y += 0.0f;
		pspr->setZOrder(120.0f);
		strcpy(szPose, "splash");
		pspr->setState(szPose);
		pspr->setDeadPose(szPose);
		this->addSprite(pspr);
		//pspr->onUpdate();
	}
#endif
}
