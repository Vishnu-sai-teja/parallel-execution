/* We have solved the problem in either way. using the serialisation and then optimised 
the program to use the shared address space to parallelise the solver - and also remove 
the rance condition using the locks and barriers to avoid any of the thread to mistakenly 
overwrite the gloabal diff, which control the convergence of the grid */

// Global Variables
int n;
bool done = false;
float diff[3] // global diff variable of the solver - this variable is responsible for the convergence of the grid
LOCK = lock;
BARRIER = barrier;

float* allocate(int a,int b){
  // return a random grid of size a x b
  return new float[a * b];
}

float* A = allocate(n+2, n+2);

void solver(float* A){
  // inistialse the local thred level varaibles 
  float myDiff = 0.0;
  int threadID = getThreadId();
  int myMin = 1 + (threadID) * (n / num_workers); // start row of work with the threadID
  int myMax = myMin + (n / num_workers);

  int index = 0; // start with a index of 0 - for teh iteartaion. the index = 0 for all the thress
  diff[index] = 0.0;

   // ---- issue ---- threads can come and replace the global diff before reset in other threads - need a barreir here
  
  while(!done){
    float myDiff = 0.0; // local variable for the thread set to 0.0
    // float diff[index] = 0.0; // wrong - as can updat the index value to 0.0 - of some of the threds started processing the final diff of teh current iteration.
    
    for(j = myMin to myMax){
      for(i = red_cells in this row){
        float prev = A[i][j];
        A[i][j] = 0.2* (A[i][j] + A[i-1][j] + A[i+1][j] + A[i][j-1] + A[i][j+1]);
        myDiff += abs(A[i][j] - prev);
        // accumulate the local diff for each of the thread
      }
    }
    lock.lock();
    diff[index] += myDiff;
    lock.unlock();
    diff[(index + 1)%3] = 0.0 // initialise the valuse of the next iteation threds to 0 - in all and no overriding of this value - byt any thred.
    barrier.wait(myBarrier, NUM_PROCESSORS);

    // ---- issue ----- some thereds can and compare the diff before all the threads
    //  have accumulated theid diff - need abarrier to stop all the threds going into comparision 
    // even before accumulating teh diff from all the threds

    if (diff[index] / (n*n) < threadhold){
      done = true;  // break the loop if the diff is less than the threshold - grid has converged.
    
    index = (index + 1)%3; 
      /* use the next index value for teh diff - now ther eis no issue in 
      teh overriding, just the issue is that the comparisioon shpould accumulat the whole diff 
      instead of teh diff of the current threds - we still need the below barrier.
      */

    // ----- issue ----- some of threds even before comparision can go ahead and reset teh value of t
    // he diff to 0.0 anf the comparision comes to true asnd reset the done to truye dna break the loop.  
    }
  }
  
}

/* ----- issue analysis ----- 
 -- thredas from previous iteration are late and these values may be reset byt teh 
 threads taht are in teh next iteartion
 -- threds from teh current iteartion have processed teh diff but the threds in the prev 
 iteartion to current iteration that are slow - might reset once teh values are updated by the 
 fast threads
 -- shared variables in the fast threads is compared even before processing the dif
 f from all the threds - so the comparision is wrong

-- How to workaround this ?
-- -- use differnet difff variable for each of the iteartion - to keep the track of 
chnages iteration by iteartion differnet.
-- differnet diff variable - for each of the sucessive iteration - stored with the array.
*/