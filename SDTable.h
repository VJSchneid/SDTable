/*
 *      Created on: 17.11.2016
 *      Author: Viktor Schneider
 *      E-Mail: info@vjs.io
 * */

#ifndef SDTABLE_SDTABLE_H
#define SDTABLE_SDTABLE_H

#include <cstdio>
#include <cstdint>
#include <sys/stat.h>

#define SDTABLE_VERSION_1           2
#define SDTABLE_VERSION_2           0
#define HEADER_STATIC_SIZE          26
#define FILE_DEFAULT_BUFFER_SIZE    512

namespace database {
    class SDTable {
    private:
        FILE* file;
        char* fileBuffer;

        enum Frame {CONTENT, FREEDLINE};

        struct {
            // Static Content
            __uint8_t   version1;
            __uint8_t   version2;
            __uint32_t  headerSize;
            __uint32_t  elementCount;
            __uint32_t  lineSize;
            __uint32_t  lineCount;
            __uint32_t  bodySize;
            __uint32_t  freedLineCount;
            // Dynamic Content
            __uint32_t* elementSize;
        } head;

        void setFilePos(unsigned int line, Frame frame = CONTENT, unsigned int offset = 0);

        void flushHead();
        bool writeHead();
        bool readHead();
        void setHead(__uint32_t elementCount, __uint32_t lineCount, __uint32_t freedLineCount,
                     __uint32_t* elementSize);
        int checkHead();

        int requestLine();
        bool removeLine(unsigned int line);
        bool freedLine(__uint32_t line);
        bool checkFreed(unsigned int line);

    public:
        SDTable();
        SDTable(const char* path, unsigned int bufSize = FILE_DEFAULT_BUFFER_SIZE);
        ~SDTable();

        int create(const char* path, unsigned int elementCount, unsigned int* elementSize);
        int open(const char* path, unsigned int bufSize = FILE_DEFAULT_BUFFER_SIZE);
        void close();
        // Return value of -1 means an error occurred
        int addLine(void* container);
        bool clearLine(unsigned int line);
        bool getElement(unsigned int line, unsigned int element, void* container);
    };
}


#endif //SDTABLE_SDTABLE_H
