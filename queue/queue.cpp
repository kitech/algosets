
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

    bret = qq.enqueue(578);
    assert(bret == true);

    assert(qq.count() == 2);

    bret = qq.enqueue(98362);
    assert(bret == true);

    assert(qq.count() == 3);

    assert(qq.full());

    int de;

    de = qq.dequeue();
    std::cout<<"den:"<<de<<std::endl;
    assert(qq.count() == 2);

    de = qq.dequeue();
    std::cout<<"den:"<<de<<std::endl;
    assert(qq.count() == 1);

    de = qq.dequeue();
    std::cout<<"den:"<<de<<std::endl;
    assert(qq.count() == 0);

    assert(qq.empty() == true);
}
