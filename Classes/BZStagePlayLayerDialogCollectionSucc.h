#if !defined(_STAGE_PLAY_LAYER_DIALOG_COLLECTION_SUCC_H_)
#define _STAGE_PLAY_LAYER_DIALOG_COLLECTION_SUCC_H_

#include "BZStagePlayLayerDialog.h"

class BZStagePlayLayerDialogCollectionSucc :
	public BZStagePlayLayerDialog
{
protected:
	virtual void _onIdle();
	virtual void _onRunning();
	virtual void _onButtonCommand(CASprite* pbutton);
public:
	BZStagePlayLayerDialogCollectionSucc(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerDialogCollectionSucc(void);

	virtual void onUpdate();

	void onEvent(const CAEvent* pevt);
};

#endif //_STAGE_PLAY_LAYER_DIALOG_COLLECTION_SUCC_H_