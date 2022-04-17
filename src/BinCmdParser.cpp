//
// Created by vell0 on 2022/4/15.
//

#include "BinCmdParser.h"

uint32_t ChecksumAdd(const uint8_t *a, uint32_t len) {
    uint32_t i, sum = 0;
    for (i = 0; i < len; i++) {
        sum += a[i];
    }
    return sum;
}

int32_t BinCmdParser::parseCmd() {
    if (tmpDataArraySize < headerLen + dataSizeLen) {
        return 0; // 数据不完整，等待下一次数据
    }
    // 解析头部信息
    for (uint32_t i = 0; i < headerLen; i++) {
        if (tmpDataArray[i] != header[i]) {
#if PRINT_CMD_DEBUG
            LOG("header error %X != %X", tmpDataArray[i], header[i]);
#endif
            // 头部信息对不齐
            return -1;
        }
    }
    // 拿到DataSize
    for (uint32_t i = 0; i < dataSizeLen; i++) {
        uint8_t inData = tmpDataArray[headerLen + i];
        cmd.dataSize |= ((0xff & inData) << (8 * i));
    }
    if (cmd.dataSize > maxDataSize) {
        // 超过最大值
        return -2;
    }
    cmd.cmdData = tmpDataArray;
    cmd.data = tmpDataArray + headerLen + dataSizeLen;
    cmd.cmdDataSize = headerLen + dataSizeLen + cmd.dataSize + checksumDataLen;
    if (tmpDataArraySize < cmd.cmdDataSize) {
        return 0; // 数据不完整，等待下一次数据
    }

    if (checksumDataLen <= 0) {
        return 1; // 不做校验，直接返回
    }
    // 校验和
    uint32_t checksumSize = headerLen + dataSizeLen + cmd.dataSize;
    uint32_t sum = ChecksumAdd(cmd.cmdData, checksumSize);
    auto *sumBytes = (uint8_t *) &sum;
    for (uint32_t i = 0; i < checksumDataLen; i++) {
        uint8_t a = sumBytes[i];
        uint8_t b = tmpDataArray[checksumSize + i];
        if (a != b) {
#if PRINT_CMD_DEBUG
            LOG("ParseCommand checksum error idx %lu sum:%X recv:%X\r\n", i, a, b);
#endif
            return -3; // 校验失败
        }
    }
    // 校验通过
    return 1;
}

void BinCmdParser::parse() {
    int32_t code;
    while (tmpDataArraySize > 0) {
        code = parseCmd();
#if PRINT_CMD_DEBUG
        LOG("parse tmpSize: %d code: %d\r\n", tmpDataArraySize, code);
#endif
        if (code < 0) {
            // 解析失败
            removeData(1);
        } else if (code > 0) {
            // 解析成功
            if (callbackFunc != nullptr) {
                // 回调函数
                ((void (*)(CmdData &)) callbackFunc)(cmd);
            }
            removeData(cmd.cmdDataSize);
        } else {
            // 数据不完整
            break;
        }
    }
}

void BinCmdParser::removeData(uint32_t size) {
    if (size >= tmpDataArraySize) {
        tmpDataArraySize = 0;
        tmpDataArray = nullptr;
        free(tmpDataOriginPtr);
        tmpDataOriginPtr = nullptr;
    } else {
        tmpDataArray += size;
        tmpDataArraySize -= size;
    }
}

BinCmdParser::BinCmdParser(const uint8_t *header, uint32_t headerLen, uint32_t dataSizeLen, void *callbackFunc)
        : header(header), headerLen(headerLen),
          dataSizeLen(dataSizeLen), callbackFunc(callbackFunc) {
}

BinCmdParser::BinCmdParser(const uint8_t *header, uint32_t headerLen, uint32_t dataSizeLen, uint32_t maxDataSize,
                           uint32_t checksumDataLen, void *callbackFunc)
        : header(header), headerLen(headerLen),
          dataSizeLen(dataSizeLen), maxDataSize(maxDataSize),
          checksumDataLen(checksumDataLen), callbackFunc(callbackFunc) {
}

void BinCmdParser::addData(const uint8_t *dataArray, uint32_t len) {
    uint32_t oldSize = tmpDataArraySize;
    uint32_t newSize = oldSize + len;
    auto *newData = (uint8_t *) malloc(newSize);
    // 将旧的数据加进来
    if (oldSize > 0 && tmpDataArray != nullptr) {
        memcpy(newData, tmpDataArray, oldSize);
        free(tmpDataOriginPtr);
    }
    // 将新数据加进来
    memcpy(newData + oldSize, dataArray, len);
    tmpDataArray = newData;
    tmpDataOriginPtr = newData;
    tmpDataArraySize = newSize;

    parse();
}
