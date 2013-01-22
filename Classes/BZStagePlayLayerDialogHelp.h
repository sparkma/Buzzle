#if !defined(_STAGE_PLAY_LAYER_HELP_H_)
#define _STAGE_PLAY_LAYER_HELP_H_

#include "BZStagePlayLayerDialog.h"

class BZStagePlayLayerDialogHelp :
	public BZStagePlayLayerDialog
{
protected:
	virtual void _onIdle();
	virtual void _onRunning();
	virtual void _onButtonCommand(CASprite* pbutton);

	CASprite* _psprs[5];
	
	//CAObjectList<BZTouchAction> _actions;
	CAGestureGenarator _gesture;
#define FS_NA		0
#define FS_Draging	1
#define FS_Flering	2
	float _fIndexBase;
	float _fIndex;
	float _fIndexTo;
	//void _doSlide();
	void _doSlideManual();
public:
	BZStagePlayLayerDialogHelp(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerDialogHelp(void);

	virtual void onUpdate();

	bool onEvent(const CAEvent* pevt);
};

#endif //_STAGE_PLAY_LAYER_HELP_H_