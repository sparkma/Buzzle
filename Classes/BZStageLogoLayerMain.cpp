
#include "BZStageLogoLayerMain.h"

#include "AWorld.h"
#include "AStage.h"
#include "BZSpriteCommon.h"

#define STATE_PREPARE	"prepare"
#define STATE_FADEIN	"fadein"
#define STATE_LOADING	"loading"
#define STAGE_FADEOUT	"fadeout"
#define STATE_WAITING	"waiting"

#define SPR_LOGO "loading_logo"

BZStageLogoLayerMain::BZStageLogoLayerMain(CAStage* pstage, CAStageLayer* playerParent) : CAStageLayer(pstage, playerParent)
{
	_Info("%s allocated", __FUNCTION__);
	_pstageNext = null;
#if defined(_PERCENT_LABEL)
	_pInfo = null;
#endif
}

BZStageLogoLayerMain::~BZStageLogoLayerMain(void)
{
	_Info("%s destroyed", __FUNCTION__);
}

void BZStageLogoLayerMain::_setState(const string& s) { _state = s; _timeState = this->getTimeNow(); }
ccTime BZStageLogoLayerMain::_getStateTime() { return this->getTimeNow() - _timeState; }

void BZStageLogoLayerMain::onEnter()
{
	GUARD_FUNCTION();

#if defined(_PERCENT_LABEL)
	// add a label shows "Hello World"
	// create and initialize a label
    _pInfo = CCLabelTTF::labelWithString(" ", "Arial", 16);
	// ask director the window size
	CCSize size = CAWorld::getScreenSize();

	_pInfo->setAnchorPoint(ccp(0.5f, 0.0f));

	float laby = _settings.getFloat("label_y");
	// position the label on the center of the screen
	_pInfo->setPosition( ccp(size.width / 2, laby * size.height) );

	_pInfo->setVertexZ(10);
	// add the label as a child to this layer
	this->addChild(_pInfo, 100);

	_pInfo->retain();
#endif

	CAStageLayer::onEnter();

	_setState(STATE_PREPARE);
}

void BZStageLogoLayerMain::onUpdate() 
{
	CAStageLayer::onUpdate();
	
	const string& state = _state; //getState();
	if (state == STATE_PREPARE)
	{
		CASprite* pspr = _getNamedSprite(SPR_LOGO);
		if (null != pspr)
		{
			_setState(STATE_FADEIN);
		}
	}
	else if (state == STATE_FADEIN)
	{
		if (null == _pstageNext)
		{
			string sn = _settings.getString("stage_next");
			_pstageNext = CAWorld::sharedWorld()->createStage(sn.c_str());
			_Assert(_pstageNext);
			_pstageNext->retain();
			string progress_fill_spr = _settings.getString( "progress");
			CASprite* pspr = _getNamedSprite(progress_fill_spr.c_str());
			pspr->setSclX(0.01f);
		}

		CASprite* pspr = _getNamedSprite(SPR_LOGO);
		if (pspr->getState() != "fadein")
		{
			pspr->setState("fadein");
		}
		else
		{
			if (pspr->isAnimationDone())
			{
				_setState(STATE_LOADING);
			}
		}
	}
	else if (state == STATE_LOADING)
	{
		EStageLoadState s;
		s = _pstageNext->loadProgressively();
		char sz[64];
		float percent = _pstageNext->getLoadPercent();
		sprintf(sz, "%.1f%%", percent * 100.0f);
#if defined(_PERCENT_LABEL)
		_pInfo->setString(sz);
#endif
		_Trace("load stage returns:%d, %.2f", s, percent);

		string progress_fill_spr = _settings.getString( "progress");
		if (_getNamedSpritesCount(progress_fill_spr.c_str()))
		{
			CASprite* pspr = _getNamedSprite(progress_fill_spr.c_str());
			//if (percent > 0.02f) pspr->setVisible(true);
			pspr->setVisible(true);
			//pspr->setAlpha(percent);
			pspr->setSclX(percent);
			//_Info("load percent = %.2f", percent);
			//for (int j = 0; j < 1000; j++)
			//for (int i = 0; i < 1000000; i++);
		}
		
		if (SLS_Finished == s)
		{
			_setState(STAGE_FADEOUT);
#if defined(_PERCENT_LABEL)
			_pInfo->setString("");
#endif
			CASprite* pspr = _getNamedSprite(SPR_LOGO);
			pspr->setState("fadeout");
		}
	}
	else if (state == STAGE_FADEOUT)
	{
		CASprite* pspr = _getNamedSprite(SPR_LOGO);
		if (pspr->isAnimationDone())
		{
			_setState(STATE_WAITING);
		}
	}
	else if (state == STATE_WAITING)
	{
		float w = _settings.getFloat("wait");
		if (_getStateTime() > w)
		{
			CAWorld::sharedWorld()->switchStage(_pstageNext);
			_pstageNext->release();
			_pstageNext = null;
		}
	}
};

void BZStageLogoLayerMain::onExit()
{
#if defined(_PERCENT_LABEL)
	if (null != _pInfo)
	{
		_pInfo->release();
		_pInfo = null;
	}
#endif
	CAStageLayer::onExit();
}

#include "AWorld.h"
REG_CLASS(BZStageLogoLayerMain);
