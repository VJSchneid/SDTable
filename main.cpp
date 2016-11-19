#include <unistd.h>
#include "SDTable.h"
#include "sys/time.h"

int main() {
    database::SDTable sdtable;
    int rValue;
    unsigned int eSize[3] = {4, 2, 4};
    rValue = sdtable.open("test.file");
    if (rValue) {
        return rValue + 10;
    }
    char buffer[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    char buffer2[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    rValue = sdtable.addLine(buffer);
    //rValue = sdtable.getLine(0, buffer2);

    timeval start, end;
    double time;
    gettimeofday(&start, NULL);

    for (long x = 20000000; x > 0; x--) {
    }

    gettimeofday(&end, NULL);

    time = ((end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000);

    printf("%d lines per second\n", (int)(20000000/time));

    return rValue;
}