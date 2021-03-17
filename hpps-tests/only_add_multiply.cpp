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

    double res[N];

    for(size_t r_=0;r_<N;r_++){
      res[r_] = std::fmod((N * N),modulo);
    }

    double temp = res[0];

    std::cout<<"1) RES[0] = "<<(res[0])<<std::endl;

    for (int t = 0; t < T; t++) {
      for (int j = 0; j < (N - 1); j++) {
        res[j] = N * (res[j+1] - 1.0001);
      }

      if(t != (T-1)) {
        for (int j = 0; j< (N - 1); j++) {
          res[j] = temp;
        }
      }
    }

    std::cout<<"2) RES[0] = "<<(res[0])<<std::endl;

    std::cout<<"3) RES[N-1] = "<<(res[N-2])<<std::endl;

    for (int t = 0; t < T; t++) {
      for (int j = 0; j < (N - 1); j++) {
        res[j] = N + (res[j+1] - 1.0001);
      }

      if(t != (T-1)) {
        for (int j = 0; j < (N - 1); j++) {
          res[j] = temp;
        }
      }
    }

    std::cout<<"4) RES[0] = "<<(res[0])<<std::endl;

    t2 = steady_clock::now();
    time_span = duration_cast<duration<double>>(t2 - t1);
  }

  std::cout<<"Iterations = "<<(count)<<std::endl;
  std::cout<<"Memory Usage: "<<(getMemoryUsage())<<std::endl;
}
