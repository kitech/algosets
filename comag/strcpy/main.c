#include <stdlib.h>

/*
  编译： qo -g -x

  检查参数
  返回dst地址
  拷贝字符串未尾的'\0'字符
 */
// 代码短，但效率比glibc中的strcpy函数慢了20倍。
// inline char * zstrcpy(char *dst, const char *src);
static char * zstrcpy(char *dst, const char *src)
{
    if (dst == NULL || src == NULL) {
        return NULL;
    }

    char *p = dst;
    while (*dst ++ = *src++) {
        // printf("%d\n", *src);
    }

    // memcpy(dst, src, strlen(src) + 1);
    // snprintf(dst, "%s", src, strlen(str));
    
    return dst;
}

static void zstrcpy_bm()
{
    int i = 10000000;
    char buf[100];

    while (i -- > 0) {
        zstrcpy(buf, "oijewfewfjijo中为地");
    }
}

static void strcpy_bm()
{
    int i = 10000000;
    char buf[100];

    while (i -- > 0) {
        strcpy(buf, "oijewfewfjijo中为地");
    }
}

int main(int argc, char **argv)
{
    char buf[100];
    strcpy(buf, "fffffffffffffvvvvvvvvvv");
    // char *p = zstrcpy(buf, "abc123456");
    // printf("buf=%p:, p=%p, buf=%s\n", buf, p, buf);

    zstrcpy_bm();
    
    return 0;
}
