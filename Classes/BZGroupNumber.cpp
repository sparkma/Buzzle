
#include "BZGroupNumber.h"
#include "AWorld.h"
#include "AStageLayer.h"
#include "BZSpriteCommon.h"

#define _FRAMES_SKIP  5

BZGroupNumber::BZGroupNumber(CAStageLayer* player, const string& spr)
{
	_player = player;
	_spr = spr;
	//_charmap = charmap;
	_numbers = CCArray::create();
	_numbers->retain();

	_mode = NCM_None;
	_order = NCO_All;
	//autorelease();
	_dirty = false;

	_state = GNS_Displaying;
}

BZGroupNumber::~BZGroupNumber(void)
{
	CCObject* pobj;
	CCARRAY_FOREACH(_numbers, pobj)
	{
		CASprite* pspr = (CASprite*)pobj;
		_player->removeSprite(pspr);
	}
	_numbers->release();
}

CASprite* BZGroupNumber::_createNumber()
{
	_Assert(_state == GNS_Displaying);

	BZSpriteCommon* pspr = new BZSpriteCommon(_player, _spr.c_str());
	pspr->setVisible(false);
	_player->addSprite(pspr);
	_numbers->insertObject(pspr, 0);
	return pspr;
}

void BZGroupNumber::makeDisappearState(const char* state)
{
	_Assert(_state == GNS_Displaying);
	_state = GNS_Disappear;

	CCObject* pobj;
	CCARRAY_FOREACH(_numbers, pobj)
	{
		BZSpriteCommon* pspr = (BZSpriteCommon*)pobj;
		pspr->setState(state);
		pspr->setDeadPose(state);
		_numbers->removeObject(pspr);
	}
	_Assert(_numbers->count() == 0);
}

void BZGroupNumber::setPos(const CCPoint& pos)
{
	_Assert(_state == GNS_Displaying);

	bool bNearPos = CAUtils::almostEqual(pos.x, _pos.x) && CAUtils::almostEqual(pos.y, _pos.y);
	if (!bNearPos)
	{
		_pos = pos;
		_dirty = true;
		_updateCounter = 0;
	}
}

void BZGroupNumber::setText(const char* pszText)
{
	_Assert(_state == GNS_Displaying);

	if (_text != pszText)
	{
		_text = pszText;
		while (_numbers->count() < _text.length())
		{
			_createNumber();
		}
		_dirty = true;
		_updateCounter = 0;
	}
}

static int _goNear(int cur, int to, int range, int step)
{
	if (cur == to) return cur;
	if ((cur - to < 0 && to - cur > (range >> 1)) ||
		(cur - to > 0 && cur - to < (range >> 1)))
	{
		cur += range - step;
	}
	else
	{
		cur += step;
	}
	cur %= range;

	return cur;
}

void BZGroupNumber::onUpdate()
{
	if (_state == GNS_Disappear || !_dirty)
		return;

	_updateCounter++;
	if ((_updateCounter % _FRAMES_SKIP) != 0)
		return;

	//int lenold = _textDisplaying.size();
	int lennew = _text.size();
	int len = _numbers->count();
	_Assert(lennew <= len);

	CCPoint posBegin;
	if (_align < 0) //left
	{
		posBegin.x = _pos.x + _size.width * (float)lennew;
		posBegin.y = _pos.y + _size.height * (float)lennew;
	}
	else if (_align == 0) //middle
	{
		posBegin.x = _pos.x + _size.width * (float)lennew / 2.0f;
		posBegin.y = _pos.y + _size.height * (float)lennew / 2.0f;
	}
	else  //right
	{
		posBegin.x = _pos.x; // - _size.width * (float)lennew;
		posBegin.y = _pos.y; // - _size.height * (float)lennew;
	}

	bool bChanged = false;
	int i;
	for (i = lennew - 1; i >= 0; i--)
	{
		char chnew = _text[i];
		CASprite* pspr = (CASprite*)_numbers->objectAtIndex(i);
		const string& state = pspr->getState();
		char chold = state.length() > 0 ? state[0] : '#';

		char szPose[4];
		if (chold == chnew)
		{
		}
		else
		{
			if (_mode == NCM_None)
			{
				bChanged = true;
			}
			else //NCM_Near
			{
				if (chnew >= '0' && chnew <= '9' && chold >= '0' && chold <= '9')
				{
					char chcur = _goNear(chold - '0', chnew - '0', 10, 1);
					chnew = chcur + '0';
					bChanged = true;
				}
				else
				{
					bChanged = true;
				}
			}
		}

		szPose[0] = chnew;
		szPose[1] = 0;

		pspr->setVisible(true);
		pspr->setPos(posBegin);
		pspr->setScl(_scale);
		posBegin.x -= _size.width;
		posBegin.y -= _size.height;
		pspr->setState(szPose);

		if (_order == NCO_Left)
		{
			if (bChanged)
			{
				break;
			}
		}
	}

	for (i = lennew ; i < len; i++)
	{
		CASprite* pspr = (CASprite*)_numbers->objectAtIndex(i);
		pspr->setVisible(false);
	}

	if (!bChanged)
	{
		_dirty = false;
	}
}

