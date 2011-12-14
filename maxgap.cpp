#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "stdlog.h"

#include "maxgap.h"

int get_max_gap_direct(int *vx, int n)
{

}

int get_max_gap_bucket(int *vx, int n)
{
    int v_min = INT_MAX, v_max = 0;
    int bkt_b, bkt_e;
//    int *bkts;
//    int *vxbks;
    int *vxbks_max;
    int *vxbks_min;
    int *vxbks_cnt;
    int vxidx;
    int x;
    int avggap;//  = (v_max - v_min)/(n-1);

    for (int i = 0; i < n; ++i) {
        x = vx[i];
        if (x < v_min) {
            v_min = x;
        }
        if (x > v_max) {
            v_max = x;
        }
    }
    avggap = (v_max - v_min)/(n-1);

    qLogx()<<v_min<<v_max<<avggap;

    vxbks_max = (int*)calloc(1, (n-1)*sizeof(int*));
    vxbks_min = (int*)calloc(1, (n-1)*sizeof(int*));
    vxbks_cnt = (int*)calloc(1, (n-1)*sizeof(int*));
    for (int i = 0; i < n; ++i) {
        vxbks_max[i] = -1;
        vxbks_min[i] = -1;
        vxbks_cnt[i] = 0;
    }

    for (int i = 0; i < n; ++i) {
        x = vx[i];
        vxidx = (x - v_min)/avggap + 1 - 1;
        // qLogx()<<"pos:"<<i<<" value:"<<x<<" into:"<<vxidx;
        // vxbks[vxidx] = i;
        if (vxbks_max[vxidx] == -1) {
             vxbks_max[vxidx] = x;
        }
        if (x > vxbks_max[vxidx]) {
            vxbks_max[vxidx] = x;
        }
        if (vxbks_min[vxidx] == -1) {
            vxbks_min[vxidx] = x;
        }
        if (x < vxbks_min[vxidx]) {
            vxbks_min[vxidx] = x;
        }
        vxbks_cnt[vxidx] ++;
    }

    int ml = 0;
    int mu = 0;
    int maxg = 0;
    int last_max = v_min;
    for (int i = 0; i < n; ++i) {
        if (vxbks_cnt[i] == 0) {

        } else {
            if (vxbks_max[i] - last_max > maxg) {
                maxg = vxbks_max[i] - last_max;
                ml = last_max;
                mu = vxbks_max[i];
            }
            last_max = vxbks_max[i];
        }
    }

    qLogx()<<"maxg:"<<maxg<<ml<<mu;// <<(n-1)<<sizeof(int*)<<(n-1)*sizeof(int);

    free(vxbks_max);
    free(vxbks_min);
    free(vxbks_cnt);

    return maxg;

    return 0;
}

void test_max_gap()
{
    int rtimes = 5;
    int n = 8000000;
    int *vx;
    int x;
    int max_gap = 0;

    srand(time(NULL));
    vx = (int*)calloc(1, n * sizeof(int*));

    for (int k = 0; k < rtimes; ++k) {
        for (int i = 0; i < n; ++i) {
            x = rand() ;//% 1000;
            vx[i] = x;
            // qLogx()<<i<<x;
        }

        max_gap = get_max_gap_bucket(vx, n);
        qLogx()<<k<<max_gap;
    }

    free(vx);
}

