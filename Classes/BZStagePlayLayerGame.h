#if !defined(_STAGE_PLAY_LAYER_GAME_H_)
#define _STAGE_PLAY_LAYER_GAME_H_

#include "AStageLayer.h"

#define _DEBUG_INFO

class BZSpritePlayer;
class BZSpriteCommon;
class BZSpriteButton;

class BZStagePlayLayerGame :
	public CAStageLayer //, public CCTargetedTouchDelegate
{
protected:
#if defined(_DEBUG_INFO)
	CCLabelTTF*		_pInfo;
#endif
	void _updateNumber(const char* prefix, int nValue);

public:
	BZStagePlayLayerGame(CAStage* pstage, CAStageLayer* playerParent);
	virtual ~BZStagePlayLayerGame(void);

	virtual string debuglog();

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();

	virtual bool onEvent(const CAEvent* pevt);

	virtual bool checkCondition(CAState* from, const CATransition& trans);
	virtual void onStateBegin(CAState* from, void* param);
	//virtual void onStateUpdate(CAState* from, size_t counter, ccTime time);
	virtual void onStateEnd(CAState* from, void* param);
};

#endif //_STAGE_PLAY_LAYER_GAME_H_
