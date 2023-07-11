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
    printf("program will auto exit at 3 seconds\n");
    MessageLoop loop;
    loop.PostTask(Slot(idle_task, &loop));
    loop.ScheduleTask(Slot(timeout_task, &loop), 1000);
    loop.Run();
}

int main() {
    test1();
    return 0;
}