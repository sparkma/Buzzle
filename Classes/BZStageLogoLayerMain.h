#if !defined(_STAGE_LOGO_LAYER_LOGO_H_)
#define _STAGE_LOGO_LAYER_LOGO_H_

#include "AStageLayer.h"

class BZStageLogoLayerMain:
	public CAStageLayer
{
protected:
	string _state;
	ccTime _timeState;
	void _setState(const string& s);
	ccTime _getStateTime();
	CAStage* _pstageNext;
#if defined(_PERCENT_LABEL)
	CCLabelTTF* _pInfo;
#endif
public:
	BZStageLogoLayerMain(CAStage* pstage = null, CAStageLayer* playerParent = null);
	virtual ~BZStageLogoLayerMain(void);

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();
};

#endif //_STAGE_LOGO_LAYER_LOGO_H_