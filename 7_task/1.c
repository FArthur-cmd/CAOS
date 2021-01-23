#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <stdio.h>
#include <inttypes.h>

typedef enum {
    all_length = 18,
    year_start = 0,
    month_start = 5,
    day_start = 8,
    hour_start = 11,
    minute_start = 14
} constants;

time_t
make_time_from(char *time) 
{
    struct tm for_time;

    memset(&for_time, 0, sizeof(for_time));

    for_time.tm_year = atoi(time) - 1900;
    for_time.tm_mon = atoi(time + month_start) - 1; 
    for_time.tm_mday = atoi(time + day_start);
    for_time.tm_hour = atoi(time + hour_start);
    for_time.tm_min = atoi(time + minute_start);
    for_time.tm_isdst = -1;

    return mktime(&for_time);
}

int
main()
{
    char first_date[all_length], second_date[all_length];
    fgets(first_date, all_length * sizeof(char), stdin);
    time_t first_time = make_time_from(first_date);
    time_t second_time;
    while (fgets(second_date, sizeof(first_date), stdin)) { 
	second_time = make_time_from(second_date);
        printf("%"PRId64"\n", (int64_t)(second_time - first_time)/60);
	first_time = second_time;
    }
    return 0;
}
