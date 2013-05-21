#if !defined(_STAGE_PLAY_LAYER_HOME_H_)
#define _STAGE_PLAY_LAYER_HOME_H_

#include "BZStageLayerCommon.h"

class BZStagePlayLayerHome :
	public BZStageLayerCommon
{
protected:
	//bool _bFirstRun;
	//string _difficulty;

	void _handleModeCommand(const char* mode, const char* diff);

	virtual void _onIdle();
	virtual void _onRunning();

	virtual void _onButtonCommand(CASprite* pbutton);
public:
	BZStagePlayLayerHome(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStagePlayLayerHome(void);

	virtual void onStateBegin(CAState* from, void* param);
	virtual bool onEvent(const CAEvent* pevt);
};

#endif //_STAGE_PLAY_LAYER_HOME_H_