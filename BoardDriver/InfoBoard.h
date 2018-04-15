#ifndef INFOBOARD_H
#define INFOBOARD_H
#include "DisplayModule.h"
#include <vector>
#include <string>
#include <list>

typedef unsigned int uint;
namespace RS232 {
    class Serial;
}
class InfoBoard{
public:

    typedef std::vector<byte> AddressList;
    InfoBoard(const AddressList& list,RS232::Serial& device);
    ~InfoBoard();

    void LoadData(const std::string& fname);
    bool ShowNextFrame();
    bool ShiftFrame();

    void ClearCursor();
private:
    std::string m_data;
    int m_cursor;
    bool m_data_end;
    std::list<DisplayModule::SPtr> m_modules;
    DisplayModule m_broadcast;
    RS232::Serial& m_serial;

    std::string GetSubFrame();
    void MoveCursor(const uint size, int& cur_pos, int& next_pos);
    bool DataIsEnd() const{
        return m_data_end;
    }
    std::list<uint> GetSubCols();

    typedef std::list<std::string> StringList;

    void Process(const StringList& list);
};


void Print(const std::string& s);
#endif // INFOBOARD_H
