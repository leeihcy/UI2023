#include "signalslot.h"

#include <assert.h>
#include <functional>

static void slot1() {
    printf("this is slot1\n");
}
static int slot2(int a, int b) {
    printf("this is slot2: %d, %d\n", a, b);
    return a+b;
}
static void test1() {
    ui::signal<void()> sig;
    sig.connect(slot1);
    sig.emit();

    ui::signal<int(int, int)> sig2;
    sig2.connect(slot2);
    sig2.emit(3, 4);

    ui::signal<int(int, int), int> sig2_1;
    sig2_1.connect(slot2, 5);
    sig2_1.emit(6);
    sig2_1.emit(7);

	ui::signal<int(int,int)> sig3;
	sig3.connect([](int a, int b) { return a+b; });
    assert(7 == sig3.emit(3,4));
}

static void test2() {
	ui::function_traits<int(int,int)>::return_type a = 0;
    printf("%d\n", a);	
}
void signalslot_unittest() {
    test1();
	test2();
}

int main() {
    signalslot_unittest();
}
