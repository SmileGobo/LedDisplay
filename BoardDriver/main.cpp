#include <Poco/Util/ServerApplication.h>
#include <Poco/Dynamic/Var.h>
#include <memory>
#include <functional>
#include <map>
#include <string>
class BoardDriver:public Poco::Util::ServerApplication{
public:
    BoardDriver();
    //~DataBaseReaderApp();
protected:
    void initialize(Application& self);


    void uninitialize();

    void defineOptions(Poco::Util::OptionSet& options);

    const std::string GetDBPath() const;

    int main(const std::vector < std::string > & args);
private:
    void ParseOptions(const std::string& name, const std::string& value);

    typedef std::function <void (const std::string& )> OptHandler;
    typedef std::map<std::string, OptHandler> OptHandlers;
    OptHandlers m_handlers;
    void RegisterOptHandler(const std::string& name, OptHandler hnd);
    bool m_init;

    std::string m_playfile;
    std::string m_devname;
    uint m_delay;
};

//typedef std::unique_ptr<Poco::Util::Application> AppUPtr;
int main(int argc,char * argv[]){

    BoardDriver app;
    return app.run(argc,argv);
    //return 0;
}



BoardDriver::BoardDriver():
    Poco::Util::ServerApplication(),
    m_handlers(),
    m_init(false), m_playfile(),
    m_devname(), m_delay(1000)
{
    this->RegisterOptHandler("devname",[this](const std::string& value){
        this->m_devname = value;
    });

    this->RegisterOptHandler("playfile",[this](const std::string& value){
        this->m_playfile = value;
    });

    this->RegisterOptHandler("pause",[this](const std::string& value){
        Poco::Dynamic::Var val(value);
        this->m_delay = val.convert<uint>();
    });
}

void BoardDriver::initialize(Poco::Util::Application& ){
    Poco::Util::ServerApplication::initialized();
    this->logger().notice("App start!");
}

void BoardDriver::uninitialize(){
    Poco::Util::ServerApplication::uninitialize();
    this->logger().notice("App is done!");
}

void BoardDriver::defineOptions(Poco::Util::OptionSet &options){
    Poco::Util::ServerApplication::defineOptions(options);
    std::list<Poco::Util::Option> opts;

    opts.push_back(Poco::Util::Option("playfile", "pf", "Файл для вопроизведения"));
    opts.push_back(Poco::Util::Option("devname", "dn", "Имя устройства последовательного порта"));
    opts.push_back(Poco::Util::Option("pause", "p", "Задрежка в миллисекундах целое (unsigned int)"));

    for (auto& opt : opts){
        opt.required(true);
        opt.repeatable(false);
        opt.argument("name=value");
        Poco::Util::OptionCallback<BoardDriver> clbk(this, &BoardDriver::ParseOptions);
        opt.callback(clbk);
        options.addOption(opt);
    }
}

#include "InfoBoard.h"
#include <iostream>
#include "RS232.h"
#include <Poco/Thread.h>
int BoardDriver::main(const std::vector<std::string> &args){
    this->logger().notice("work...");

    RS232::Serial port;
    port.Open(m_devname,230400);
    if(!port.IsOpen()){
        return -1;
    }

    //std::string fname ("./data/n2.txt");
    InfoBoard::AddressList list;
    list.push_back(16);
    list.push_back(18);
    list.push_back(20);
    InfoBoard board(list,port);
    board.LoadData(m_playfile);
    while(!board.ShiftFrame()){
        Poco::Thread::sleep(m_delay);
    }

    port.Close();
    return 0;
}

void BoardDriver::ParseOptions(const std::string &name, const std::string& value){
    m_init = true;
    try{
        m_handlers[name](value);
    }
    catch(const std::exception& e){
        m_init = false;
    }
}

void BoardDriver::RegisterOptHandler(const std::string &name, OptHandler hnd){
    m_handlers[name] = hnd;
}

