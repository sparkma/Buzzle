#if !defined(_STAGE_PLAY_LAYER_DIALOG_COLLECTION_FAILED_H_)
#define _STAGE_PLAY_LAYER_DIALOG_COLLECTION_FAILED_H_

#include "BZStagePlayLayerDialog.h"

class BZStagePlayLayerDialogCollectionFailed :
	public BZStagePlayLayerDialog
{
protected:
	virtual void _onIdle();
	virtual void _onRunning();
	virtual void _onButtonCommand(CASprite* pbutton);
public:
	BZStagePlayLayerDialogCollectionFailed(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerDialogCollectionFailed(void);

	virtual void onUpdate();

	bool onEvent(const CAEvent* pevt);
};

#endif //_STAGE_PLAY_LAYER_DIALOG_COLLECTION_FAILED_H_