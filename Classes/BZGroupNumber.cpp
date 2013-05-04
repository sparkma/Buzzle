
#include "BZGroupNumber.h"
#include "AWorld.h"
#include "AStageLayer.h"
#include "BZSpriteCommon.h"

#define _FRAMES_SKIP  5

BZGroupNumber::BZGroupNumber(CAStageLayer* player, CASprite** psprs, int count, bool acc)
{
	_player = player;
	
	_Assert(count < SIZE_OF_ARRAY(_psprs));
	memset(_psprs, 0, sizeof(_psprs));
	int i;
	for (i = 0; i < count; i++)
	{
		_psprs[i] = acc ? psprs[count - 1 - i] : psprs[i];
	}
	//memcpy(_psprs, psprs, count * sizeof(CASprite*));
	_count = count;

	_mode = NCM_None;
	_order = NCO_All;
	//autorelease();
	_dirty = false;

	_state = GNS_Displaying;
}

BZGroupNumber::~BZGroupNumber(void)
{
	memset(_psprs, 0, sizeof(_psprs));
}

void BZGroupNumber::makeDisappearState(const char* state)
{
	_Assert(_state == GNS_Displaying);
	_state = GNS_Disappear;
}

void BZGroupNumber::setText(const char* pszText)
{
	_Assert(_state == GNS_Displaying);

	if (_text != pszText)
	{
		//render full once
		if (_dirty)
		{
			ENumberChangeOrder oldorder = _order;
			ENumberChangeMode oldmode = _mode;
			_order = NCO_All;
			_mode = NCM_None;
			int n = 0;
			while (_dirty)
			{
				n++;
				onUpdate(false);
			}
			_Assert(!_dirty && n < 3);
			_order = oldorder;
			_mode = oldmode;
		}
		//
		_text = pszText;

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

void BZGroupNumber::onUpdate(bool bDelay)
{
	if (_state == GNS_Disappear || !_dirty)
		return;

	_updateCounter++;
	
	if ((_updateCounter % _FRAMES_SKIP) != 0 && bDelay)
		return;

	int lennew = _text.length();
	bool bChanged = false;
	int i;
	for (i = lennew - 1; i >= 0; i--)
	{
		char chnew = _text[lennew - 1 - i];
		CASprite* pspr = (CASprite*)_psprs[i];
		if (null == pspr)
			continue;

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
		//pspr->setScl(_scale);
		pspr->setState(szPose);

		if (_order == NCO_Left)
		{
			if (bChanged)
			{
				break;
			}
		}
	}

	for (i = lennew ; i < _count; i++)
	{
		CASprite* pspr = (CASprite*)_psprs[i];
		if (null == pspr) continue;
		pspr->setVisible(false);
	}

	if (!bChanged)
	{
		_dirty = false;
	}
}

