#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <algorithm>
#include <vector>


// 调用栈方式，好像到40就走不动了。
uint64_t fib_recursive(int n)
{
    if (n == 0) return 0;
    if (n == 1) return 1;

    return fib_recursive(n - 1) + fib_recursive(n - 2);
}

uint64_t fib_reduce(int n)
{
    // 是否能使用两个变量实现这个功能？
    uint64_t v = 0;
    uint64_t v0 = 0;
    uint64_t v1 = 0;

    for (int i = 0; i < n; i++) {
        if (i > 1) {
            v = v0 + v1;
            v0 = v1;
            v1 = v;
        } else if (i == 1) {
            v = 1;
            v0 = 0;
            v1 = 1;
        } else {
            v = v0 = 0;
        }
    }

    return v;
}

uint64_t i = 0;
uint64_t gnfunc()
{
    return i++;
}

#define fibx fib_recursive
// #define fibx fib_reduce

int main(int argc, char **argv)
{
    int n = 30;
    uint64_t v = 0;
    v = fibx(n);
    printf("the %d's val: %lu\n", n, v);
    
    int num = 140; 
    std::vector<int> range_vec(num);
    std::generate_n(range_vec.begin(), num, gnfunc);
    for (auto e: range_vec) {
        n = e;
        v = 0;
        v = fibx(n);
        printf("the %d's val: %lu\n", n, v);
    }
}

