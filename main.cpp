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

    sdtable.clearLine(3);
    sdtable.clearLine(3);
    sdtable.clearLine(4);
    sdtable.clearLine(0);
    rValue = sdtable.addLine(buffer);
    rValue = sdtable.addLine(buffer);
    rValue = sdtable.addLine(buffer);
    rValue = sdtable.addLine(buffer);
    rValue = sdtable.addLine(buffer);
    rValue = sdtable.addLine(buffer);

    sdtable.clearLine(3);
    rValue = sdtable.addLine(buffer);
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