#if !defined(_STAGE_TEST_LAYER_TEST_BUBBLES_H_)
#define _STAGE_TEST_LAYER_TEST_BUBBLES_H_

#include "AStageLayer.h"

class BZSpritePlayer;

class BZStageTestLayerTestBubbles :
	public CAStageLayer
{
protected:
	void _removeAllSprites();
	void _createSprite(const CCPoint& pos);
public:
	BZStageTestLayerTestBubbles(CAStage* pstage, CAStageLayer* playerParent);
	virtual ~BZStageTestLayerTestBubbles(void);

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onEvent(CAEvent* pevt);
	virtual void onExit();
};

#endif //_STAGE_TEST_LAYER_TEST_BUBBLES_H_