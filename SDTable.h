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
#define HEADER_STATIC_SIZE          (sizeof(head) - sizeof(head.elementSize))
#define FILE_DEFAULT_BUFFER_SIZE    256

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

namespace database {
    class SDTable {
    private:
        FILE* file;
        char* fileBuffer;

        enum Frame {CONTENT, FREEDLINE};

        struct {
            // Static Content
            __uint16_t  version1;
            __uint16_t  version2;
            __uint32_t  headerSize;
            __uint32_t  elementCount;
            __uint32_t  lineSize;
            __uint64_t  bodySize;
            __uint32_t  lineCount;
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
        // @warning call only if FD is opened
        int requestLine();
        bool removeLine(unsigned int line);
        bool freedLine(__uint32_t line);
        bool checkFreed(unsigned int line);

    public:
        SDTable();
        SDTable(const char* path, unsigned int bufSize = FILE_DEFAULT_BUFFER_SIZE);
        ~SDTable();

        int create(const char* path, unsigned int elementCount, unsigned int* elementSize, unsigned int bufSize = FILE_DEFAULT_BUFFER_SIZE);
        int open(const char* path, unsigned int bufSize = FILE_DEFAULT_BUFFER_SIZE);
        void close();

        // Return value of -1 means an error occurred
        int addLine(void* container);
        bool clearLine(unsigned int line);

        bool getElement(unsigned int line, unsigned int element, void* container);
        bool setElement(unsigned int line, unsigned int element, void* container);

        bool setLine(unsigned int line, void* container);
        bool getLine(unsigned int line, void* container);

        bool isFreed(unsigned int line);

        // HEADER QUERY INTERFACE
        unsigned short  getVersion1();
        unsigned short  getVersion2();
        unsigned int    getHeaderSize();
        unsigned int    getElementCount();
        unsigned int    getLineSize();
        unsigned int    getLineCount();
        unsigned long   getBodySize();
        unsigned int    getFreedLineCount();
        unsigned int    getElementSize(unsigned int element);
    };
}

#pragma clang diagnostic pop

#endif //SDTABLE_SDTABLE_H