
#include "BZGameLogic.h"
#include "AStageLayer.h"

#define _FALLING_DX_	(1.0f / 20.0f)
#define _FALLING_DELAY	(0.2f)

///Block
BZBlock::BZBlock(BZGame* pgame)
{
	_pgame = pgame;
	_pgroup = null;
	_acceleration = DEFAULT_ACCELERATION;
	_setState(BS_NA);

	memset(_neighbours, 0, sizeof(_neighbours));
}

BZBlock::~BZBlock()
{
}

void BZBlock::setFallingAcceleration(float a)
{
	_acceleration = a;
}

void BZBlock::_setState(EBlockState s)
{
	_timeStateBegin = _pgame->getTimeNow();
	_state = s;
	_pgame->onBlockStateChanged(this, _state);
}

void BZBlock::setBlock(const char* block)
{
	CASprite* pspr = new BZSpriteCommon(_pgame->layer(), block);
	pspr->setState("na");
	_psprBlock = pspr;
}

void BZBlock::setNeighbour(EBlockNeighbour bn, BZBlock* pblock)
{
	if (_neighbours[bn]) 
	{
		BZBlock* pn = _neighbours[bn];
		pn->release();
		_neighbours[bn] = null; //must set to null first
		pn->setNeighbour((EBlockNeighbour)((bn + 2) % 4), null);
	}
	_neighbours[bn] = pblock;
	if (pblock) pblock->retain();

	//change pose here
}

void BZBlock::attachTo(CAStageLayer* pl)
{
	_Assert(pl);
	_Assert(_psprBlock);

	pl->addSprite(_psprBlock);

	return;
}

void BZBlock::_setPos(float x, float y)
{
	_Assert(_pgame);
	if (_pgame->refineBlockPosition(this, x, y))
	{
		_pos.x = x;
		_pos.y = y;
		_row = (int)(y + 0.5f);
		_col = (int)(x + 0.5f);
		_Assert(_pgame->verifyBlock(this));
	}
	else
	{
		//can not change position now
	}
}

void BZBlock::onEnter()
{
}

void BZBlock::onUpdate()
{
	switch (_state)
	{
	case BS_NA:
		_setState(BS_NAing);;
		break;
	case BS_NAing:
		//do nothing, until state is changed to another
		break;
	case BS_Born:
		_psprBlock->setState("born");
		_setState(BS_Borning);
		break;
	case BS_Borning:
		if (_psprBlock->isAnimationDone())
		{
			_setState(BS_Fall);
		}
		break;
	case BS_Fall: 
		//delay for _FALLING_DELAY(0.2f) sec before falling
		if (_pgame->getTimeNow() - _timeStateBegin < _FALLING_DELAY)
		{
			_setState(BS_Falling);
			_lastFallingTime = _pgame->getTimeNow();
			_fallingspeed = 0;
		}
		break;
	case BS_Falling:
		if (_pgame->canFalling(this))
		{
			//I will calculate speed here, until game tell me I can stop
			//my position may not be aligned in grid! so, be careful!
			CCPoint posOld = _pos;
			CCPoint posNew = _pos;

			float t = _pgame->getTimeNow() - _lastFallingTime;
			_lastFallingTime = _pgame->getTimeNow();

			//posNew.x
			float f = floor(posNew.x);
			float c = ceil(posNew.x);
			if (c - posNew.x < 0.5f) { if (posNew.x + _FALLING_DX_ < c) posNew.x += _FALLING_DX_; else posNew.x = c; }
			else { _Assert(posNew.x - f < 0.5f); if (posNew.x - _FALLING_DX_ > f) posNew.x -= _FALLING_DX_; else posNew.x = f; }

			//acc = 10	v0 = 0
			//0		1		2		3		4
			//		5 s=10	20 s=20	45 s=30	80						 

			posNew.y += _fallingspeed * t + _acceleration * t / 2.0f;
			_fallingspeed += _acceleration * t;


			//notify the game to refine position of me
			_pgame->onBlockPositionChanged(this, posOld, posNew);

			_pos = posNew;
		}
		else
		{
			_setState(BS_Stop);
		}
		break;
	case BS_Drag:
		//enable block light here
		//and change state
		_setState(BS_Dragging);
		break;
	case BS_Dragging:
		break;
	case BS_Release:
		//turn off block light now
		//block will falling
		_setState(BS_Fall);
		break;
	case BS_Stop:
		_setState(BS_Stopping);
		{
			//adjust delta x
			CCPoint posNew = _pos;
			posNew.x = (float)((int)(posNew.x + 0.5f));
			posNew.y = (float)((int)(posNew.y + 0.5f));
			this->_setPos(posNew.x, posNew.y);
			//_psprBlock->setState("stop");
		}
		break;
	case BS_Stopping:
		//calcualte blending here
		if (_pgame->getTimeNow() - _timeStateBegin > 0.3f 
			&& 
			_psprBlock->isAnimationDone())
		{
			_setState(BS_Stand);
		}
		break;
	case BS_Stand:
		_setState(BS_Standing);
		break;
	case BS_Standing:
		//do nothing here
		//if I should be belended with another block, game will call setNeighbour
		if (null == this->_neighbours[N_BOTTOM] && _row != _pgame->getRows() - 1)
		{
			_setState(BS_Fall);
		}
		break;
	case BS_Die:
		_setState(BS_Dying);
		_psprBlock->setState("dead");
		break;
	case BS_Dying:
		//do nothing, "dead" is the deadpose
		//and _psprBlock is BZSpriteCommon
		break;
	default:
		_Assert(false);
		break;
	}
}

