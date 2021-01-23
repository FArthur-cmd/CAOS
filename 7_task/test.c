#include <string.h>
#include <stdio.h>

extern void normalize_path(char * path);

int
main()
{
    char path[200];
    scanf("%s", path);
    normalize_path(path);
    printf("%s\n", path);
}
