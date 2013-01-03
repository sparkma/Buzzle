#if !defined(_STAGE_PLAY_LAYER_DIALOG_COLLECTION_PAUSE_H_)
#define _STAGE_PLAY_LAYER_DIALOG_COLLECTION_PAUSE_H_

#include "BZStagePlayLayerDialog.h"

class BZStagePlayLayerDialogCollectionPause :
	public BZStagePlayLayerDialog
{
protected:
	virtual void _onIdle();
	virtual void _onRunning();
	virtual void _onButtonCommand(CASprite* pbutton);
public:
	BZStagePlayLayerDialogCollectionPause(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerDialogCollectionPause(void);

	virtual void onUpdate();

	void onEvent(const CAEvent* pevt);
};

#endif //_STAGE_PLAY_LAYER_DIALOG_COLLECTION_PAUSE_H_