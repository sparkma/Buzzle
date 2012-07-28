
#include "BZStageTestLayerTestBubbles.h"
#include "AWorld.h"
#include "AStage.h"
#include "BZSpriteCommon.h"

BZStageTestLayerTestBubbles::BZStageTestLayerTestBubbles(CAStage* pstage, CAStageLayer* playerParent) : CAStageLayer(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
#if defined(_USE_MEM_GUARDER_)
	_pInfo = null;
#endif
	_pTarget = null;
	_off = CCPointZero;
}

BZStageTestLayerTestBubbles::~BZStageTestLayerTestBubbles(void)
{
	_Trace("%s destroyed", __FUNCTION__);

	if (null != _pTarget) 
		_pTarget->release();
	_pTarget = null;

#if defined(_USE_MEM_GUARDER_)
	_pInfo->release();
#endif
}

void BZStageTestLayerTestBubbles::onEnter()
{
	int i, count;
	i = count = 0;

	GUARD_FUNCTION();

	CAStageLayer::onEnter();

	//this->setScale(0.5f);

#if defined(_USE_MEM_GUARDER_)
	// add a label shows "Hello World"
	// create and initialize a label
    _pInfo = CCLabelTTF::labelWithString("I", "Arial", 24);
	// ask director the window size
	CCSize size = CAWorld::getScreenSize();

	_pInfo->setAnchorPoint(ccp(1.0f, 0.0f));
	// position the label on the center of the screen
	_pInfo->setPosition( ccp(size.width, 0) );

	// add the label as a child to this layer
	this->addChild(_pInfo, 1);

	_pInfo->retain();
#endif
	//return;


#if defined(_TEST_SEA_)
	const char* psz[] = {"sea", "wave-01", "wave-01", "wave-02", "wave-02", "wave-03", "wave-03", "sand-01", "sand-01"};
	
	for (i = 0; i < 9; i++)
	{
		BZSpriteCommon* pspr;

		pspr = (BZSpriteCommon*)CAWorld::sharedWorld().createSprite(this, psz[i]);
		//pspr->setFollowCamera(false);
		this->addSprite(pspr, 60 + i);
		pspr->setState("stand");

		if (0 != i)
		{
			_sea[i - 1] = pspr;
		}

		CCSize siz = CAWorld::sharedWorld().getWinSize();
		float w = siz.width;

		switch(i)
		{
		case 0:
			pspr->setFollowCamera(false);
			break;
		case 1:
		case 2:
			_wave1[i - 1] = pspr;
			pspr->setMoveSpeed(30);
			pspr->setPos(ccp((i - 1) * w, 0));
			break;
		case 3:
		case 4:
			_wave2[i - 3] = pspr;
			pspr->setMoveSpeed(20);
			pspr->setPos(ccp((i - 3) * w, 0));
			break;
		case 5:
		case 6:
			_wave3[i - 5] = pspr;
			pspr->setMoveSpeed(10);
			pspr->setPos(ccp((i - 5) * w, 0));
			break;
		case 7:
		case 8:
			_sand1[i - 7] = pspr;
			pspr->setMoveSpeed(10);
			pspr->setPos(ccp((i - 7) * w, 0));
			break;
		}
	}
#endif
#if defined(_TEST_PLAYER_)
	BZSpritePlayer* player;

	player = (BZSpritePlayer*)CAWorld::sharedWorld().createSprite(this, "player");
	player->setVertexZ(1);
	this->addSprite(player, 100);
	player->setState("born");
	_pTarget = player;
#endif

#if defined(_TEST_FISH_)
	for (i = 0; i < _FISHES_; i++)
	{
		BZSpriteFish* pf;
		pf = (BZSpriteFish*)CAWorld::sharedWorld().createSprite(this, "fish-01");
		CCPoint pos = stage()->getPointFromPercent(ccp(CAUtils::Rand(1.0f, 1.2f), CAUtils::Rand()), true);
		pf->setPos(pos);
		float sd = CAUtils::Rand() * 20 + 20;
		float rt = CAUtils::Rand() * 30 - 15;
		pf->setMoveDirection(rt);
		pf->setMoveSpeed(-sd);
		this->addSprite(pf, 10); //(int)CAUtils::Rand(10, 12));
		pf->setVertexZ(1.8);
		pf->setState("swim");
		_pTarget = pf;
	}
#endif

#if defined(_TEST_BUBBLES_)
	count = _BUBBLES_COUNT;

	for (i = 0; i < count; i++)
	{
		float x, y;

		const CCSize& size = this->getContentSizeInPixels();
		x = CAUtils::Rand() * size.width;
		y = CAUtils::Rand() * 30.0f;
			
		BZSpriteBubble* pspr;
		pspr = (BZSpriteBubble*)CAWorld::sharedWorld().createSprite(this, "bubble");
		_Assert(this);

		float ats = CAUtils::Rand() * 0.2f + 0.1f;
		pspr->setScl(ats / 0.3f);
		pspr->setTimeScale(ats);

		//x += size.width / 2 * (CAUtils::Rand() - 0.5f);
		//y += size.height / 2 * (CAUtils::Rand() - 0.5f);
		pspr->setPos(ccp(x, y));
		pspr->setMoveDirection(0);
		pspr->setMoveSpeed(700);

		//stage()->setOffset(ccp(size.width / 2, 0));

		this->addSprite(pspr);

		pspr->switchPose("pop");
		int r = pspr->retainCount();
		r = 0;
	}
#endif
	if (_pTarget)
		_pTarget->retain();

}

