#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "stdlog.h"

#include "max_sub_set.h"

int max_sub_set(int *ssets, int len, int *ssb, int *sse)
{
    int res;
    int max;

    int tb = 0;
    *ssb = *sse = 0;
    res = max = ssets[0];

    for (int i = 1; i < len; ++i) {
        if (max >= 0) {
            max += ssets[i];
        } else {
            max = ssets[i];
            tb = i;
        }

        if (max > res) {
            res = max;
            *ssb = tb;
            *sse = i;
        }
    }

    return res;
}

void test_max_sub_set()
{
    int len = 80;
    int maxr = 0;
    int ssb=0, sse=-1;
    int *ssets = (int*)calloc(1, len * sizeof(int*));
    std::string sstr;
    char tbuf[20];

    srand(time(NULL));
    for (int i = 0; i < len; ++i) {
        ssets[i] = (rand() - rand()) % 10000;

        memset(tbuf, 0, sizeof(tbuf));
        snprintf(tbuf, sizeof(tbuf)-1, "%d,", ssets[i]);
        sstr += std::string(tbuf);
    }

    maxr = max_sub_set(ssets, len, &ssb, &sse);

    qLogx()<<"max sub set res:"<<maxr<<ssb<<sse<<". sets:"<<sstr;

    free(ssets);
}
