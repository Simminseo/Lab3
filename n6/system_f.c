#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <command>, [argv0], [argv1]\n", argv[0]);
        exit(1);
    }
    else
    {
        char *command[256];
        sprintf(command, "%s%s", "/bin/", argv[1]);
        execl(command, argv[1], argv[2], (char *)0);
        return 0;
    }
}
