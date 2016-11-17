#include <unistd.h>
#include "SDTable.h"
#include "sys/time.h"

int main() {
    database::SDTable sdtable;
    int rValue;
    unsigned int eSize[10] = {4, 10, 8, 255, 50659, 448, 4876, 1257, 522322, 554878};
    rValue = sdtable.create("test.file", 10, eSize);

    /*
    timeval start, end;
    double time;
    gettimeofday(&start, NULL);

    for (int x = 100000; x > 0; x--) {

    }

    gettimeofday(&end, NULL);

    time = (end.tv_sec - start.tv_sec) * 10 + (end.tv_usec - start.tv_usec) / 100000;
    printf("Elapsed time: %.3e µs\n", time);*/

    return rValue;
}