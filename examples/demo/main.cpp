#include <BinCmdParser.h>

void cmdCallback(const CmdData &msg) {
    Serial1.write(msg.data, msg.dataSize);
}

uint8_t cmdHeader[2] = {0xFF,0xF1};
BinCmdParser dataCmd(cmdHeader,
                        2, 10,
                        (void *) &cmdCallback);
void setup()
{
    Serial1.begin(115200);
}

void loop(){
    int size = Serial1->available();
    if (size > 0) {
        uint8_t msg[size];
        Serial1->readBytes(msg, size);
        dataCmd.addData(msg, size);
    }
}