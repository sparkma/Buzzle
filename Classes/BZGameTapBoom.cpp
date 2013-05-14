
#include "BZGameTapBoom.h"
#include "AStageLayer.h"
#include "AWorld.h"


BZGameTapBoom::BZGameTapBoom(CAStageLayer* player)
: BZGame(player)
{
	_name = "tapboom";
	_state = TBS_NA;
	_nLevel = 1;

	//_VerifyClass(this);
}

BZGameTapBoom::~BZGameTapBoom()
{
}

class _BlockMap
{
public:
	int rows, cols;
	string pat;
	
	char getp(int row, int col) const
	{
		_Assert(row >= 0 && row < rows);
		_Assert(col >= 0 && col < cols);
		return pat.c_str()[cols * row + col];
	}
	//get empty block or insert block count in colmun
	int getCount(int col, bool E)
	{
		int r = rows - 1;
		int count = 0;
		while (r >= 0)
		{
			if (getp(r, col) == (E ? '0' : '1'))
				count++;
			r--;
		}
		return count;
	}
	//get empty block or insert block count in map
	int getTCount(bool E)
	{
		int c;
		int n = 0;
		for (c = 0; c < cols; c++)
		{
			n += getCount(c, E);
		}
		return n;
	}
	int getEI(int col, char* ei, int size)
	{
		int r = rows - 1;

		int pair = 0;
		while (true)
		{
			int count = 0;
			while (r >= 0 && getp(r, col) == '1') 
			{
				count++;
				r--;
			}
			ei[pair++] = count;
			count = 0;
			while (r >= 0 && getp(r, col) != '1') 
			{
				count++;
				r--;
			}
			ei[pair++] = count;
			if (ei[pair - 1] == 0 && ei[pair - 2] == 0)
				break;
		}
		_Assert(pair <= size);
		return pair;
	}
	bool canNavigatePhase2(int level)
	{
		int col;
		int blocks[16];
		int n = 0;
		int t = 0;
		memset(blocks, 0, sizeof(blocks));
		for (col = 0; col < cols; col++)
		{
			int e = getCount(col, true);
			t += e;
			if (e <= 0)
			{
				n++;
			}
			else
			{
				blocks[n] += e;
			}
		}

		if (t < 10)
		{
			return true;
		}
		if (n >= 3)
		{
			return true;
		}
		return false;
	}
	bool canInsert(int row, int col, _BlockMap* pmapSelected)
	{
		return true;
	}

	void insert(int row, int col, _BlockMap* pmapSelected)
	{
	}
};

static _BlockMap patterns_raw[] = 
{
	{2, 1, "11"},			//OO
	{3, 1, "111"},			//OOO
	{4, 1, "1111"},
	{5, 1, "11111"},
	{6, 1, "111111"},
	{7, 1, "1111111"},
	//OX
	//OO
	{2, 2, "1111"},			
	//OX
	//OO
	{2, 2, "1011"},			
	//OX
	//OO
	//OX
	{2, 3, "101110"},		
	//OO
	//OX
	//OO
	{2, 3, "111011"},		
};

static _BlockMap* patterns = null;

//阶段0，随意填充稳定块阶段，并且插入点是空白区域。直到底部充满，则进入下一阶段。
//阶段1，布控阶段，难度提升阶段；可以底部插入，中部插入，上部插入稳定块，如果造成区域分割，则进入下一阶段。
//		可以Undo？ push整个局面。 70个字符。
//阶段2，补空阶段，收官阶段；插入简单块，补空收官。连续的块，符合级别规则，则直接填充。

//插入算法，此算法决定了阶段0，只能插入稳定块（不包含中间空位的块）。
//	1 待插入块的任何一列和对应的位置比较。需要满足以下条件：
//		a 空位（E）要大于等于插入块（I）的非空块个数; 
//		b a中的E-I差值必须为0，或者大于等于2；
//		c 非空位要大于等于插入块的空位个数;
//	2 对于插入块的任何一列执行以下步骤（列命名为COL）
//		a 扫描COL从下向上，获得非空块个数（N），直到找到空位或者到顶。
//		b 将插入位置的已有块上移N块。 
//		c 插入位置上移N。if 到顶，完成。
//		d 继续从新位置(COL+N)扫描，找到空块个数（M），知道找到非空位或者到顶。
//		e 将插入位置上移M。此位置一定存在非空块。if 到顶，完成。
//		f 重复a;
//	3 重复2直到所有列完成。
//
// 阶段变成
// 阶段1：插入块，直到局面中出现冒顶，将局面分割；或者剩余空位小于某个阈值。
//		  第一关阈值为14; 第128关阈值为10；
// 阶段2：野蛮补齐（不使用简单块）；差多少补充多少。
//
// 阶段变成
// 阶段1：插山，按级别生成山大小 (1-10:1, 11-20:2, 21-30:3, NN:3)
// 阶段2：补插模式块。
// 阶段3：补山。
// 整个过程可见，
void BZGameTapBoom::_initializeMap()
{
	_BlockMap map;
	map.rows = this->getRows();
	map.cols = this->getColumns();
	map.pat = "";
	int n = map.rows * map.cols;
	while (n-- >= 0) map.pat += "0";

	_BlockMap* pmapSelected = null;
	//init select condition
	while (!map.canNavigatePhase2(_nLevel))
	{
		//select a pattern

		//select a position
		//rand 5 position
		CCPoint poses[5];
		bool inserted = false;
		for (int i = 0; i < 5; i++)
		{
			int row = (int)poses[i].x;
			int col = (int)poses[i].y;
			if (map.canInsert(row, col, pmapSelected))
			{
				map.insert(row, col, pmapSelected);
				inserted = true;
				break;
			}
		}
		if (!inserted)
		{
			//modify select condition
		}
	}
	//map.phase2();
	//fill empty slots

	_map = map.pat;
}

void BZGameTapBoom::_doBornStrategy()
{
	ccTime time = _pLayer->getTimeNow();
	switch (_state)
	{
	case TBS_NA:
		_state = TBS_InitializeMap;
		break;
	case TBS_InitializeMap:
		_initializeMap();
		break;
	}
}

