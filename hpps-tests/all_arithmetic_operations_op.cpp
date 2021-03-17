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

  __attribute__((annotate("range 0 9999809091"))) double res[N];

  for(size_t r_=0;r_<N;r_++){
    res[r_] = std::fmod((N * N),modulo);
  }

  __attribute__((annotate("range 0 5000"))) double temp = res[0];

  /*Iterate until approximately X second has elapsed.*/
  while(time_span.count()<=seconds){
    count += 1;

    for (int j = 0; j < N; j++) {
      res[j] = temp;
    }

    std::cout<<"1) INIT = "<<(res[0])<<std::endl;

    for (int t = 0; t < T; t++) {
      for (int j = 0; j < N; j++) {
        res[j] = N * (j - (N / (N + t)));
      }
    }

    std::cout<<"2) MULTIPLY[0] = "<<(res[0])<<std::endl;

    std::cout<<"3) MULTIPLY[N-1] = "<<(res[N-1])<<std::endl;

    for (int t = 0; t < T; t++) {
      for (int j = 0; j < N; j++) {
        res[j] = N + (j - (N / (N + t)));
      }
    }

    std::cout<<"4) ADD[0] = "<<(res[0])<<std::endl;

    ///
    for (int t = 0; t < T; t++) {
      for (int j = 0; j < N; j++) {
        res[j] = N - (j - (N / (N + t)));
      }
    }

    std::cout<<"5) SUB[0] = "<<(res[0])<<std::endl;

    for (int t = 0; t < T; t++) {
      for (int j = 0; j < N; j++) {
        res[j] = N / (j + ((N + t) / N));
      }
    }

    std::cout<<"6) DIV[0] = "<<(res[0])<<std::endl;

    t2 = steady_clock::now();
    time_span = duration_cast<duration<double>>(t2 - t1);
  }

  std::cout<<"Iterations = "<<(count)<<std::endl;
  std::cout<<"Memory Usage: "<<(getMemoryUsage())<<std::endl;
}
