#include "fixed_point.hpp"
#include <vector>
#include <chrono>
#include <cmath>
#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>

using namespace std::chrono;
using namespace std;


long getMemoryUsage()
{
  struct rusage usage;
  if(0 == getrusage(RUSAGE_SELF, &usage))
    return usage.ru_maxrss; // bytes
  else
    return 0;
}

int main(int argc,char** argv){

  int N = 100000;
  int T = 10000;
  int modulo = 5000;
  float seconds = 300.0;

  /*How many iterations we did.*/
  size_t count=0;

  /*Initialize timers.*/
  steady_clock::time_point t1 = steady_clock::now();
  steady_clock::time_point t2 = steady_clock::now();
  duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

  /*Iterate until approximately X second has elapsed.*/
  while(time_span.count()<=seconds){
    count+=1;

  fixed_point_t<29,3>    res[N]; // TAFFO:   __attribute__((annotate("range 0 499000000")))

    //res[0] = 1;

    //std::cout<<"1/3 = "<<(res[0]/3)<<std::endl;


    for(size_t r_=0;r_<N;r_++){
      res[r_] = fixed_point_t<29, 3>(std::fmod((N * N),modulo));
    }


    std::cout<<"TEST = "<<(res[0])<<std::endl;

    t2 = steady_clock::now();
    time_span = duration_cast<duration<double>>(t2 - t1);
  }

  std::cout<<"Iterations = "<<(count)<<std::endl;
  std::cout<<"Memory Usage: "<<(getMemoryUsage())<<std::endl;
}
