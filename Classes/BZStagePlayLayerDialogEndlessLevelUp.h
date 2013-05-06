#if !defined(_STAGE_PLAY_LAYER_DIALOG_ENDLESS_LEVELUP_H_)
#define _STAGE_PLAY_LAYER_DIALOG_ENDLESS_LEVELUP_H_

#include "BZStagePlayLayerDialog.h"

class BZStagePlayLayerDialogEndlessLevelUp :
	public BZStagePlayLayerDialog
{
protected:
	virtual void _onButtonCommand(CASprite* pbutton);
public:
	BZStagePlayLayerDialogEndlessLevelUp(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerDialogEndlessLevelUp(void);

	virtual void onUpdate();

	virtual void onStateBegin(CAState* from, void* param);
	bool onEvent(const CAEvent* pevt);
};

#endif //_STAGE_PLAY_LAYER_DIALOG_ENDLESS_LEVELUP_H_