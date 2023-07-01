#include "message_loop.h"
using namespace ui;

void idle_task(MessageLoop* loop) {
    printf("idle_task\n");
    // loop->Quit();
}
bool timeout_task(MessageLoop* loop) {
    static int hitcount = 0;
    hitcount ++;

    printf("timeout_task: %d\n", hitcount);
    bool continue_ = hitcount < 3;
    if (!continue_) {
        loop->Quit();
    }
    return continue_;
}

void test1() {
    MessageLoop loop;
    loop.AddIdleTask(Slot(idle_task, &loop));
    loop.AddTimeout(1000, Slot(timeout_task, &loop));
    loop.Run();
}

int main() {
    test1();
    return 0;
}