#include "signal.h"

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
    STACK[0] = STACK[0] + 1;
}
static int add(int a, int b) {
    return a+b;
}
static int sub(int a, int b) {
    return a-b;
}
static int multiply(int a, int b) {
    return a*b;
}
static int divide(int a, int b) {
    return a/b;
}

class Math {
public:
	int add(int a, int b) {
		return a+b;
	}
    void foo() {
        STACK[0] = STACK[0] + 1;
    }
    static void bar() {
        STACK[0] = STACK[0] + 2;
    }
};

// slot 功能测试
void test_slot_function() {

    // 简单函数类型：void()
    { 
        slot<void()> s1(foo);
        s1.emit();
        assert(FETCH(0) == 1);
    }

    // 带参数，带返回值函数类型：int(int, int)
    {
        slot<int(int, int)> s2(add);
        assert(7 == s2.emit(3, 4));
    }

    // 绑定1个参数
    {
        slot<int(int, int), int> s3(add, 3);
        assert(7 == s3.emit(4));
    }

    // 绑定2个参数
    {
        slot<int(int, int), int, int> s4(add, 3, 4);
        assert(7 == s4.emit());
    }

    // 兼容函数指针
    {
        slot<void(*)()> s1(foo);
        s1.emit();
        assert(FETCH(0) == 1);

        slot<int(*)(int,int)> s2(add);
        assert(7 == s2.emit(3, 4));

        slot<int(*)(int,int), int> s3(add, 3);
        assert(7 == s3.emit(4));

        slot<int(*)(int,int), int, int> s4(add, 3, 4);
        assert(7 == s4.emit());
    }

    // 直接推导模板
    {
        slot(foo).emit();
        assert(FETCH(0) == 1);

        assert(7 == slot(add).emit(3, 4));
        assert(7 == slot(add, 3).emit(4));
        assert(7 == slot(add, 3, 4).emit());
    }
}

// sloto功能测试2
void test_slot_method() {

    Math math;

    // 成员函数指针
    slot<int(Math::*)(int,int), Math*> s1(&Math::add, &math);
    assert(7 == s1.emit(3, 4));

    // 参数绑定
    {
        slot<int(Math::*)(int,int)> s1(&Math::add);
        assert(7 == s1.emit(&math, 3, 4));

        slot<int(Math::*)(int,int), Math*, int> s2(&Math::add, &math, 3);
        assert(7 == s2.emit(4));

        slot<int(Math::*)(int,int), Math*, int, int> s3(&Math::add, &math, 3, 4);
        assert(7 == s3.emit());
    }

    // 静态成员函数
    {
        slot<void(*)()> s1(&Math::bar);
        s1.emit();
        assert(FETCH(0) == 2);

        slot<void()> s2(&Math::bar);
        s2.emit();
        assert(FETCH(0) == 2);

        slot(&Math::bar).emit();
        assert(FETCH(0) == 2);
    }

    // 自动推导
    assert(7 == slot(&Math::add).emit(&math, 3, 4));
    assert(7 == slot(&Math::add, &math).emit(3, 4));
    assert(7 == slot(&Math::add, &math, 3).emit(4));
    assert(7 == slot(&Math::add, &math, 3, 4).emit());

    // TODO: 与普通函数统一
    // {
    //     slot<int(int,int), Math*> s1(&Math::add, &math);
    //     assert(7 == s1.emit(3, 4));
    // }
}

void test_slot_base()
{
    Math math;

    slot<int(int, int)> s1(&add);
    slot<int(Math::*)(int,int), Math*> s2(&Math::add, &math);

    assert(7 == s1.emit(3,4));
    assert(7 == s2.emit(3,4));

    // 两个slot其实可以共用一个基类。
    slot_base<int(int,int)>* sb[2];
    sb[0] = static_cast<slot_base<int(int,int)>*>(&s1);
    sb[1] = static_cast<slot_base<int(int,int)>*>(&s2);
    for (int i = 0; i < 2; i++) {
        assert(7 == sb[i]->emit(3,4));
    }
}

void test_signal_void() {
    Math math;

    // void() 
    {
        ui::signal<void()> s;
        s.connect(&foo);
        s.emit();
        assert(FETCH(0) == 1);
    }

    // 多个void ()
    {
        ui::signal<void()> s;
        s.connect(&foo);
        s.connect(&foo);
        s.connect(&foo);
        s.emit();
        assert(FETCH(0) == 3);
    }

    // 成员函数
    {
        ui::signal<void()> s;
        s.connect(&Math::foo, &math);
        s.emit();
        assert(FETCH(0) == 1);

        ui::signal<void(Math*)> s2;
        s2.connect(&Math::foo);
        s2.emit(&math);
        assert(FETCH(0) == 1);
    }
    {
        ui::signal<void()> s;
        s.connect(&Math::foo, &math);
        s.connect(&Math::foo, &math);
        s.connect(&Math::foo, &math);
        s.emit();
        assert(FETCH(0) == 3);
    }

    // 静态成员函数
    {
        ui::signal<void()> s;
        s.connect(&Math::bar);
        s.emit();
        assert(FETCH(0) == 2);
    }

    // 同时连接function与method
    {
        ui::signal<void()> s;
        s.connect(&Math::foo, &math);
        s.connect(&foo);
        s.emit();
        assert(FETCH(0) == 2);
    }

    // 空
    {
        ui::signal<void()> s;
        s.emit();
    }
}

struct ResultCombinerMax {
    void feed(int return_value) {
      m_value = std::max(m_value, return_value);
    }
    int m_value = 0;
};

void test_signal_return()
{
    Math math;

    // 单个 int(int,int)
    {
        ui::signal<int(int,int)> s;
        s.connect(&add);
        assert(7 == s.emit(3, 4));

        ui::signal<int(int,int)> s2;
        s2.connect(&Math::add, &math);
        assert(7 == s2.emit(3, 4));
    }

    // 多个 int(int,int)，结果取最后一个。
    {
        ui::signal<int(int,int)> s;
        s.connect(&add);
        s.connect(&sub);
        assert(-1 == s.emit(3, 4));

        ui::signal<int(int,int)> s2;
        s2.connect(&sub);
        s2.connect(&Math::add, &math);
        s2.connect(&multiply);
        assert(12 == s2.emit(3, 4));
    }

    // 自定义 result combiner
    {
        ui::signal<int(int,int)> s2;
        s2.connect(&multiply);
        s2.connect(&add);
        s2.connect(&divide);
        s2.connect(&sub);

        ResultCombinerMax combiner;
        s2.emit_with_combiner(&combiner, 3, 4);
        assert(combiner.m_value == 12);
    }
}

void signalslot_unittest() {
    test_slot_function();
    test_slot_method();

    test_slot_base();

    test_signal_void();
    test_signal_return();
}

int main() {
    signalslot_unittest();
}
