#include <stdio.h>
#include <curl/curl.h>
#include "curl/CurlDemo.h"

int main(void) {
    int a = 10;
    int b = 20;
    int sum = a + b;
    printf("Sum: %d\n", sum);
    // t1();
    testPost();

    return 0;
}