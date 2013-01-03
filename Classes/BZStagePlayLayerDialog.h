#if !defined(_STAGE_PLAY_LAYER_DIALOG_H_)
#define _STAGE_PLAY_LAYER_DIALOG_H_

#include "BZStageLayerCommon.h"

class BZStagePlayLayerDialog :
	public BZStageLayerCommon
{
protected:
	string _backTo;
	string _result;

	void _setResult(const string& r) { _result = r; }
	virtual void _onClean();
public:
	BZStagePlayLayerDialog(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerDialog(void);

	const string& getResult() const { return _result; }

	void doBack(const char* backTo);
};

#endif //_STAGE_PLAY_LAYER_DIALOG_H_