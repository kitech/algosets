
#include <assert.h>

#include "queue.h"

void test_queue_queue()
{
    bool bret;
    LeoQueue qq;

    assert(qq.empty());

    bret = qq.enqueue(56);
    assert(bret == true);

    assert(qq.count() == 1);

    qq.enqueue(578);

    assert(qq.count() == 2);

    qq.enqueue(98362);

    assert(qq.count() == 3);

    assert(qq.full());

    int de;

    de = qq.dequeue();

    assert(qq.count() == 2);
}
