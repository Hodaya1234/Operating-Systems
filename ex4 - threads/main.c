#include <stdio.h>
#include <unistd.h>
#include "threadPool.h"
#include "osqueue.h"


void hello (void* a)
{
  printf("\n*Task*\n\n");
}


void test_thread_pool_sanity()
{
  int num_threads = 4;
  int i;

  ThreadPool* tp = tpCreate(num_threads);

  for(i=0; i<num_threads; ++i)
  {
    tpInsertTask(tp,hello,NULL);
  }

  //sleep(1);
  tpDestroy(tp,0);
}




int main() {
  test_thread_pool_sanity();

  return 0;
}


