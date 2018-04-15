#ifndef COMANDREADER_H
#define COMANDREADER_H

#include <Poco/Activity.h>
#include <iostream>


class ComandReader{
public:
    ComandReader();


    bool Start();
    bool Stop();
    void ClearData();
private:
    enum{
        READ_SIZE=10
    };
    std::string m_data;
    Poco::Activity<ComandReader> m_act;

    void ProcInput();
    void ProcData(const std::string& data);

};

#endif // COMMANDREADER_H
