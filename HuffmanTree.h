#pragma once
//包堆的头文件
#include"Heap.h"

template<class W>//W存权值
struct HuffmanTreeNode
{
	HuffmanTreeNode<W>* _left;
	HuffmanTreeNode<W>* _right;
	HuffmanTreeNode<W>* _parent;

	W _w;

	HuffmanTreeNode(const W& w)
		:_w(w)
		, _left(NULL)
		, _right(NULL)
		, _parent(NULL)
	{}
};

template<class W>
class HuffmanTree
{
	typedef HuffmanTreeNode<W> Node;
public:
	HuffmanTree()
		:_root(NULL)
	{}

	HuffmanTree(W *a, size_t n, const W& invalid)//只比较出现的字符，部分比较,所以使用非法值invalid
	{
		struct NodeCompare
		{
			bool operator()(Node* l, Node* r)//按权值比较
			{
				return l->_w < r->_w;
			}
		};
		Heap<Node*,NodeCompare> minHeap;//根据W中的权值比较而不是指针比较

		for (size_t i = 0; i < n; ++i)
		{
			if (a[i] != invalid)//限定比较的字符
			{
				minHeap.Push(new Node(a[i]));
			}
		}

		while (minHeap.Size()>1)//小堆
		{
			Node* left = minHeap.Top();
			minHeap.Pop();

			Node* right = minHeap.Top();
			minHeap.Pop();

			Node* parent = new Node(left->_w + right->_w);
			parent->_left = left;
			parent->_right = right;

			left->_parent = parent;
			right->_parent = parent;

			minHeap.Push(parent);
		}
		_root = minHeap.Top();
	}
	Node* GetRoot()
	{
		return _root;
	}

protected:
	Node* _root;
};

void TestHuffmanTree()
{
	int a[] = { 1, 2, 3, 4 };
	HuffmanTree<int> t(a, sizeof(a) / sizeof(a[0]), 0);
}
