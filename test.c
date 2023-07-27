#include <stdio.h>
// #include <poll.h>
#include "src/str.h"
#include <sys/time.h>

int main(void) {
    char *str = "Example test string";

    // struct timeval stop, start;
    // gettimeofday(&start, NULL);
    // for (int i = 0; i < 1000 * 1000; i++) {
    //     replace(str, 8, 4, "hello");
    // }
    // gettimeofday(&stop, NULL);
    // printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);

    printf("%s\n", str);

    printf("Sub: %s\n", substr(str, 8, 4));
    printf("-Sub: %s\n", substr(str, 12, -4));

    printf("1c: %d\n", findf(str, 't'));
    printf("lc: %d\n", findl(str, 't'));

    printf("Rep: %s\n", replace(str, 8, 4, "hello"));
    printf("-Rep: %s\n", replace(str, 12, -4, "hello"));

    printf("Era: %s\n", erase(str, 8, 4));
    printf("-Era: %s\n", erase(str, 12, -4));

    return 0;
}