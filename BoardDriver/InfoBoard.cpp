#include "InfoBoard.h"
#include "RS232.h"
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <algorithm>
void Print(const std::string& s){
    for (auto symbol : s){
        std::cout<<(int(symbol)&255)<<", ";
    }
    std::cout<<std::endl;
}

InfoBoard::InfoBoard(const InfoBoard::AddressList &list, RS232::Serial& device):
    m_data(), m_cursor(0),
    m_data_end(true), m_modules(),
    m_broadcast(0), m_serial(device)
{
    for(auto addr : list){
        auto module = std::make_shared<DisplayModule>(addr);
        if (module){
            m_modules.push_back(module);
        }
    }

}

InfoBoard::~InfoBoard(){
    m_serial.Close();
}


void InfoBoard::LoadData(const std::string &fname){
    std::fstream file (fname, std::ios::in | std::ios::binary);

    if(!file){
        std::stringstream ss;
        ss<<"Error in InfoBoard::LoadData(): can't open file: "<<fname;
        throw std::runtime_error(ss.str());
    }
    file.seekg (0, file.end);
    auto size = file.tellg();
    file.seekg (0, file.beg);

    //std::cout<<"size"<<size<<std::endl;
    m_data.resize(size);
    file.seekg(0, std::ios::beg);
    if(!file.read(const_cast<char*>(m_data.data()), size)){
        file.close();
        throw std::runtime_error("Error in InfoBoard::LoadData(): fail to read");
    }
    ClearCursor();
}

bool InfoBoard::ShowNextFrame(){
    if (0 == m_data.size()){
        throw std::logic_error("Erroro in InfoBoard::ShowNextFrame(): no data");
    }
    StringList commands;
    std::string cmd;
    for (auto module : m_modules){
        auto frame = this->GetSubFrame();
        //std::cout<<"sub_frame";
        //Print(frame);
        if(module->LoadFrame(cmd,frame)){
            commands.push_back(cmd);
        }
    }

    if (m_broadcast.ShowFrame(cmd)){
        commands.push_back(cmd);
    }

    this->Process(commands);
    return this->DataIsEnd();
}

bool InfoBoard::ShiftFrame(){
    if (0 == m_data.size()){
        throw std::logic_error("Error in InfoBoard::ShiftFrame(): empty data!");
    }
    auto cols = this->GetSubCols();
    StringList commands;
    std::string cmd;
    auto module = m_modules.begin();
    for (const auto col: cols){
        if ((*module)->LoadColumn(cmd,m_data[col])){
            commands.push_back(cmd);
        }
        ++module;
    }
    if (m_broadcast.ShiftAndShow(cmd)){
        commands.push_back(cmd);
    }
    this->Process(commands);
    return this->DataIsEnd();
}

void InfoBoard::ClearCursor(){
    m_cursor = 0;
    m_data_end = false;
}

std::string InfoBoard::GetSubFrame(){
    int start,stop;
    std::string rslt;
    this->MoveCursor(DisplayModule::SIZE,start,stop);
    if (start == stop){
        return rslt;
    }
    rslt.resize(stop-start);
    std::copy(m_data.begin()+start,m_data.begin()+stop,rslt.begin());
    return rslt;
}

void InfoBoard::MoveCursor(const uint size, int &cur_pos, int &next_pos){
    const auto MAX_SIZE = m_data.size();
    if (size >= MAX_SIZE){
        cur_pos = 0;
        next_pos = 0;
        return;
    }
    cur_pos = m_cursor;
    m_cursor +=size;
    if (m_cursor >= MAX_SIZE){
        m_data_end = true;
        m_cursor = MAX_SIZE;
    }
    next_pos = m_cursor;
}

std::list<uint> InfoBoard::GetSubCols(){
    int start,stop;
    std::list<uint> rslt;
    this->MoveCursor(1,start,stop);
    if (start == stop){
        return rslt;
    }

    const auto BACK_OFFSET = DisplayModule::SIZE;
    const auto MAX_POS = m_modules.size();
    for (auto pos_count = 0;pos_count < MAX_POS; pos_count++){
        rslt.push_back(start);
        start -= BACK_OFFSET;
        if (start < 0){
            break;
        }
    }
    std::cout<<"rslt size"<<rslt.size()<<std::endl;
    rslt.reverse();
    return rslt;
}

void InfoBoard::Process(const InfoBoard::StringList &list){
    if (!m_serial.IsOpen()){
        throw std::logic_error("Error in InfoBoard::Process(): port is close!");
    }
    for (auto cmd: list){
        Print(cmd);
        m_serial.SendByteArray((byte*)(cmd.data()), cmd.size());
    }
}
