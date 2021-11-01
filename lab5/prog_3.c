#include <stdio.h>
#include <sys/stat.h>

int main() {
    struct stat *statbuf;

    FILE *f1 = fopen("result_file.txt", "a");
    stat("result_file.txt", statbuf);
    printf("fopen file #1 inode  = %ld, buffsize = %ld blocksize= %ld\n", statbuf->st_ino, statbuf->st_size,
           statbuf->st_blksize);


    FILE *f2 = fopen("result_file.txt", "a");
    stat("result_file.txt", statbuf);
    printf("fopen file #2 inode  = %ld, buffsize = %ld blocksize= %ld\n", statbuf->st_ino, statbuf->st_size,
           statbuf->st_blksize);

    for (char c = 'a'; c <= 'z'; c++) {
        if (c % 2 == 1)
            fprintf(f1, "%c", c);
        else
            fprintf(f2, "%c", c);
    }

    fclose(f1);
    stat("result_file.txt", statbuf);
    printf("fclose file #1 inode  = %ld, buffsize = %ld blocksize= %ld\n", statbuf->st_ino, statbuf->st_size,
           statbuf->st_blksize);

    fclose(f2);
    stat("result_file.txt", statbuf);
    printf("fclose file #2 inode  = %ld, buffsize = %ld blocksize= %ld\n", statbuf->st_ino, statbuf->st_size,
           statbuf->st_blksize);

    return 0;
}