#if !defined(_STAGE_TEST_LAYER_TEST_BUBBLES_H_)
#define _STAGE_TEST_LAYER_TEST_BUBBLES_H_

#include "AStageLayer.h"

class BZSpritePlayer;

class BZStageTestLayerTestBubbles :
	public CAStageLayer
{
protected:
	CCPoint _off;
	CASprite* _sea[8];
	CASprite* _wave1[2];
	CASprite* _wave2[2];
	CASprite* _wave3[2];
	CASprite* _sand1[2];

	CASprite*		_pTarget;
#if defined(_USE_MEM_GUARDER_)
	CCLabelTTF*		_pInfo;
#endif
	//CAArray*		_sprites;
public:
	BZStageTestLayerTestBubbles(CAStage* pstage, CAStageLayer* playerParent);
	virtual ~BZStageTestLayerTestBubbles(void);

	virtual void onEnter();
	virtual void onUpdate();
	virtual void onExit();
};

#endif //_STAGE_TEST_LAYER_TEST_BUBBLES_H_