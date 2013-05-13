
#include "BZStageLayerCommon.h"
#include "AStage.h"
#include "AString.h"
#include "AWorld.h"

BZStageLayerCommon::BZStageLayerCommon(CAStage* pstage, CAStageLayer* playerParent) : CAStageLayer(pstage, playerParent)
{
	_Info("%s allocated", __FUNCTION__);
	_navigateTo = "";
}

BZStageLayerCommon::~BZStageLayerCommon(void)
{
	_Info("%s destroyed", __FUNCTION__);
}

int BZStageLayerCommon::_findSprites(CASprite** psprs, const char* fmt, int from, int count)
{
	int i, n = 0;
	char name[128];
	for (i = from; i < from + count; i++)
	{
		sprintf(name, fmt, i);
		CASprite* pspr = this->_getNamedSprite(name, 0);
		if (pspr) n++;
		psprs[i - from] = pspr;
	}
	return n;
}

void BZStageLayerCommon::showDialog(const char* dialog, float zBegin)
{
	BZStageLayerCommon* playerFrom = this;
	playerFrom->pause();
	BZStageLayerCommon* pl = null;
	pl = (BZStageLayerCommon*)_pstage->getSubLayer(dialog);
	pl->show(SHOW_AsDialog, playerFrom->name().c_str(), zBegin);
}

void BZStageLayerCommon::replaceLayer(const char* layer)//, const string& state = "fadein", void* setingup_data = null)
{
	this->hide();
	_navigateTo = layer;
}

bool BZStageLayerCommon::checkCondition(CAState* from, const CATransition& trans)
{
	//static condition "true" "false" check
	if (CAMachine::checkCondition(from, trans))
		return true;

	bool result = false;
	float timeout = trans.timeout;
	if (timeout > 0 && this->getTimeNow() - from->getTimeEnter() > timeout)
	{
		result = true;
	}
	return result;
}

void BZStageLayerCommon::_onIdle()
{
	_Trace("onIdle:%s", this->name().c_str());
	this->removeAllTimelines();
}

void BZStageLayerCommon::_onClean()
{
	_Trace("onClean:%s", this->name().c_str());
	this->removeAllTimelines();

	if (_navigateTo.length() > 0)
	{
		BZStageLayerCommon* playerFrom = this;
		BZStageLayerCommon* pl = null;
		pl = (BZStageLayerCommon*)_pstage->getSubLayer(_navigateTo.c_str());
		pl->show(SHOW_AsNavigate, playerFrom->name().c_str(), 0.0f);
	}
}

void BZStageLayerCommon::_onCreate()
{
	_Trace("onCreate:%s", this->name().c_str());
	_pstage->setOffset(ccp(0, 0), 0.0f);
	this->activeAllTimelines();
}


void BZStageLayerCommon::_onFadein()
{
	_Trace("onFadein:%s", this->name().c_str());
	this->setState("fadein", null);
}


void BZStageLayerCommon::_onRunning()
{
	_pstage->resetPlaylist(_musics, _pcount);
	_pstage->playMusic(null);

	_Trace("onRunning:%s", this->name().c_str());
	this->setState("stand", null);
	_pstage->setFocus(this);
}


void BZStageLayerCommon::_onPause()
{
	_pstage->stopMusic();
	_Trace("onPause:%s", this->name().c_str());
}


void BZStageLayerCommon::_onResume()
{
	_Trace("onResume:%s", this->name().c_str());
	_pstage->setFocus(this);
}


void BZStageLayerCommon::_onFadeout()
{
	_Trace("onFadeout:%s", this->name().c_str());
	this->setState("fadeout", null);
}


void BZStageLayerCommon::onStateBegin(CAState* from, void* param) 
{
	GUARD_FUNCTION();

	const string& fname = from->getFullName();
	if (0) ;
	else if (fname == "root.idle")
	{
		_onIdle();
	}
	else if (fname == "root.clean")
	{
		_onClean();
	}
	else if (fname == "root.create")
	{
		_onCreate();
	}
	else if (fname == "root.fadein")
	{
		_onFadein();
	}
	else if (fname == "root.running")
	{
		_onRunning();
	}
	else if (fname == "root.pause")
	{
		_onPause();
	}
	else if (fname == "root.resume")
	{
		_onResume();
	}
	else if (fname == "root.fadeout")
	{
		_onFadeout();
	}
	else
	{
	}
};

void BZStageLayerCommon::onStateEnd(CAState* from, void* param) 
{
	//nothing to do here
};

bool BZStageLayerCommon::onEvent(const CAEvent* pevt)
{
	bool ret = CAStageLayer::onEvent(pevt);

	switch (pevt->type())
	{
	case ET_Touch:
		break;
	case ET_Command:
		{
			CAEventCommand* pec = (CAEventCommand*)pevt;
			if (pec->command() == "onClick")
			{
				_Assert(pec->getSenderType() == ST_Sprite);
				CASprite* pspr = (CASprite*)pec->sender();
				_onButtonCommand(pspr);
			}
		}
		break;
	}
	return ret;
}

void BZStageLayerCommon::show(int mode, const char* from, float zorder)
{
	_mode = mode;
	_from = from;

	this->setBaseZOrder(zorder);

	_Trace("show:%s", this->name().c_str());
	this->setConditionResult("root.idle@user.show", true);
}

void BZStageLayerCommon::hide()
{
	_Trace("hide:%s", this->name().c_str());
	string signal = this->getCurrentState()->getFullName() + "@user.hide";
	this->setConditionResult(signal.c_str(), true);
}

void BZStageLayerCommon::pause()
{
	_Trace("pause:%s", this->name().c_str());
	string signal = this->getCurrentState()->getFullName() + "@user.pause";
	this->setConditionResult(signal.c_str(), true);
}

void BZStageLayerCommon::resume()
{
	_Trace("resume:%s", this->name().c_str());
	this->setConditionResult("root.pause@user.resume", true);
}

#include "AWorld.h"
REG_CLASS(BZStageLayerCommon);

