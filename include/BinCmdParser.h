//
// Created by vell0 on 2022/4/15.
//

#ifndef ZGMCU_ARDUINO_BINCMDPARSER_H
#define ZGMCU_ARDUINO_BINCMDPARSER_H

#include <cstdint>
#include <cstring>
#include <malloc.h>

#ifndef LOG
#define LOG(...)
#endif

typedef struct CommandData {
    uint8_t *cmdData = nullptr; // 输出命令
    uint32_t cmdDataSize = 0; // 命令长度
    uint8_t *data = nullptr; // 数据段指针
    uint32_t dataSize = 0;
} CmdData;


uint32_t ChecksumAdd(const uint8_t *a, uint32_t len);

/**
 * 二进制命令解析器，支持自定义包头字节数、【数据长度字段】字节数、校验和字节数
 */
class BinCmdParser {
private:
    const uint8_t *header;
    uint32_t headerLen;
    uint32_t dataSizeLen = 2; // 数据长度字段的长度
    uint32_t maxDataSize = 1024; // 最大数据字节数
    uint32_t checksumDataLen = 1; // 数据长度字段的长度

    void *callbackFunc = nullptr;

    uint8_t *tmpDataOriginPtr = nullptr; // 原始指针地址，用于free
    uint8_t *tmpDataArray = nullptr; // 还未解析的数据地址
    uint32_t tmpDataArraySize = 0;

    CmdData cmd;

    int32_t parseCmd();

    void parse();

    void removeData(uint32_t size);

public:
    BinCmdParser(const uint8_t *header, uint32_t headerLen, uint32_t dataSizeLen, void *callbackFunc);

    /**
     * 创建一个命令解析器
     *
     * @param header 命令头
     * @param headerLen 命令头字节数
     * @param dataSizeLen 数据大小字段字节数，支持最大4字节
     * @param maxDataSize 最大数据字节数
     * @param checksumDataLen 校验字段长度，0为不校验，支持最大4字节
     * @param callbackFunc 回调函数, void cmdCallback(const CommandData &msg);
     */
    BinCmdParser(const uint8_t *header, uint32_t headerLen, uint32_t dataSizeLen, uint32_t maxDataSize,
                 uint32_t checksumDataLen,
                 void *callbackFunc);

    void addData(const uint8_t *dataArray, uint32_t len);
};


#endif //ZGMCU_ARDUINO_BINCMDPARSER_H
