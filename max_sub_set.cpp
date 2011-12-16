#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "stdlog.h"

#include "max_sub_set.h"

int max_sub_set(int *ssets, int len, int *ssb, int *sse)
{
    int res = - INT_MAX;

    int *rsubs = (int*)calloc(1, len * sizeof(int*));


    memset(rsubs, 0, sizeof(int*)*len);
    for (int i = 0; i < len; ++i) {
        if (i == 0) {
            rsubs[i] = ssets[i];
        } else {
            rsubs[i] = rsubs[i-1] > 0 ? (rsubs[i-1]+ssets[i]) : ssets[i];
        }
        if (rsubs[i] > res) {
            res = rsubs[i];
            *sse = i;
        }
    }

    free(rsubs);

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

    qLogx()<<"max sub set res:"<<maxr<<ssb<<sse<<sstr;

    free(ssets);
}
