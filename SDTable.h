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
#include <vector>

#define SDTABLE_VERSION_1           3
#define SDTABLE_VERSION_2           0
#define FILE_DEFAULT_BUFFER_SIZE    256

namespace database {

    struct Element {
        uint32_t id;
        uint32_t size;

        bool operator==(const Element &rhs) const;
    };

    class SDTable {
    private:
        FILE* file;
        char* fileBuffer;

        enum Frame {CONTENT, FREEDLINE};

        struct StaticHead {
            uint16_t  version1 = 0;
            uint16_t  version2 = 0;
            uint32_t  headerSize = 0;
            uint32_t  elementCount = 0;
            uint32_t  lineSize = 0;
            uint64_t  bodySize = 0;
            uint32_t  lineCount = 0;
            uint32_t  freedLineCount = 0;
        };

        struct Head: StaticHead {
            std::vector<Element> elements;
        } head;

        void setFilePos(unsigned int line, Frame frame = CONTENT,
                        unsigned int offset = 0) const;

        bool writeHead() const;
        bool readHead();
        void setHead(uint32_t lineCount, uint32_t freedLineCount,
                     std::vector<Element> elements);
        int checkHead() const;
        // @warning call only if FD is opened
        int requestLine();
        bool removeLine(unsigned int line);
        bool freedLine(uint32_t line);
        bool checkFreed(unsigned int line) const;

      public:
        SDTable();
        SDTable(const char* path, unsigned int bufSize = FILE_DEFAULT_BUFFER_SIZE);
        ~SDTable();

        int create(const char* path, std::vector<Element> elements,
                   unsigned int bufSize = FILE_DEFAULT_BUFFER_SIZE);
        int open(const char* path, unsigned int bufSize = FILE_DEFAULT_BUFFER_SIZE);
        void close();

        // Return value of -1 means an error occurred
        int addLine(const void* container);
        int addLine();
        bool clearLine(unsigned int line);

        bool getElement(unsigned int line, unsigned int element,
                        void *container) const;
        bool setElement(unsigned int line, unsigned int element,
                        const void *container) const;

        bool setLine(unsigned int line, const void *container) const;
        bool getLine(unsigned int line, void *container) const;

        bool isFreed(unsigned int line) const;

        // HEADER QUERY INTERFACE
        unsigned short getVersion1() const;
        unsigned short getVersion2() const;
        unsigned int getHeaderSize() const;
        unsigned int getElementCount() const;
        unsigned int getLineSize() const;
        unsigned int getLineCount() const;
        unsigned long getBodySize() const;
        unsigned int getFreedLineCount() const;
        bool getElement(unsigned int no, Element *element) const;
        unsigned int getElementSize(unsigned int no) const;
        const std::vector<Element> &getElements() const;
    };
}

#endif //SDTABLE_SDTABLE_H
