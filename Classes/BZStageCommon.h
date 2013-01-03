#if !defined(_TSSTAGE_LOGO_H_)
#define _TSSTAGE_LOGO_H_

#include "AStage.h"
class BZStageLayerCommon;

class BZStageCommon :
	public CAStage
{
public:
	BZStageCommon(const char* name = null);
	virtual ~BZStageCommon(void);

	/*
	void move2Layer(BZStageLayerCommon* playerFrom, const char* layerTo);
	void back2Layer(BZStageLayerCommon* playerFrom, const char* layerTo);
	void pushLayer(BZStageLayerCommon* playerFrom, const char* layerTo);
	void popLayer(BZStageLayerCommon* playerFrom, const char* layerTo);
	*/
};

#endif //_TSSTAGE_LOGO_H_