void BZBlock::onExit()
{
}


/// Game
BZGame::BZGame(CAStageLayer* player, int rows, int cols)
{
	_pLayer = player;
	
	_rows = rows;
	_cols = cols;
	_blocksInBoards = new BZBlock* [rows * cols];
	memset(_blocksInBoards, 0, sizeof(BZBlock*) * rows * cols);

	_nScores = 0;
	_timeLastBorn = 0;

	//_blocks = CCArray::arrayWithCapacity(100);
	_groups = CCArray::arrayWithCapacity(40);
	_psprDoodads = CCArray::arrayWithCapacity(20);
}

BZGame::~BZGame()
{
	int i;
	for (i = 0; i < _rows * _cols; i++)
	{
		if (_blocksInBoards[i])
		{
			_blocksInBoards[i]->release();
			_blocksInBoards[i] = null;
		}
	}
	//_blocks->release();
	//_blocks = null;
	_psprDoodads->release();
	_psprDoodads = null;
	_groups->release();
	_groups = null;
}

float BZGame::getTimeNow()
{ 
	return _pLayer->getTimeNow(); 
}

void BZGame::onEnter()
{
	_timeLastBorn = 0;

	//later, we will load this from xml
	int n = 0;
	_types[n++] = "yello";
	_types[n++] = "pink";
	_types[n++] = "pred";
}

BZBlock* BZGame::_createUnmanagedBlock(const char* type, const CCPoint& pt)
{
	string name = type;
	name += "_blocks";
	BZBlock* pblock = new BZBlock(this);
	pblock->setBlock(name.c_str());
	pblock->setInitPos(pt);
	pblock->setState(BS_Born);

	return pblock;
}


//we can refine posNew here, and we can stop this block here
void BZGame::onBlockPositionChanged(BZBlock* pblock, const CCPoint& posOld, CCPoint& posNew)
{
	//do not fall through the block under me
	
}

bool BZGame::refineBlockPosition(BZBlock* pblock, float& x, float& y)
{
	int r = (int)(x + 0.5f);
	int c = (int)(y + 0.5f);
	BZBlock* pb = _getBlock(r, c);
	if (null == pb)
		return true;
	if (pblock == pb)
		return true;
	//there is another block at (r, c)
	return false;
}

bool BZGame::verifyBlock(BZBlock* pblock)
{
	return _getBlock(pblock->getIndexRow(), pblock->getIndexColumn()) == pblock;
}

//block will call this function when block falling
//can I stop now? 
bool BZGame::canFalling(BZBlock* pblock)
{
	int r = pblock->getIndexRow();
	int c = pblock->getIndexColumn();

	bool bWillStop = false;
	if (r >= _rows - 1)
	{
		bWillStop = true;
	}
	else
	{
		BZBlock* pblockBottom = _getBlock(r + 1, c);
		if (null != pblockBottom && BS_Standing == pblockBottom->getState())
		{
			bWillStop = true;
		}
	}
	if (bWillStop)
	{
		float y = pblock->getPos().y;
		if (CAUtils::almostEqual(y, (float)r, 0.00001f))
		{
			return true;
		}
	}
	return false;
}

