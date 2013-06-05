
#include "BZSpriteButtonItem.h"
#include "AStageLayer.h"
#include "AWorld.h"
#include "AUserData.h"

#define NAME_PP "flying__pieces"

#define FILL_RATE_LIMITE_HIGH (0.72f)
#define FILL_RATE_LIMITE_LOW (0.60f)

#define TIME_WARNING	 (5.0f)
#define TIME_HEART_BEAT_INTERVAL	(9999999999.0f)
#define TIME_HEARTBEAT   (0.3f)

#define _FOR_SCREENSHOT_

BZSpriteButtonItem::BZSpriteButtonItem(CAStageLayer* player, const char* name) : BZSpriteButton(player, name)
{
	_pieces = CCArray::create();
	_pieces->retain();
	_pcount = 0;
	_limitLevel = 0;
	_level = 0;
	_fillRate = 0.0f;
	_timeWarning = 0;
	_timeHeartBeat =0; // player->getTimeNow();
	_setState(EBIS_Idle);
	//_onPieceCountChanged();
}

BZSpriteButtonItem::~BZSpriteButtonItem(void)
{
	if (_pieces)
	{
		_pieces->release();
		_pieces = null;
	}
}

void BZSpriteButtonItem::setDifficulty(const string& diff)
{
	_difficulty = diff;
	string key = this->getModName() + diff;
	_pcount = CAUserData::sharedUserData().getInteger(key.c_str(), 0);
#if defined(_FOR_SCREENSHOT_)
	_pcount = 5;
#endif
	_onPieceCountChanged();
}

void BZSpriteButtonItem::_onPieceCountChanged()
{
#if defined(_DEBUG)
#else
	this->enable(_pcount == 5);
#endif

	string key = this->getModName() + _difficulty;
	_Assert(_difficulty.length());
	_Assert(key.length());
	{
		CAUserData::sharedUserData().setInteger(key.c_str(), _pcount);
	}

	_resetNewPose();
}

void BZSpriteButtonItem::setLimitLevel(int level)
{ 
	_limitLevel = level; 
#if defined(_FOR_SCREENSHOT_)
	_limitLevel = 0; 
#endif
	//force rejudge button states
	onLevelChanged(_level); 
};

void BZSpriteButtonItem::onLevelChanged(int level)
{
	_level = level; 
	if (level < _limitLevel)
	{
		string key = this->getModName() + _difficulty;
		CAUserData::sharedUserData().setInteger(key.c_str(), 0);

		_setState(EBIS_Disabled);
		setState("6d");
		_Info("%s item state to 6d", this->getModName().c_str());
	}
	else
	{
		_fillRate = 0; //reset fill rate
		_setState(EBIS_Idle);
		_resetNewPose();
	}
};

void BZSpriteButtonItem::_resetNewPose()
{
	char sz[16];
	
	int c = _pcount;
	if (c < 0) c = 0;
	else if (c > 5) c = 5;

	sprintf(sz, "6%d", c);
	_Info("%s item reset pose to %s", this->getModName().c_str(), sz);
	setState(sz);
}

void BZSpriteButtonItem::_onClick()
{
	//for screenshot, remove this two condition
	if (_pcount < 5) return;
	if (!_enabled()) return;

	bool acc = false;
	switch (_gs)
	{
	case GS_Idle:
		break;

	case GS_Running:
		acc = true;
		break;
	case GS_LevelUpPrepare:
	case GS_LevelUpWaiting:
	case GS_LevelUpPaused:
	case GS_LevelUpResume:
	case GS_LevelUped:

	case GS_ItemChangeColor:
	case GS_ItemSameColorBooom:
	case GS_ItemBooom:
	
	case GS_SpecEffecting:

	case GS_Leaving:
		break;
	}
	if (!acc)
		return;
	BZSpriteButton::_onClick();
	_pcount = 0;
#if defined(_FOR_SCREENSHOT_)
	_pcount = 5;
#endif
	_setState(EBIS_Idle);
	_onPieceCountChanged();
}

