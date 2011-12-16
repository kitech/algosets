#ifndef MAX_SUB_SET_H
#define MAX_SUB_SET_H

/*
  返回最大序列的和
  序列存放在rsets
  ssb: 返回子序列的开始位置
  sse: 返回子序列的结束位置
  子序列为：[ssb,sse], 为闭区间
  */
int max_sub_set(int *ssets, int *ssb, int *sse);

void test_max_sub_set();

#endif // MAX_SUB_SET_H
