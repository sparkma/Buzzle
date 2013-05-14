#if !defined(_BZGAME_MODE_H_)
#define _BZGAME_MODE_H_

#include "AObject.h"
#include "BZBoard.h"

#define Z_DYNAMIC_SCORE 20

#define BLOCK_TYPES		5
#define MAX_LEVELS		128

class BZConsts
{
public:
	static const char* SFX_Lighting;
};

//	level		delay		range		star delay		
//	======		=======		=======		===========		
//	1			3			3			5
//	128			0.5			8			6
// this should be read from xml config file
class BZLevelParams
{
public:
	float	fDelayOneRow;
	float	timeDelayBorn;
	//available stars for open-blocks
	int		nAvailableStars;
	int		nRangeBubbleBorn;
	float	fPrebornLines;
};

typedef enum enumGameState
{
	GS_Idle = 0,

	GS_Running = 1000,

	GS_LevelUpPrepare = 2000,
	GS_LevelUpWaiting,
	GS_LevelUpPaused,
	GS_LevelUpResume,
	GS_LevelUped,

	GS_ItemChangeColor  = 3000,
	GS_ItemSameColorBooom,
	GS_ItemBooom,
	
	GS_SpecEffecting = 4000,

	GS_GameOver = 5000,
	GS_GamePaused = 6000,

	GS_Leaving = 10000,
}
EGameState;

class BZGame : public BZBoard
{
protected:
	string			_name;

	int				_nCounter;

	//BZBoard*		_pboard;
	EGameState		_gamestate;
	int				_nGameStatesCount;
	EGameState		_oldGameStates[512];
	//EGameState		_stateWhenPaused;

	string			_types[BLOCK_TYPES];
	int				_typesCount;

	//int				_nStarsUsed;
	BZLevelParams	_params;
	float			_timeLastBorn;
	string			_lastBubble;

	int _nScore;

	virtual void _doBornStrategy() {};
	virtual void _onScoreChanged() {};
	virtual void _onBubbleBoomedChanged() {}
	virtual void _onBlockBoomedChanged() {};
	virtual void _onComboChanged() {};
	virtual void _onLevelChanged(bool first) {};
	virtual bool _canBoom(BZBlock* pblock) const { return false; };

	virtual BZBubble* _onUpdateBlock(BZBlock* pblock);

	virtual void _onDetachBubbleSprite(BZBubble* pbubble);

	int _indexOfType(const char* type) const;
public:
	BZGame(CAStageLayer* player);
	virtual ~BZGame();

	//void loadData();
	//void saveData();

	virtual string debuglog();

	virtual void createBoard(const CCPoint& ptBoardAnchor, int rows, int cols, float bubblesize);

	virtual void setState(EGameState s);
	EGameState getState() const { return _gamestate; }

	int getScore() const { return _nScore; }

	virtual void onBubbleClicked(BZBubble* pbubble) {};

	//void setAnchor(const CCPoint& ptBorn) { _ptLeftTopBorn = ptBorn; }
	void setLevelParams(BZLevelParams& params) { _params = params; }

	virtual bool isGameOver() const { return false; };
	
	virtual void clear();

	virtual void onPaused() { /*_stateWhenPaused = _state; setState(GS_GamePaused);*/ }
	virtual void onResume() { /* setState(_stateWhenPaused); */ }; //level up or paused ==> resumed
	virtual void onGameOver() { setState(GS_GameOver); }

	virtual void onEnter();
	virtual void onUpdate();
	virtual bool onEvent(const CAEvent* pevt);
	virtual void onExit();
};

#endif //_BZGAME_MODE_H_
