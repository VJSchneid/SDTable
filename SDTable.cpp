/*
 *      Created on: 17.11.2016
 *      Author: Viktor Schneider
 *      E-Mail: info@vjs.io
 * */

#include "SDTable.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

namespace database {
    SDTable::SDTable() {
        // Set NULL pointers
        file = NULL;
        fileBuffer = NULL;
        head.elementSize = NULL;
        // Flush Head
        flushHead();
    }

    SDTable::SDTable(const char *path, unsigned int bufSize) {
        // First initialisation
        SDTable();
        // Open file
        open(path, bufSize);
    }

    SDTable::~SDTable() {
        // Close file
        close();
    }

    int SDTable::open(const char *path, unsigned int bufSize) {
        // If file opened close it
        close();
        // Open file
        file = fopen(path, "rb+");
        if (!file) {
            return 1;
        }
        // Create buffer
        if (bufSize) {
            fileBuffer = new char[bufSize];
            setvbuf(file, fileBuffer, _IOFBF, bufSize);
        }
        else {
            setvbuf(file, NULL, _IONBF, 0);
        }
        // Load Header
        if (!readHead()) {
            return 2;
        }
        // Check loaded head
        if (checkHead()) {
            return 3;
        }
        return 0;
    }

    void SDTable::close() {
        // Close file
        if (file) {
            fclose(file);
            file = NULL;
        }
        // Remove buffer
        if (fileBuffer) {
            delete[] fileBuffer;
            fileBuffer = NULL;
        }
        // Flush head
        flushHead();
    }

    int SDTable::create(const char *path, unsigned int elementCount, unsigned int *elementSize, unsigned int bufSize) {
        __uint32_t* elementSizeDyn;
        int rValue;
        // If file opened close it
        close();
        // Open file
        file = fopen(path, "wb");
        if (!file) {
            return 1;
        }
        // Disable buffer for file
        setvbuf(file, NULL, _IONBF, 0);
        // Create dynamic content
        elementSizeDyn = new __uint32_t[elementCount];
        for (int x = elementCount - 1; x >= 0; x--) {
            elementSizeDyn[x] = elementSize[x];
        }
        // Save content to head
        setHead(elementCount, 0, 0, elementSizeDyn);
        // Write content to file
        if (!writeHead()) {
            return 2;
        }
        // Reopen file to make reading content possible
        rValue = open(path, bufSize);
        if (rValue) {
            rValue += 2;
        }
        return rValue;
    }

    inline void SDTable::flushHead() {
        // Remove dynamic content from head
        if (head.elementSize) {
            delete[] head.elementSize;
            head.elementSize = 0;
        }
        // Remove static content from head
        for (int x = HEADER_STATIC_SIZE - 1; x >= 0; x--) {
            ((char*)&head)[x] = 0;
        }
    }

    inline bool SDTable::writeHead() {
        if (file) {
            // Set position to beginning of file
            rewind(file);
            // Write static & dynamic content to file
            return fwrite(&head, 1, HEADER_STATIC_SIZE, file) == HEADER_STATIC_SIZE &&
                   fwrite(head.elementSize, 4, head.elementCount, file) == head.elementCount;
        }
        return false;
    }

    inline bool SDTable::readHead() {
        // Flush head, because of deleting dynamic memory
        flushHead();
        // Set position to beginning of file
        rewind(file);
        // Read static content from file
        if (fread(&head, 1, HEADER_STATIC_SIZE, file) != HEADER_STATIC_SIZE) {
            return false;
        }
        // Create dynamic content
        head.elementSize = new __uint32_t[head.elementCount];
        // Read dynamic content from file
        return fread(head.elementSize, 4, head.elementCount, file) == head.elementCount;
    }

    inline int SDTable::checkHead() {
        struct stat statBuf;
        __uint32_t lineSize = 0;
        // Check Version
        if (head.version1 != SDTABLE_VERSION_1) {
            return 1;
        }
        // Check headerSize
        if (head.headerSize != HEADER_STATIC_SIZE + head.elementCount * 4) {
            return 2;
        }
        // Allocate lineSize
        for (int x = head.elementCount - 1; x >= 0; x--) {
            lineSize += head.elementSize[x];
        }
        // Check lineSize
        if (head.lineSize != lineSize) {
            return 3;
        }
        // Check bodySize
        if (head.bodySize != head.lineCount * lineSize) {
            return 4;
        }
        // Get FileSize
        fstat(fileno(file), &statBuf);
        // Check FileSize
        if (statBuf.st_size < head.headerSize + head.bodySize + head.freedLineCount * 4) {
            return 5;
        }
        return 0;
    }

    inline void SDTable::setHead(__uint32_t elementCount, __uint32_t lineCount, __uint32_t freedLineCount,
                          __uint32_t *elementSize) {
        __uint32_t lineSize = 0;
        // Allocate some information
        // LineSize:
        for (int x = elementCount - 1; x >= 0; x--) {
            lineSize += elementSize[x];
        }
        // Flush current head, because of deleting dynamic memory
        flushHead();
        // Store content
        head.version1       = SDTABLE_VERSION_1;
        head.version2       = SDTABLE_VERSION_2;
        head.headerSize     = HEADER_STATIC_SIZE + elementCount * 4;
        head.elementCount   = elementCount;
        head.lineSize       = lineSize;
        head.lineCount      = lineCount;
        head.bodySize       = lineCount * lineSize;
        head.freedLineCount = freedLineCount;
        head.elementSize    = elementSize;
    }

