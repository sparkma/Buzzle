#if !defined(_STAGE_PLAY_LAYER_MENU_ENDLESS_H_)
#define _STAGE_PLAY_LAYER_MENU_ENDLESS_H_

#include "BZStageLayerCommon.h"

class BZStagePlayLayerMenuEndless :
	public BZStageLayerCommon
{
protected:
	//string _buttonCommand;
	virtual void _onButtonCommand(CASprite* pbutton);
	void _goBack();
public:
	BZStagePlayLayerMenuEndless(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerMenuEndless(void);

	virtual void onUpdate();
	virtual bool onEvent(const CAEvent* pevt);
};

#endif //_STAGE_PLAY_LAYER_MENU_ENDLESS_H_