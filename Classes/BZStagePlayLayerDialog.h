#if !defined(_STAGE_PLAY_LAYER_DIALOG_H_)
#define _STAGE_PLAY_LAYER_DIALOG_H_

#include "BZStageLayerCommon.h"

class BZStagePlayLayerDialog :
	public BZStageLayerCommon
{
protected:
	string _backTo;
	string _result;

	virtual void _onClean();
public:
	BZStagePlayLayerDialog(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerDialog(void);

	void doBack(const char* backTo, const char* result);
};

#endif //_STAGE_PLAY_LAYER_DIALOG_H_