void BZStageTestLayerTestBubbles::onUpdate() 
{
	int i = 0;

	CAStageLayer::onUpdate();

#if defined(_TEST_SEA_)
	CCRect rect;
	for (i = 0; i < 8; i++)
	{
		rect = _sea[i]->getBoundingBox();
		if (rect.origin.x + rect.size.width < _off.x)
		{
			CCPoint pos = _sea[i]->getPos();
			pos.x += rect.size.width * 2;
			_sea[i]->setPos(pos);
		}
	}
#endif

	{
		static int state = 0;
		CASprite* pspr = this->_getNamedSprite("text_yellow");
		if (pspr)
		{
			float timeNow = this->getTimeNow();
			float time10 = fmod(timeNow, 13.0f);
			int time = (int)time10;
			if (time == 10 && 1 == state)
			{
				state = 2;
				pspr->switchPose("fadeout");
				_Trace("%.2f-%.2f pose fo", time10, timeNow);
			}
			else if (time == 12 && 2 == state)
			{
				state = 0;
				_Trace("%.2f-%.2f idle", time10, timeNow);
			}
			else if (0 == state)
			{
				if (time == 9)
					state = 1;
				char sz[2];
				sz[0] = time + '0';
				sz[1] = 0;
				pspr->switchPose(sz);
				_Trace("%.2f-%.2f pose N", time10, timeNow);
			}
		}
	}

#if defined(_USE_MEM_GUARDER_)
	unsigned int alloced = gGetMemSizeAlloced();
	unsigned int maxalloced = gGetMemMaxSizeAlloced();
	char sz[128];
	if (_pTarget)
	{
		sprintf_s(sz, 128, "(%.0f,%.0f)(x:%.1f,y:%.1f)%d/%d/%d", 
			_off.x, _off.y,
			_pTarget->getPos().x, _pTarget->getPos().y, 
			CASprite::counter, alloced, maxalloced);
	}
	else
	{
		sprintf_s(sz, 128, "(%.0f,%.0f)%d/%d/%d", 
			_off.x, _off.y,
			CASprite::counter, alloced, maxalloced);
	}
	_pInfo->setString(sz);
#endif

	_off.x += (CAUtils::Rand(960.0f / 1.6f, 960.0f / 1.4f) * getTimeDelta()) / 5.0f;
	stage()->setOffset(_off);
};

void BZStageTestLayerTestBubbles::onExit()
{
	CAStageLayer::onExit();
}
