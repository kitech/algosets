#ifndef _STDLOG_H_
#define _STDLOG_H_

#include <unistd.h>
#include <sys/syscall.h>
#include <syscall.h>

#include <iostream>

class StdlogBase {
public:
  StdlogBase(std::ostream& _out):out(_out){}
  template<typename T>
  StdlogBase& operator<<(const T& vlog) {log(); out << vlog << " "; return *this;}
protected:
  virtual void log() = 0;
  std::ostream& out;
};

class StdLogger : public StdlogBase {
public:
    StdLogger(std::ostream &_out) : StdlogBase(_out) {

    }
    virtual ~StdLogger() {
        out << " ENDL."<< std::endl;
    }

protected:
  void log() {
      // std::cerr << "Printing" << std::endl;
  }
};

// #define qLogx(log) std::cout<<__FILE__<<" "<<__LINE__<<" "<<__FUNCTION__<<" " log<<std::endl;
#define qLogx() StdLogger(std::cout)<<__FILE__<<__LINE__<<__FUNCTION__<<syscall(__NR_gettid)
// LoggedStream(std::cout) << "log" << "Three" << "times";
#define qErrorx() StdLogger(std::err)<<__FILE__<<__LINE__<<__FUNCTION__<<syscall(__NR_gettid)

#endif
