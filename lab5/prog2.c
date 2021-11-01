#include <fcntl.h>

int main()
{
    char c;
    // have kernel open two connection to file alphabet.txt
    int fd1 = open("symbols.txt", O_RDONLY);
    int fd2 = open("symbols.txt", O_RDONLY);

    while (read(fd1, &c, 1) == 1 || read(fd2, &c, 1) == 1)
    {
        write(1, &c, 1);
    }
    return 0;
}
