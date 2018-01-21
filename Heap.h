#pragma once
#include<iostream>
#include<vector>
#include<assert.h>
using namespace std;

template<class T,class Compare>
class Heap                                         //仿函数管理大小堆更方便
{
public:
	Heap()
	{}
	Heap(T* a, size_t n)
	{
		_a.reserve(n);
		for (size_t i = 0; i < n; ++i)
		{
			_a.push_back(a[i]);
		}
		//建堆
		for (int i = (_a.size() - 2) / 2; i >= 0; --i)//需要等于0所以用int
		{
			AdjustDown(i);
		}
	}
	size_t Size()
	{
		return _a.size();
	}
	void AdjustDown(int root)//向下调整
	{
		Compare com;
		int parent = root;
		int child = parent * 2 + 1;
		while (child < _a.size())
		{
			if (child + 1 < _a.size()
				&& com(_a[child + 1],_a[child]))//比较出大的孩子
			{
				++child;
			}
			if (com(_a[child], _a[parent]))
			{
				swap(_a[child], _a[parent]);
				parent = child;
				child = parent * 2 + 1;
			}
			else   //表示父亲大于孩子
			{
				break;
			}
		}
	}
	void Push(const T& x)
	{
		_a.push_back(x);
		AdjustUp(_a.size() - 1);
	}
	void AdjustUp(int child)//向上调整
	{
		Compare com;
		int parent = (child - 1) / 2;
		while (child > 0)//这里的parent不可能为负数
		{
			//if (_a[child]>_a[parent])
			if (com(_a[child],_a[parent]))
			{
				swap(_a[child], _a[parent]);
				child = parent;
				parent = (child - 1) / 2;
			}
			else
			{
				break;
			}
		}
	}

	void Pop()//pop最大的数据
	{
		assert(!_a.empty());
		swap(_a[0], _a[_a.size() - 1]);
		_a.pop_back();
		AdjustDown(0);
	}
	T& Top()
	{
		assert(!_a.empty());

		return _a[0];
	}

private:
	vector<T>_a;
};
template<class T>
struct Less
{
	bool operator()(const T& l, const T& r)
	{
		return l < r;
	}
};
template<class T>
struct Greater
{
	bool operator()(const T& l, const T& r)
	{
		return l > r;
	}
};
void AdjustDwon(int* a, size_t n, int root)
{
	int parent = root;
	int child = parent * 2 + 1;
	while (child < n)
	{
		if (child + 1 < n
			&& a[child + 1] > a[child])
		{
			++child;
		}
		if (a[parent] < a[child])
		{
			swap(a[parent], a[child]);
			parent = child;
			child = parent * 2 + 1;
		}
		else
		{
			break;
		}
	}
}
void HeapSort(int* a,size_t n)//堆排序
{
	for (int i = (n - 2) / 2; i >= 0; --i)//建堆
	{
		AdjustDwon(a, n, i);
	}
	//排序
	size_t end = n - 1;
	while (end > 0)//当end不等于0时候就需要交换
	{
		swap(a[0], a[end]);
		AdjustDwon(a, end, 0);//end 是最后数的下标以及个数
		--end;
	}
}

void TestHeap()
{
	//int a[] = { 10, 11, 13, 12, 16, 18, 15, 17, 14, 19 };
	//Heap<int>hp1(a, sizeof(a)/sizeof(a[0]));
	//hp1.Push(30);
	//hp1.Pop();
	/*int a[] = { 10, 11, 13, 12, 16, 18, 15, 17, 14, 19 };
	Heap<int,Less<int>>hp1(a, sizeof(a)/sizeof(a[0]));
	Heap<int,Greater<int>>hp2(a, sizeof(a) / sizeof(a[0]));*/
	//Less<int>less;
	//cout << less(1, 2)<<endl;
}
