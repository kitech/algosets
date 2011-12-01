#ifndef _QUEUE_H_
#define _QUQUE_H_

#include <iostream>

class LeoQueue
{
public:
    LeoQueue() {
        head = -1;
        tail = 0;
    }

    virtual ~LeoQueue() {

    }

    bool enqueue(int e) {
        if (!this->full()) {
            if (head == -1) {
                head = tail;
            }

            this->elems[tail] = e;

            if (tail == MAX_ELEMENT_COUNT-1) {
                tail = 0;
            } else {
                ++ tail;
            }
        } else {
            return false;
        }
        return true;
    }

    // 如果当前的队列为空，应该怎么返回呢？
    int dequeue() {
        int re = 0;
        if (!this->empty()) {
            re = this->elems[head];
            if (head == MAX_ELEMENT_COUNT-1) {
                head = 0;
            } else {
                ++head;
            }
            if (head == tail) {
                head = -1;
            }
        } else {
            return -1;
        }
        return re;
    }

    bool full() {
        if (head == tail) {
            return true;
        }
        return false;
    }

    bool empty() {
        if (head == -1) {
            return true;
        }

        return false;
    }

    int size() {
        return sizeof(MAX_ELEMENT_COUNT);
    }

    int count() {
        if (head == -1) {
            return 0;
        }
        std::cout<<"c="<<head<<" " <<tail<<std::endl;
        if (head == tail) {
            // full state
            return MAX_ELEMENT_COUNT;
        } else if (head < tail) {
            return (tail - head);
        } else {
            int c = ((MAX_ELEMENT_COUNT - head) + tail);
            std::cout<<"c="<<c<<std::endl;
            return c;
        }
        return 0;
    }

protected:
    const static int MAX_ELEMENT_COUNT = 3;
    int elems[MAX_ELEMENT_COUNT];
    int head; // 指向当前可取元素的位置
    int tail; // 指向当前可存元素的位置
};


void test_queue_queue();

#endif
