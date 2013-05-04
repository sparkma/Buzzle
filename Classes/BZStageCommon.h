#if !defined(_TSSTAGE_LOGO_H_)
#define _TSSTAGE_LOGO_H_

#include "AStage.h"
class BZStageLayerCommon;

class BZStageCommon : public CAStage
{
protected:
	bool _debug_info;
	int _counter;
	CCLabelTTF* _pInfo;
public:
	BZStageCommon(const char* name = null);
	virtual ~BZStageCommon(void);

	virtual string status();

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();
	/*
	void move2Layer(BZStageLayerCommon* playerFrom, const char* layerTo);
	void back2Layer(BZStageLayerCommon* playerFrom, const char* layerTo);
	void pushLayer(BZStageLayerCommon* playerFrom, const char* layerTo);
	void popLayer(BZStageLayerCommon* playerFrom, const char* layerTo);
	*/
};

#endif //_TSSTAGE_LOGO_H_