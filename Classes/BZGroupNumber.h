#if !defined(_TSGROUP_NUMBER_H_)
#define _TSGROUP_NUMBER_H_

#include "AObject.h"
class CASprite;
class CAStageLayer;
class BZSpriteCommon;

#define MAX_NUMBER_COUNT	16
class BZGroupNumber
{
protected:
	CAStageLayer* _player;

	CASprite* _sprs[MAX_NUMBER_COUNT];

	//string _model;
	string _charmap;

	int _len;
	//int _lennow;
	//int _numberDisplayed;
	//int _number;
	//string _textDisplying;
	string _text;
	bool _dirty;
public:
	BZGroupNumber();
	virtual ~BZGroupNumber(void);

	//void init(CAStageLayer* player, const char* pszModel, int len = 0);
	void init(CAStageLayer* player, CASprite** psprNumber, int count, const char* pszCharMap);

	//void setLayout(const CCPoint& pos, const CCSize& size);
	//void setNumber(int number);
	void setText(const char* pszText);
	void setState(const char* ps);
	void setLeadingPos(const CCPoint& pos);

	virtual void onUpdate();
};

#endif //_TSGROUP_NUMBER_H_
