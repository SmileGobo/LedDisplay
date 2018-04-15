#include <Poco/Util/ServerApplication.h>
#include <memory>
#include "ComandReader.h"
class DataBaseReaderApp:public Poco::Util::ServerApplication{
public:
    DataBaseReaderApp();
    //~DataBaseReaderApp();
protected:
    void initialize(Application& self);


    void uninitialize();

    void defineOptions(Poco::Util::OptionSet& options);

    const std::string GetDBPath() const;
    bool SetDBPath(const std::string& path);

    int main(const std::vector < std::string > & args);
private:
    std::string m_dbpath;
    void SetDBPath(const std::string& name, const std::string& value);
};

//typedef std::unique_ptr<Poco::Util::Application> AppUPtr;
int main(int argc,char * argv[]){
    DataBaseReaderApp app;
    //return app.run(argc,argv);
    std::string data;
    for(;;){
        std::cin>>data;
        std::cout<<"you write next row:"<<data<<std::endl;
    }
    return 0;
}



DataBaseReaderApp::DataBaseReaderApp():
    Poco::Util::ServerApplication(),m_dbpath(".")
{

}

void DataBaseReaderApp::initialize(Poco::Util::Application& ){
    Poco::Util::ServerApplication::initialized();
    this->logger().notice("App start!");
}

void DataBaseReaderApp::uninitialize(){
    Poco::Util::ServerApplication::uninitialize();
    this->logger().notice("App is done!");
}

void DataBaseReaderApp::defineOptions(Poco::Util::OptionSet &options){
    Poco::Util::ServerApplication::defineOptions(options);
    Poco::Util::Option opt("db_path", "db", "spec database dir");
    this->logger().notice("define options!");
    opt.required(true);
    opt.repeatable(false);
    opt.argument("name=value");

    Poco::Util::OptionCallback<DataBaseReaderApp> clbk(this, &DataBaseReaderApp::SetDBPath);
    opt.callback(clbk);

    options.addOption(opt);
}

bool DataBaseReaderApp::SetDBPath(const std::string &path){
    return true;
}

int DataBaseReaderApp::main(const std::vector<std::string> &args){
    this->logger().notice("work...");
    ComandReader reader;
    reader.Start();
    /*for(;;){
        Poco::Thread::sleep(1);
    }*/
    waitForTerminationRequest();
    reader.Stop();
    return 0;
}

void DataBaseReaderApp::SetDBPath(const std::string &name, const std::string &value){
    auto& lgr=this->logger();
    lgr.notice(name+":"+value);
    if(!SetDBPath(value)){
        lgr.error("Path not valid!");
        stopOptionsProcessing();
    }
}
