#pragma once

// ���ڽ����̳й�ϵ�����ݽṹ��TODO: �������ʹ��
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