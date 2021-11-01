#include <stdio.h>
#include <sys/stat.h>
#include <pthread.h>

void *write_file(int arg)
{
    int num = arg;
    struct stat statbuf;
    FILE *f = fopen("result_file_2.txt", "a");
    stat("result_file_2.txt", &statbuf);
    printf("opened file #%d inode_number  = %ld, blocksize= %ld,  buffsize = %ld\n ", num, statbuf.st_ino, statbuf.st_size, statbuf.st_blksize);

    for (char c = 'a'; c <= 'z'; c++) {
        if (c % 2 == arg)
            fprintf(f, "%c", c);
    }

    fclose(f);
    stat("result_file_2.txt", &statbuf);
    printf("fclose file #%d inode  = %ld, buffsize = %ld blocksize= %ld\n", num, statbuf.st_ino, statbuf.st_size, statbuf.st_blksize);
    return 0;
}

int main()
{
    pthread_t thread;
    int code = pthread_create(&thread, NULL, write_file, 0);
    if (code != 0)
    {
        printf("can't create thread, code = %d\n", code);
        return -1;
    }
    write_file(1);

    pthread_join(thread, NULL);
    return 0;
}