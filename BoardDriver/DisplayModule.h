#ifndef DISPLAYMODULE_H
#define DISPLAYMODULE_H
//#include <Serial/Serial.h>
#include <string>
#include <memory>
typedef unsigned char byte;
class DisplayModule{
public:
    DisplayModule(const byte addr);
    enum {SIZE = 32};

    byte GetAddress() const;
    void SetAddress(const byte addr);

    bool LoadFrame(std::string& cmd, const std::string& frame);
    bool ShowFrame(std::string& cmd);
    bool LoadColumn(std::string& cmd, const byte col);
    bool ShiftAndShow(std::string& cmd);

    typedef std::shared_ptr<DisplayModule> SPtr;
private:
    byte m_addr;
    //Serial& m_serial;
    enum {
        MAX_DATA_LEN = 32,
        START_SYMBOL = 0x55
    };

    enum CommandTypes{
        SHOW_FRAME = 0x01,
        SHIFT_AND_SHOW = 0x02,
        LOAD_COL = 0x02,
        LOAD_FRAME = 0x03,
        SET_BRIGH = 0x04
    };

    enum {
        POS_START = 0,
        POS_ADDR_CMD = 1,
        POS_SIZE = 2,
        POS_DATA = 3
    };

    const std::string MakeCommand(const CommandTypes type, const std::string& data = "");
    byte make_sb(const byte addr, const CommandTypes type){
        return type|(addr << 3);
    }
};

#endif // DISPLAYMODULE_H
