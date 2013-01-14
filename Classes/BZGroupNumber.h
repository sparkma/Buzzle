#if !defined(_TSGROUPED_NUMBER_H_)
#define _TSGROUPED_NUMBER_H_

#include "AObject.h"
class CASprite;
class CAStageLayer;
class BZSpriteCommon;

typedef enum enumNumberChangeMode
{
	NCM_None,	//Directly
	NCM_Near,	//goes to near
}
ENumberChangeMode;

typedef enum enumNumberChangeOrder
{
	NCO_All,	//
	NCO_Left,	//left ==> right
}
ENumberChangeOrder;

typedef enum enumGroupNumberState
{
	GNS_Displaying,
	GNS_Disappear,
}
EGroupNumberState;

class BZGroupNumber : public CAObject
{
protected:
	CAStageLayer*	_player;

	int				_count;
	CASprite*		_psprs[32];
	
	int _state;

	string _text;

	ENumberChangeMode _mode;
	ENumberChangeOrder _order;

	int _updateCounter;
	bool _dirty;
public:
	BZGroupNumber(CAStageLayer* player, CASprite** psprs, int count);
	virtual ~BZGroupNumber(void);

	void setText(const char* pszText);

	void makeDisappearState(const char* state);
	void setChangeType(ENumberChangeMode mode = NCM_None, ENumberChangeOrder order = NCO_All) 
	{ _mode = mode; _order = order; }

	virtual void onUpdate(bool bDelay = true);
};

#endif //_TSGROUPED_NUMBER_H_
