#include "weakptr.h"
#include "../signalslot/signal.h"
#include <assert.h>

using namespace ui;

int STACK[10] = { 0 };
int FETCH(int index) {
    int value = STACK[index];
    STACK[index] = 0;
    return value;
}


class Math {
public:
    void foo() {
        assert(m_var != -1);
        STACK[0] = STACK[0] + 1;
    }
    Math() {
        // printf(" Math %p\n", this);
    }
    ~Math() {
        // printf("~Math %p\n", this);
        m_var = -1;
    }
public:
    weakptr_factory<Math> m_weakptr_factory = {this};
    int m_var = 0;
};

void test_destructor() {
    Math* p;
    
    weakptr<Math> mathptr;
    assert(!mathptr);

    printf("enter-----\n");
    {
        Math math;
        mathptr = math.m_weakptr_factory.get();
        assert(mathptr);

        mathptr->foo();

        weakptr<Math> mathptr2 = mathptr;
        mathptr2->foo();
        weakptr<Math> mathptr3(mathptr);
        mathptr3->foo();
    }
    printf("exit-----\n");

    // 对象释放后可检测出来
    assert(!mathptr);
    if (mathptr) {
        mathptr->foo();
    }
}

void test_slot() {
    // 正常版本
    {
        ui::signal<void()> s;
            
        Math math;
        weakptr<Math> mathptr = math.m_weakptr_factory.get();
        s.connect(&Math::foo, &math);
        s.connect(&Math::foo, mathptr);
        s.emit();

        assert(FETCH(0) == 2);
    }

    // 销毁版本
    {
        ui::signal<void()> s;
        {
            Math math;
            weakptr<Math> mathptr = math.m_weakptr_factory.get();
            s.connect(&Math::foo, mathptr);
        }
        s.emit();

        assert(FETCH(0) == 0);
    }
}

void weakptr_unittest() {
    //test_destructor();
    test_slot();
}

int main() {
    weakptr_unittest();
}
