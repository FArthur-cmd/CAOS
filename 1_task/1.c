#include <stdint.h>
#include <stdio.h>

void
transform(char* val_from, int64_t* val_to, int64_t k_from_to)
{
    int64_t index = 0;
    while (val_from[index] != '\0') {
        *val_to *= k_from_to;
        if ((val_from[index] >= '0') && (val_from[index] <= '9'))
        {
            *val_to += val_from[index] - '0';
        }
        else{
            *val_to += val_from[index] - 'a';
        }
        ++index;
    }
}

double
sum(double x, uint32_t y, char* z)
{
    uint64_t Base = 27;
    int64_t tmp_z = 0;
    transform(z, &tmp_z, Base);
    return x + y + tmp_z;
}

int
main(int argc, char *argv[])
{
    double x;
    uint32_t y = 0x00;
    char* z = argv[1];
    scanf("%lf %x", &x, &y);
    printf("%.3lf", sum(x, y, z));
    return 0;
}