    int SDTable::addLine(void *container) {
        if (file) {
            // Request line to store content
            int line = requestLine();
            if (line == -1) {
                return -1;
            }
            // Save line
            setFilePos((unsigned int)line);
            if (fwrite(container, 1, head.lineSize, file) != head.lineSize) {
                removeLine((unsigned int) line);
                return -1;
            }
            // Save Head
            if (!writeHead()) {
                return -1;
            }
            return line;
        }
        return -1;
    }

    inline int SDTable::requestLine() {
        if (head.freedLineCount) {
            __uint32_t line;
            head.freedLineCount--;
            setFilePos(head.freedLineCount, FREEDLINE);
            // If an error occur return -1
            if (fread(&line, 4, 1, file) != 1) {
                return -1;
            }
            return (int) line;
        }
        else {
            head.bodySize += head.lineSize;
            head.lineCount++;
            return head.lineCount - 1;
        }
    }

    inline void SDTable::setFilePos(unsigned int line, SDTable::Frame frame, unsigned int offset) {
        if (file) {
            // Allocate and set file position
            switch (frame) {
                case CONTENT:
                    fseek(file, head.headerSize + head.lineSize * line + offset, SEEK_SET);
                    return;
                case FREEDLINE:
                    fseek(file, head.headerSize + head.bodySize + 4 * line + offset, SEEK_SET);
                    return;
            }
        }
    }

    inline bool SDTable::removeLine(unsigned int line) {
        if (line >= head.lineCount) {
            return false;
        }
        // Check if line is the last content in file
        if (head.freedLineCount || line < head.lineCount - 1) {
            return checkFreed(line) || freedLine(line);
        }
        else {
            // Line is last content in file
            // TODO Override content with zeros
            head.lineCount--;
            head.bodySize -= head.lineSize;
            return true;
        }
    }

    inline bool SDTable::freedLine(__uint32_t line) {
        char cache = 0;
        // Clear all chars in line
        setFilePos(line);
        for (int x = head.lineSize - 1; x >= 0; x--) {
            if (fwrite(&cache, 1, 1, file) != 1) {
                return false;
            }
        }
        // Mark as FreedLine in Footer
        setFilePos(head.freedLineCount, FREEDLINE);
        if (fwrite(&line, 4, 1, file) != 1) {
            return false;
        }
        head.freedLineCount++;
        return true;
    }

    bool SDTable::clearLine(unsigned int line) {
        return removeLine(line) && writeHead();
    }

    inline bool SDTable::checkFreed(unsigned int line) {
        // Check freed state
        uint32_t refLine;
        setFilePos(0, FREEDLINE);
        for (int x = head.freedLineCount - 1; x >= 0; x--) {
            if (fread(&refLine, 4, 1, file) != 1) {
                return false;
            }
            if (refLine == line) {
                return true;
            }
        }
        return false;
    }

    bool SDTable::getElement(unsigned int line, unsigned int element, void* container) {
        unsigned int offset = 0;
        if (element >= head.elementCount || line >= head.lineCount) {
            return false;
        }
        // Allocate offset
        for (int x = element - 1; x >= 0; x--) {
            offset += head.elementSize[x];
        }
        // Set file position
        setFilePos(line, CONTENT, offset);
        // Read content from file
        return fread(container, 1, head.elementSize[element], file) == head.elementSize[element];
    }

    bool SDTable::setElement(unsigned int line, unsigned int element, void *container) {
        unsigned int offset = 0;
        if (element >= head.elementCount || line >= head.lineCount) {
            return false;
        }
        // Allocate offset
        for (int x = element - 1; x >= 0; x--) {
            offset += head.elementSize[x];
        }
        // Set file position
        setFilePos(line, CONTENT, offset);
        // Write content to file
        return fwrite(container, 1, head.elementSize[element], file) == head.elementSize[element];
    }

    bool SDTable::getLine(unsigned int line, void *container) {
        if (line >= head.lineCount) {
            return false;
        }
        setFilePos(line);
        return fread(container, 1, head.lineSize, file) == head.lineSize;
    }

    bool SDTable::setLine(unsigned int line, void *container) {
        if (line >= head.lineCount) {
            return false;
        }
        setFilePos(line);
        return fwrite(container, 1, head.lineSize, file) == head.lineSize;
    }

    bool SDTable::isFreed(unsigned int line) {
        return checkFreed(line);
    }

    unsigned short SDTable::getVersion1() {
        return (unsigned short) head.version1;
    }

    unsigned short SDTable::getVersion2() {
        return (unsigned short) head.version2;
    }

    unsigned int SDTable::getHeaderSize() {
        return (unsigned int) head.headerSize;
    }

    unsigned int SDTable::getElementCount() {
        return (unsigned int) head.elementCount;
    }

    unsigned int SDTable::getLineSize() {
        return (unsigned int) head.lineSize;
    }

    unsigned int SDTable::getLineCount() {
        return (unsigned int) head.lineCount;
    }

    unsigned long SDTable::getBodySize() {
        return (unsigned long) head.bodySize;
    }

    unsigned int SDTable::getFreedLineCount() {
        return (unsigned int) head.freedLineCount;
    }

    unsigned int SDTable::getElementSize(unsigned int element) {
        if (element >= head.elementCount) {
            return 0;
        }
        return head.elementSize[element];
    }

}

#pragma clang diagnostic pop