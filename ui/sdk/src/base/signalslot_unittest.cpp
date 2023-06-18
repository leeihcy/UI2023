#include "signalslot.h"

#include <assert.h>
#include <functional>

using namespace ui;

int STACK[10] = { 0 };
int FETCH(int index) {
    int value = STACK[index];
    STACK[index] = 0;
    return value;
}

static void foo() {
    // printf("this is foo\n");
    STACK[0] = 1;
}
static int add(int a, int b) {
    // printf("this is add: %d, %d\n", a, b);
    return a+b;
}
class Math {
public:
	int add(int a, int b) {
		return a+b;
	}
    static void foo() {
        STACK[0] = 2;
    }
};

// slot 功能测试
void test_slot_function() {

    // 简单函数类型：void()
    { 
        slot<void()> s1(foo);
        s1.invoke();
        assert(FETCH(0) == 1);
    }


    // 带参数，带返回值函数类型：int(int, int)
    {
        slot<int(int, int)> s2(add);
        assert(7 == s2.invoke(3, 4));
    }

    // 绑定1个参数
    {
        slot<int(int, int), int> s3(add, 3);
        assert(7 == s3.invoke(4));
    }
    
    // 绑定2个参数
    {
        slot<int(int, int), int, int> s4(add, 3, 4);
        assert(7 == s4.invoke());
    }

    // 兼容函数指针
    {
        slot<void(*)()> s1(foo);
        s1.invoke();
        assert(FETCH(0) == 1);

        slot<int(*)(int,int)> s2(add);
        assert(7 == s2.invoke(3, 4));

        slot<int(*)(int,int), int> s3(add, 3);
        assert(7 == s3.invoke(4));

        slot<int(*)(int,int), int, int> s4(add, 3, 4);
        assert(7 == s4.invoke());
    }

    // 直接推导模板
    {
        slot(foo).invoke();
        assert(FETCH(0) == 1);

        assert(7 == slot(add).invoke(3, 4));
        assert(7 == slot(add, 3).invoke(4));
        assert(7 == slot(add, 3, 4).invoke());
    }
}

// sloto功能测试2
void test_slot_method() {

    Math math;

    // 成员函数指针
    slot<int(Math::*)(int,int), Math*> s1(&Math::add, &math);
    assert(7 == s1.invoke(3, 4));

    // 参数绑定
    {
        slot<int(Math::*)(int,int)> s1(&Math::add);
        assert(7 == s1.invoke(&math, 3, 4));

        slot<int(Math::*)(int,int), Math*, int> s2(&Math::add, &math, 3);
        assert(7 == s2.invoke(4));

        slot<int(Math::*)(int,int), Math*, int, int> s3(&Math::add, &math, 3, 4);
        assert(7 == s3.invoke());
    }

    // 静态成员函数
    {
        slot<void(*)()> s1(&Math::foo);
        s1.invoke();
        assert(FETCH(0) == 2);

        slot<void()> s2(&Math::foo);
        s2.invoke();
        assert(FETCH(0) == 2);

        slot(&Math::foo).invoke();
        assert(FETCH(0) == 2);
    }

    // 自动推导
    assert(7 == slot(&Math::add).invoke(&math, 3, 4));
    assert(7 == slot(&Math::add, &math).invoke(3, 4));
    assert(7 == slot(&Math::add, &math, 3).invoke(4));
    assert(7 == slot(&Math::add, &math, 3, 4).invoke());

    // TODO: 与普通函数统一
    // {
    //     slot<int(int,int), Math*> s1(&Math::add, &math);
    //     assert(7 == s1.invoke(3, 4));
    // }
}

void test_signal() {
    //  signal<void()> s;
    //  s.connect(&foo);
    //s.emit();
}

void signalslot_unittest() {
    test_slot_function();
    test_slot_method();

    test_signal();
}

int main() {
    signalslot_unittest();
}
