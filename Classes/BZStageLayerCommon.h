#if !defined(_STAGELAYER_COMMON_H_)
#define _STAGELAYER_COMMON_H_

#include "AStageLayer.h"

#define SHOW_AsDialog	1
#define SHOW_AsNavigate	2

class BZStageLayerCommon :
	public CAStageLayer
{
protected:
	int _mode;
	string _from;
	string _navigateTo;

	virtual void _onIdle();
	virtual void _onClean();
	virtual void _onCreate();
	virtual void _onFadein();
	virtual void _onRunning();
	virtual void _onPause();
	virtual void _onResume();
	virtual void _onFadeout();
	virtual void _onButtonCommand(CASprite* pbutton) {};

public:
	BZStageLayerCommon(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStageLayerCommon(void);

	virtual bool checkCondition(CAState* from, const CATransition& trans);
	virtual void onStateBegin(CAState* from, void* param);
	//virtual void onStateUpdate(CAState* from, size_t counter, ccTime time);
	virtual void onStateEnd(CAState* from, void* param);

	virtual void onEvent(const CAEvent* pevt);

	void showDialog(const char* dialog, float zBegin = 5.0f);
	void replaceLayer(const char* layer);

	void show(int mode, const char* from, float zorder);
	void hide();
	void pause();
	void resume();
};

#endif //_STAGELAYER_COMMON_H_