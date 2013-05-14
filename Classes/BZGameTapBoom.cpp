
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

//�׶�0����������ȶ���׶Σ����Ҳ�����ǿհ�����ֱ���ײ��������������һ�׶Ρ�
//�׶�1�����ؽ׶Σ��Ѷ������׶Σ����Եײ����룬�в����룬�ϲ������ȶ��飬����������ָ�������һ�׶Ρ�
//		����Undo�� push�������档 70���ַ���
//�׶�2�����ս׶Σ��չٽ׶Σ�����򵥿飬�����չ١������Ŀ飬���ϼ��������ֱ����䡣

//�����㷨�����㷨�����˽׶�0��ֻ�ܲ����ȶ��飨�������м��λ�Ŀ飩��
//	1 ���������κ�һ�кͶ�Ӧ��λ�ñȽϡ���Ҫ��������������
//		a ��λ��E��Ҫ���ڵ��ڲ���飨I���ķǿտ����; 
//		b a�е�E-I��ֵ����Ϊ0�����ߴ��ڵ���2��
//		c �ǿ�λҪ���ڵ��ڲ����Ŀ�λ����;
//	2 ���ڲ������κ�һ��ִ�����²��裨������ΪCOL��
//		a ɨ��COL�������ϣ���÷ǿտ������N����ֱ���ҵ���λ���ߵ�����
//		b ������λ�õ����п�����N�顣 
//		c ����λ������N��if ��������ɡ�
//		d ��������λ��(COL+N)ɨ�裬�ҵ��տ������M����֪���ҵ��ǿ�λ���ߵ�����
//		e ������λ������M����λ��һ�����ڷǿտ顣if ��������ɡ�
//		f �ظ�a;
//	3 �ظ�2ֱ����������ɡ�
//
// �׶α��
// �׶�1������飬ֱ�������г���ð����������ָ����ʣ���λС��ĳ����ֵ��
//		  ��һ����ֵΪ14; ��128����ֵΪ10��
// �׶�2��Ұ�����루��ʹ�ü򵥿飩������ٲ�����١�
//
// �׶α��
// �׶�1����ɽ������������ɽ��С (1-10:1, 11-20:2, 21-30:3, NN:3)
// �׶�2������ģʽ�顣
// �׶�3����ɽ��
// �������̿ɼ���
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

