#include "ComandReader.h"
#include <iostream>
#include <Poco/Thread.h>
#include <Poco/CountingStream.h>
#include <sys/select.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>



void nonblock(int state){
    const int NB_ENABLE=0;
    const int NB_DISABLE=1;
    struct termios ttystate;

    //get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);

    if (state==NB_ENABLE)
    {
        //turn off canonical mode
        ttystate.c_lflag &= ~ICANON;
        //minimum of number input read.
        ttystate.c_cc[VMIN] = 1;
    }
    else if (state==NB_DISABLE)
    {
        //turn on canonical mode
        ttystate.c_lflag |= ICANON;
    }
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);

}
int kbhit()
{
    // timeout structure passed into select
    struct timeval tv;
    // fd_set passed into select
    fd_set fds;
    // Set up the timeout.  here we can wait for 1 second
    tv.tv_sec = 0;
    tv.tv_usec = 200000;

    // Zero out the fd_set - make sure it's pristine
    FD_ZERO(&fds);
    // Set the FD that we want to read
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    // select takes the last file descriptor value + 1 in the fdset to check,
    // the fdset for reads, writes, and errors.  We are only passing in reads.
    // the last parameter is the timeout.  select will return if an FD is ready or
    // the timeout has occurred
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    // return 0 if STDIN is not ready to be read.
    return FD_ISSET(STDIN_FILENO, &fds);
}

ComandReader::ComandReader():m_data(),m_act(this,&ComandReader::ProcInput){

}

void ComandReader::ProcInput(){
    nonblock(0);
    while(m_act.isRunning()){
        std::string buf(READ_SIZE,' ');
        if (kbhit()==0){
            continue;
        }
        std::cin>>buf;
        std::cout<<"availabel size:"<<buf.size()<<std::endl;
        if (buf.size()!=0){
               ProcData(buf);
        }
    }
    nonblock(1);
}

void ComandReader::ProcData(const std::string &data){
    std::cout<<data<<std::endl;
}

bool ComandReader::Start(){
    if (m_act.isRunning()) return false;

    ClearData();
    m_act.start();
    return true;
}
#include <Poco/Util/Application.h>

bool ComandReader::Stop(){
    if(m_act.isStopped()) return false;
    try{
        //const auto stop_symbol='\0';
        //std::cin.putback(stop_symbol);
        m_act.wait();
        std::cout<<"good finish!"<<std::endl;
    }
    catch(const Poco::TimeoutException& e){
        Poco::Util::Application::instance().logger().log(e);
    }

    return true;
}

void ComandReader::ClearData(){
    m_data.clear();
}
