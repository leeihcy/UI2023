#include "message_loop.h"
using namespace ui;

void task_test(MessageLoop* loop) {
    loop->Quit();
}

void test1() {
    MessageLoop loop;
    loop.AddIdleTask(Slot(task_test, &loop));
    loop.Run();
}

int main() {
    test1();
    return 0;
}