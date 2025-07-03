/* In the naive way of solving this problem we have used the data parallel way of 
solving the problem - where the orchestracton problem is given to the ispc to 
control it - here is adiffernet way of solving the problem

Lets dive a but more deep to understand the working of the solver

 In the previous case we have just state teh parallel work in the for_all and let 
 the ispc take care of the orchestraction of the independent work - on the threads 
- but here we are going to orchestracte the work to each of the thread */

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// GLOABL VARIABLES
int n; // size of the grid 
bool done = false; // flag to check for the convergence of the grid 
float diff = 0.0; // global diff variable of the solver - this is the diff that decides the convergence of the grid - and weather to proceed with next iteration 
// SHARED ADDDRESS SPACE - shared space that accumulate the results of each of the thread
LOCK lock;
BARRIER barrier;
int num_workers = 10; // total number of cores in the system

float* allocate(int a,int b){
  // return a random grid of size a x b
  return new float[a * b];
}

int getThreadId(){
  // Pseud fucntion to get the thread id of the current thread 
  return 5;
}


float *A = allocate(n+2, n+2);

void solve(float* A){ // The function itself is considered as a SPMD - and execute assigned number of computations on this thread
  // The idea is to spawn multiple threads - and each thread compute the difference on its own execution context.
  // The issie is how are we gonna accumulate each threads diff value to the global diff ? - using a global diff - but there could be a race condition - so this is where we need a lock
  float myDiff; // initialise the local diff for each thread
  int threadID = getThreadId(); // this a thread that is spwaned to do the parallel work of the solver - and we need to assign some unique work to each of the thread
  // How and what work to assign this thread to ?
  int myMin = 1 + (threadID) * (n / num_workers); // min for the thread to process - start from the row - with the threadID *  [(n / numworkers) - this is the work assigned to each thread]
  int myMax = myMin + (n / num_workers); // this is the work done by each thread 

  while(!done){
    float myDiff = 0.0; // inintalise the local diff with null;
    diff = 0.0; // initialise the global diff with null

    /* ALERT BARRIER - All threds have to wait here unitl all the thredsa have completed their 
    processing here - as the diff comparision is happening at thread level, and if either thread is 
    fater than another, then the faster thread will start yhe loop and update the my diff and the
    diff if it is super fast , and teh slower thread will reset the diff value to 0 - which is wrong 
    so */
    barrier.wait(my_barrier, NUM_PROCESSORS); // NUM_PROCESSOR - is the number of participants taht the barrier is waiting for.

    for(j=myMin to myMax){
      for(i = red_cells in this row){ // only process red cells in this row - as we go red / black cell wise 
        float prev = A[i][j];
        A[i][j] = 0.2* (A[i][j] + A[i-1][j] + A[i+1][j] + A[i][j-1] + A[i][j+1]);
        myDiff += abs(A[i][j] - prev) // accumulate the local diff for each of the threa
      }
    }

      // Update the local Diff global diff - its part of teh contribution 
      lock.lock();
      diff += myDiff;
      lock.unlock();

      /* ALERT BARRIER - All threads have to wait here until all the thredas have completed their 
      processing. - why - cause if we have not accuulated the diff from all the threds - when
      divided by n * n - we will get the wrong comparision with the thresold */ 
      barrier.wait(my_barrier, NUM_PROCESSORS);

      if (diff / (n*n) < threadhold){
        done = true;
      }

      /* ALERT BARRIER - All threads have to wait here until all the threds have completed the 
      processing - we have to understand that even though the diff is a global variable - the value 
      if diff is compared at a thread level - so if we took a bit more time comparing in one thread 
      than another - the faster thred start the loop again and update the diff value to 0 - reset 
      and we are comparing the wrong value of the diff to threshold - so we need to wait for all the 
      threads to complete the processing */
      barrier.wait(my_barrier, NUM_PROCESSORS);
  }
}

