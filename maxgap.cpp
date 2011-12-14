#include "stdlog.h"

#include "maxgap.h"

int get_max_gap_direct(int *vx, int n)
{

}

int get_max_gap_bucket(int *vx, int n)
{
    int v_min = 0, v_max = 0;
    int bkt_b, bkt_e;
    int *bkts;
    int gap = (v_max - v_min)/(n-1);


    qLogx()<<v_min<<v_max<<gap;

}

void test_max_gap()
{

}

