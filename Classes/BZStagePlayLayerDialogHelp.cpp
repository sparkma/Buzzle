
#include "BZStagePlayLayerDialogHelp.h"
#include "BZStageCommon.h"

#include "AWorld.h"
#include "AString.h"

BZStagePlayLayerDialogHelp::BZStagePlayLayerDialogHelp(CAStage* pstage, CAStageLayer* playerParent) : BZStagePlayLayerDialog(pstage, playerParent)
{
	_Trace("%s allocated", __FUNCTION__);
	_fIndexTo = _fIndex = 0;
	memset(_psprs, 0, sizeof(_psprs));
}

BZStagePlayLayerDialogHelp::~BZStagePlayLayerDialogHelp(void)
{
	_Trace("%s destroyed", __FUNCTION__);
}

void BZStagePlayLayerDialogHelp::_onIdle()
{
	BZStageLayerCommon::_onIdle();
	memset(_psprs, 0, sizeof(_psprs));

	_gesture.setTimeProvider(_pstage);
	_gesture.addEventListener(this);
	_gesture.setAcc(0.93f);
}

void BZStagePlayLayerDialogHelp::_onRunning()
{
	BZStageLayerCommon::_onRunning();
	_fIndexTo = _fIndex = 0;

	int i;
	for (i = 0; i < 5; i++)
	{
		char szName[32];
		sprintf(szName, "help_p%d", i + 1);
		_psprs[i] = this->_getNamedSprite(szName);
		_Assert(null != _psprs[i]);
		_psprs[i]->addEventListener(this);
	}
}

/*
void BZStagePlayLayerDialogHelp::_doSlide()
{
	if (_fIndexTo == _fIndex)
	{
		return;
	}

	int i;
	for (i = 0; i < 5; i++)
	{
		if (!_psprs[i]->isAnimationDone())
		{
			//wait if animating
			return;
		}
	}

	for (i = 0; i < 5; i++)
	{
		_psprs[i]->setState("stand");
		_psprs[i]->setPos(100000.0f, 0);
	}

	string state;
	CCPoint pos;

	int a = _fIndex;
	int b;
	pos = _pstage->getPointFromPercent(ccp(0.5f, 0.5f), false);
	_psprs[_fIndex]->setPos(pos);
	if (_fIndexTo > _fIndex)
	{
		_fIndex++;
		b = _fIndex;
		state = "sleft";
		pos = _pstage->getPointFromPercent(ccp(1.5f, 0.5f), false);
		_psprs[_fIndex]->setPos(pos);
	}
	else
	{
		_fIndex--;
		b = _fIndex;
		state = "sright";
		pos = _pstage->getPointFromPercent(ccp(-0.5f, 0.5f), false);
		_psprs[_fIndex]->setPos(pos);
	}
	_psprs[a]->setState(state);
	_psprs[b]->setState(state);
	_Info("->%d, %d, %d", _fIndexTo, a, b);
	//_fIndex = _fIndexTo;
}

void BZStagePlayLayerDialogHelp::_doSlideManual()
{
	float indexold = _fIndex;
	switch (_dragingState)
	{
	case FS_NA:
		{
			if (_ta._dragState != DS_NA)
			{
				_timeReadBegin = _pstage->getTimeNow();
				_fIndexBase = _fIndex;
				_dragingState = FS_Draging;
				_Info("slide begin");
			}
		}
		break;
	case FS_Draging:
		{
			_Info("draging");
			float dtime = _pstage->getTimeNow() - _timeReadBegin;

			float sx = 1.0f;
			CAWorld::percent2view(sx, true);

			CCPoint posDelta;
			if (!_pa->getpos(posDelta, dtime))
				break;
			posDelta.x -= _pa->ptBase.x;
			posDelta.y -= _pa->ptBase.y;

			float percent = posDelta.x / sx;
			if (_Abs(percent) > 0.15f)
			{
				_fIndexTo = _fIndexBase + percent / _Abs(percent);
				//remove this action
				_actions.removeBegin();
				//enter flering state
				_dragingState = FS_Flering;
			}
			else 
			{
				_fIndex = _fIndexBase + percent;
			}
		}
		break;
	case FS_Flering:
		{
			_Info("flering");
			float delta = _fIndexTo - _fIndex;
			delta /= 3.0f;
			if (delta < 0.1f)
			{
				_fIndex = _fIndexTo;
				_dragingState = FS_NA;
			}
			else
			{
				_fIndex += delta;
			}
		}
		break;
	}

	if (indexold != _fIndex)
	{
		int i;
		for (i = 0; i < 5; i++)
		{
			CCPoint pos = _psprs[i]->getPos();
			pos.x = CAWorld::getScreenSize().width * (0.5f + (float)i - _fIndex);
			_psprs[i]->setPos(pos);
		}
	}
}
*/

void BZStagePlayLayerDialogHelp::onUpdate()
{
	BZStageLayerCommon::onUpdate();
	if (CAMachine::getCurrentState()->getFullName() == "root.running")
	{
		_gesture.update();
		//_doSlideManual();
	}
}

void BZStagePlayLayerDialogHelp::_onButtonCommand(CASprite* pbutton)
{
	BZStageCommon* pstage = (BZStageCommon*)this->stage();
	string btn = pbutton->getModName();
	if ("mi_back" == btn)
	{
		doBack(_from.c_str());
	}
}

void BZStagePlayLayerDialogHelp::onEvent(const CAEvent* pevt)
{
	BZStageLayerCommon::onEvent(pevt);

	switch (pevt->type())
	{
	case ET_Touch:
		if (null != _psprs[0])
		{
			_gesture.onTouch((CAEventTouch*)pevt);
		}
		break;
	case ET_Command:
		/*
		{
			CAEventCommand* pc = (CAEventCommand*)pevt;
			if (pc->getSenderType() == ST_Sprite)
			{
				CASprite* p = (CASprite*)pc->sender();
				if (pc->command() == "animStop")
				{
					if (CAString::startWith(p->getModName(), "help_p"))
					{
						const CASpriteModelPoseModifierKey& k = p->getCombinedKey();
						p->setPos(k.x, k.y);
					}
				}
			}
		}
		*/
		break;
	case ET_Gesture:
		{
			CAEventGesture* pges = (CAEventGesture*)pevt;
			const string& command = pges->command();
			const CATimedTouchPoint* point = pges->point();
			/*
			_Info("GES Event: %16s dt:%.2f pt(%.2f,%.2f) dpt(%.2f,%.2f) v(%.2f,%.2f)", 
				command.c_str(), 
				point->dtime,
				point->pt.x,
				point->pt.y,
				point->dpt.x,
				point->dpt.y,
				point->xv, point->yv);
			*/
			const CATimedTouchPoint& tp = *point;
			_Info("UIUI time=%.4f pt=(%.2f,%.2f) dpt=(%.2f,%.2f) v(%.2f,%.2f) k:%.2f", 
				//tp.time, 
				_pstage->getTimeNow(),
				tp.pt.x, tp.pt.y, tp.dpt.x, tp.dpt.y, tp.xv, tp.yv, tp.k);
			int i;
			for (i = 0; i < 5; i++)
			{
				CCPoint pos = _psprs[i]->getPos();
				//pos.x = CAWorld::getScreenSize().width * (0.5f + (float)i - _fIndex);
				pos.x += point->dpt.x;
				//pos. += point->dpt.x;
				_psprs[i]->setPos(pos);
			}
		}
		break;
	}
}

#include "AWorld.h"
REG_CLASS(BZStagePlayLayerDialogHelp);
