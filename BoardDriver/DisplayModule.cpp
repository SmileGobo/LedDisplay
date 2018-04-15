#include "DisplayModule.h"
#include <stdexcept>
DisplayModule::DisplayModule(const byte addr):
    m_addr(addr)
{
}

byte DisplayModule::GetAddress() const{
    return m_addr;
}

void DisplayModule::SetAddress(const byte addr){
    m_addr = addr;
}

bool DisplayModule::LoadFrame(std::string& cmd, const std::string &frame){
    if (frame.size() > SIZE ){
        throw std::logic_error("Error in DisplayModule::LoadFrame(...): data too big");
    }
    const auto addr = this->GetAddress();
    cmd.clear();
    if (0 == addr){
        return false;
    }

    cmd = this->MakeCommand(LOAD_FRAME, frame);
    return true;
}

bool DisplayModule::ShowFrame(std::string& cmd){
    cmd.clear();
    if (0 != this ->GetAddress()){
        return false;
    }

    cmd=this->MakeCommand(SHOW_FRAME);
    return true;
}

bool DisplayModule::LoadColumn(std::string &cmd, const byte col){
    cmd.clear();
    if (this->GetAddress() == 0){
        return false;
    }
    cmd = this->MakeCommand(LOAD_COL, std::string(1,col));
    return true;
}

bool DisplayModule::ShiftAndShow(std::string &cmd){
    cmd.clear();
    if (this->GetAddress() != 0){
        return false;
    }
    cmd = this->MakeCommand(SHIFT_AND_SHOW);
    return true;
}

const std::string DisplayModule::MakeCommand(const DisplayModule::CommandTypes type, const std::string &data){
    const auto MIN_PACK_SIZE = 2;
    const auto data_size = data.size();
    const auto tottal_size = ((0 == data_size)? MIN_PACK_SIZE : MIN_PACK_SIZE+1+data_size);
    const auto addr = this->GetAddress();


    std::string cmd(tottal_size, ' ');
    cmd[POS_START] = START_SYMBOL;
    cmd[POS_ADDR_CMD] = this->make_sb(addr,type);
    if (0 != data_size){
        cmd[POS_SIZE] = tottal_size;
        std::copy(data.begin(), data.end(), cmd.begin() + POS_DATA);
    }
    return cmd;
}


