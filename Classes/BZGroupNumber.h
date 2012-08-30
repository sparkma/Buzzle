#if !defined(_TSGROUP_NUMBER_H_)
#define _TSGROUP_NUMBER_H_

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

class BZGroupNumber
{
protected:
	CAStageLayer*	_player;
	string			_spr;

	CCArray*		_numbers;
	
	int _state;

	string _text;
	int _align;
	CCPoint _pos;
	CCSize _size;
	float _scale;

	ENumberChangeMode _mode;
	ENumberChangeOrder _order;

	int _updateCounter;
	bool _dirty;
	CASprite* _createNumber();
public:
	BZGroupNumber(CAStageLayer* player, const string& spr);
	virtual ~BZGroupNumber(void);

	void setText(const char* pszText, const CCPoint& pos);
	void makeDisappearState(const char* state);
	void setChangeType(ENumberChangeMode mode = NCM_None, ENumberChangeOrder order = NCO_All) 
	{ _mode = mode; _order = order; }
	void setLayout(int align, const CCSize& sizeDelta, float scale = 1.0f)
	{ _align = align; _size = sizeDelta; _scale = scale; };

	virtual void onUpdate();
};

#endif //_TSGROUP_NUMBER_H_
