#if !defined(_TSSPRITE_BUTTON_ITEM_H_)
#define _TSSPRITE_BUTTON_ITEM_H_
#include "BZSpriteButton.h"

class BZSpriteButtonItem : public BZSpriteButton
{
protected:
public:
	BZSpriteButtonItem(CAStageLayer* palyer = null, const char* name = null);
	virtual ~BZSpriteButtonItem(void);
};

#endif //_TSSPRITE_BUTTON_ITEM_H_
