#if !defined(_STAGE_PLAY_LAYER_MENU_COLLECTION_H_)
#define _STAGE_PLAY_LAYER_MENU_COLLECTION_H_

#include "BZStageLayerCommon.h"

class BZStagePlayLayerMenuCollection :
	public BZStageLayerCommon
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
	void _goBack();
public:
	BZStagePlayLayerMenuCollection(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerMenuCollection(void);

	virtual void onUpdate();

	void onEvent(const CAEvent* pevt);
};

#endif //_STAGE_PLAY_LAYER_MENU_COLLECTION_H_