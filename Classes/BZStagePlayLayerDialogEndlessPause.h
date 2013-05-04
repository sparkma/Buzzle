#if !defined(_STAGE_PLAY_LAYER_DIALOG_ENDLESS_PAUSE_H_)
#define _STAGE_PLAY_LAYER_DIALOG_ENDLESS_PAUSE_H_

#include "BZStagePlayLayerDialog.h"

class BZStagePlayLayerDialogEndlessPause :
	public BZStagePlayLayerDialog
{
protected:
	virtual void _onButtonCommand(CASprite* pbutton);
public:
	BZStagePlayLayerDialogEndlessPause(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerDialogEndlessPause(void);

	virtual void onUpdate();

	virtual void onStateBegin(CAState* from, void* param);
	bool onEvent(const CAEvent* pevt);
};

#endif //_STAGE_PLAY_LAYER_DIALOG_ENDLESS_PAUSE_H_