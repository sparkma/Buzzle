#if !defined(_TSSPRITE_BUTTON_ITEM_H_)
#define _TSSPRITE_BUTTON_ITEM_H_
#include "BZSpriteButton.h"
#include "BZSpriteCommon.h"
#include "BZGame.h"

typedef enum enumButtonItemState
{
	EBIS_Idle = 0,
	EBIS_Disabled,
	EBIS_WarnIdle,
	EBIS_Warning,
	EBIS_WarnWaiting,
	EBIS_HeartBeat,
	EBIS_HeartBeating,
}
EButtonItemState;

class BZSpriteButtonItem : public BZSpriteButton
{
protected:
	EButtonItemState _item_state;
	void _setState(EButtonItemState s);
	EButtonItemState _getState() const { return _item_state; };

	CCArray* _pieces;
	string _difficulty;
	int _level;
	int _limitLevel;
	int _pcount;
	float _timeHeartBeat;
	float _timeWarning;
	float _timeDelay;
	//EButtonItemState _state;
	float _fillRate;

	EGameState _gs;

	bool _enabled() { return _level >= _limitLevel; }
	void _onPieceCountChanged();
	void _resetNewPose();
	virtual void _onClick();
public:
	BZSpriteButtonItem(CAStageLayer* palyer = null, const char* name = null);
	virtual ~BZSpriteButtonItem(void);

	void setGameState(EGameState gs) { _gs = gs; };
	void setFillRate(float percent) { _fillRate = percent; }
	void setLimitLevel(int level);
	void onLevelChanged(int level);
	void setDifficulty(const string& diff);
	void addPiece(const string& name, const CCPoint& pos);

	virtual void onEnter();
	virtual void onUpdate();
};

#endif //_TSSPRITE_BUTTON_ITEM_H_
