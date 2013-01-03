#if !defined(_STAGE_PLAY_LAYER_DIALOG_ENDLESS_OVER_H_)
#define _STAGE_PLAY_LAYER_DIALOG_ENDLESS_OVER_H_

#include "BZStagePlayLayerDialog.h"

class BZStagePlayLayerDialogEndlessGameover :
	public BZStagePlayLayerDialog
{
protected:
	virtual void _onIdle();
	virtual void _onRunning();
	virtual void _onButtonCommand(CASprite* pbutton);
public:
	BZStagePlayLayerDialogEndlessGameover(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerDialogEndlessGameover(void);

	virtual void onUpdate();

	void onEvent(const CAEvent* pevt);
};

#endif //_STAGE_PLAY_LAYER_DIALOG_ENDLESS_OVER_H_