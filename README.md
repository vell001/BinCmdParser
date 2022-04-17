# BinCmdParser
binary command parser, with dynamic header size, dynamic data size, dynamic checksum size

Cmd format : {header,dataSize,data,checksum}

```c++
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
```