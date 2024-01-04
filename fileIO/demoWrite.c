#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int main()
{
#if 1
    int fd = open("./hello.txt", O_WRONLY );
    if (fd == -1)
    {
        perror("open error");
    }
    char *ptr = "hello world \n";
    /* 向文件之中写数据 */
    int bytes = write(fd, ptr, strlen(ptr) + 1);
    if(bytes < 0)
    {
        perror("writr error");
    }
    printf("bytes %d\n", bytes);
    close(fd);

#endif

#if 0
    int fd = open("./hello.txt", O_WRONLY | O_TRUNC);
    if (fd == -1)
    {
        perror("open error");
    }
    close(fd);
    
#endif
        return 0;
}