void BZSpriteButtonItem::addPiece(const string& name, const CCPoint& pos)
{
	if (!_enabled()) return;

	CAStageLayer* pl = this->layer();
	CASprite* pspr = CAWorld::sharedWorld()->createSprite(pl, name.c_str());
	pspr->setGroupName(NAME_PP);
	pspr->setVertexZ(this->getVertexZ() + 10.0f);
	pspr->setPos(pos);
	pspr->setState("fly");
	pl->addSprite(pspr);
	_pieces->addObject(pspr);
}

void BZSpriteButtonItem::_setState(EButtonItemState s)
{ 
	if (EBIS_Disabled == _item_state)
	{
		if (_level >= _limitLevel)
		{
			_item_state = s;
		}
	}
	else
	{
		_item_state = s; 
	}
	_Info("%s item change state to %d", this->getModName().c_str(), _item_state);
}

void BZSpriteButtonItem::onEnter()
{
	BZSpriteButton::onEnter();
}

void BZSpriteButtonItem::onUpdate()
{
	BZSpriteButton::onUpdate();

	switch (_getState())
	{
	case EBIS_Idle:
		if (_pcount >= 5)
		{
			if (_fillRate > FILL_RATE_LIMITE_HIGH)
			{
				_setState(EBIS_Warning);
			}
			else
			{
				float diff = this->layer()->getTimeNow() - _timeHeartBeat;
				if (diff > TIME_HEART_BEAT_INTERVAL)
				{
					_setState(EBIS_HeartBeat);
				}
			}
		}
		break;
	case EBIS_WarnIdle:
		_Assert(_pcount >= 5);
		if (_fillRate < FILL_RATE_LIMITE_LOW)
		{
			_setState(EBIS_Idle);
		}
		break;
	case EBIS_Warning:
		{
			_Assert(_pcount >= 5);
			_timeWarning = this->layer()->getTimeNow();
			_timeDelay = TIME_WARNING;
			setState("66");
			_Info("%s item warning to %s", this->getModName().c_str(), "66");
			_setState(EBIS_WarnWaiting);
		}
	case EBIS_WarnWaiting:
		{
			_Assert(_pcount >= 5);
			float diff = this->layer()->getTimeNow() - _timeWarning;
			if (diff > _timeDelay || _fillRate < FILL_RATE_LIMITE_LOW)
			{
				_resetNewPose();
				_setState(EBIS_WarnIdle);
			}
		}
		break;
	case EBIS_HeartBeat:
		{
			_Assert(_pcount >= 5);
			_timeHeartBeat = this->layer()->getTimeNow();
			_timeDelay = TIME_HEARTBEAT;
			setState("66");
			_Info("%s item heart to %s", this->getModName().c_str(), "66");
			_setState(EBIS_HeartBeating);
		}
		break;
	case EBIS_HeartBeating:
		{
			_Assert(_pcount >= 5);
			float diff = this->layer()->getTimeNow() - _timeHeartBeat;
			if (diff > _timeDelay)
			{
				_resetNewPose();
				_setState(EBIS_Idle);
			}
		}
		break;
	}

	int n = 0;
	CASprite* psprsRemoved[256];
	CAObject* pobj;
	CCARRAY_FOREACH(_pieces, pobj)
	{
		CASprite* pspr = (CASprite*)pobj;
		float p0 = pspr->getAnimationPercent(this->layer()->getTimeNow());
		float p = p0 + 0.1f;
		if (p > 1.0f) p = 1.0f;

		CCPoint pos;
		CCPoint posBegin = pspr->getPos();
		CCPoint posTo = getPos();
		pos.x = posBegin.x + (posTo.x - posBegin.x) * p;
		pos.y = posBegin.y + (posTo.y - posBegin.y) * p;

		_Info("piece(%.2f) %.2f,%.2f", p0, pos.x, pos.y);

		pspr->setPos(pos);
		if (pspr->isAnimationDone())
		{
			if (_pcount < 5) 
			{
				_pcount++;
			}
			_onPieceCountChanged();

			pspr->setPos(posTo);
			pspr->killMyself();
			psprsRemoved[n++] = pspr;
		}
	}
	while (n > 0)
	{
		CASprite* pspr = psprsRemoved[n - 1];
		_pieces->removeObject(pspr);
		n--;
	}
}


#include "AWorld.h"
REG_CLASS(BZSpriteButtonItem);