void BZGame::onBlockStateChanged(BZBlock* pblock, EBlockState state)
{
	switch (state)
	{
	case BS_Dying:
	case BS_Falling:
		//remove neighbours of me
		pblock->setNeighbour(N_TOP, null);
		pblock->setNeighbour(N_LEFT, null);
		pblock->setNeighbour(N_BOTTOM, null);
		pblock->setNeighbour(N_RIGHT, null);
		break;
	case BS_Standing:
		//find neighbours here, and blend with them
		{
			int r = pblock->getIndexRow();
			int c = pblock->getIndexColumn();
			//test top
			int				dr[4] = { 0,	+1,			0,			-1};
			int				dc[4] = {-1,	0,			+1,			0};
			EBlockNeighbour	nb[4] = {N_TOP,	N_RIGHT,	N_BOTTOM,	N_LEFT};
			BZBlock*		pblockNeighbour;
			int i;
			for (i = 0; i < 4; i++)
			{
				pblockNeighbour = this->_getBlock(r + dr[i], c + dc[i]);
				if (null != pblockNeighbour && BS_Standing == pblockNeighbour->getState())
				{	
					pblock->setNeighbour(nb[i], pblockNeighbour);
				}
			}
			/*
			unsigned int i, j;
			unsigned int c = _blocks->count();
			for (i = 0; i < c; i++)
			{
				BZBlock* pblockA = (BZBlock*)_blocks->objectAtIndex(i);
				const CCPoint& posA = pblockA->getPos();
				for (j = 0; j < c; j++)
				{
					BZBlock* pblockB = (BZBlock*)_blocks->objectAtIndex(j);
					if (BS_Standing == pblockB->getState())
					{
						//we can test distance of them
						const CCPoint& posB = pblockB->getPos();
						float dist2 = (posA.x - posB.x) * (posA.x - posB.x) + (posA.y - posB.y) * (posA.y - posB.y);
						if (CAUtils::almostEqual(dist2, 1.0f, 0.0001f))
						{
							//we are neighbure
							if (CAUtils::almostEqual(posA.x, posB.x, 0.0001f))
							{
								// | X | 
								// | X |
								if (posA.y > posB.y)
								{
									// | B | 
									// | A |
									pblockA->setNeighbour(N_TOP, pblockB);
									//pblockB->setNeighbour(N_BOTTOM, pblockA);
								}
								else
								{
									// | A | 
									// | B |
									pblockA->setNeighbour(N_BOTTOM, pblockB);
									//pblockB->setNeighbour(N_TOP, pblockA);
								}
							}
							else 
							{
								_Assert(CAUtils::almostEqual(posA.y, posB.y, 0.0001f));
								// | X | X |
								if (posA.x > posB.x)
								{
									// | B | A |
									pblockA->setNeighbour(N_LEFT, pblockB);
									//pblockB->setNeighbour(N_RIGHT, pblockA);
								}
								else
								{
									// | A | B |
									pblockA->setNeighbour(N_RIGHT, pblockB);
									//pblockB->setNeighbour(N_LEFT, pblockA);
								}
							}
						}
					}
				}
			}
			*/
		}
		break;
	}
}

void BZGame::_doBornStrategy()
{
	ccTime time = _pLayer->getTimeNow();
	if (time - _timeLastBorn > _params._fDelayBlockBorn)
	{
		_timeLastBorn = time;

		//select column first
		int typ = (int)CAUtils::Rand(0, (float)_params._fRangeblockBorn);
		_Assert(typ >= 0 && typ < BLOCK_TYPES);
		string type = _types[typ];

		//how many free slots 
		int free = 0;
		int slots[64];
		_Assert(_cols <= 64);

		memset(slots, 0, sizeof(slots));
		int i;
		for (i = 0; i < _cols; i++)
		{
			if (null == _getBlock(0, i))
			{
				slots[free++] = i;
			}
		}

		//create a block if there is a free slot
		if (free > 0)
		{
			//rand a slot
			int rand = (int)CAUtils::Rand(0, (float)free);
			int slot = slots[rand];

			BZBlock* pblock = _createUnmanagedBlock(type.c_str(), ccp(0, slot));
			_setBlock(0, slot, pblock);

			pblock->onUpdate();	//update real position of this block
			pblock->attachTo(_pLayer);
		}
	}
}


void BZGame::_doBlocksUpdate()
{
	int i;

	for (i = 0; i < _rows * _cols; i++)
	{
		BZBlock* pblock = _getBlock(i / _cols, i % _cols);
		if (pblock)
		{
			pblock->onUpdate();
		}
	}
}

void BZGame::onUpdate()
{
	_doBornStrategy();
	_doBlocksUpdate();


}

void BZGame::onExit()
{
}

