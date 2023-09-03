#pragma once

// 用于解析继承关系的数据结构，TODO: 抽象出来使用
template<class T>
class tree
{
public:
	tree()
	{
		parent = nullptr;
		first_child = nullptr;
		first_neighbour = nullptr;
	};

	T         data;
	tree<T>*  parent;
	tree<T>*  first_child;
	tree<T>*  first_neighbour;
};