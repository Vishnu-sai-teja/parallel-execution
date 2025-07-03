// This is the naive implementation of the grid solver - 
// without any optimizations - only optimisatoon is at choosing a optimal algorithm

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

float* allocate(int a,int b){
  // return a random grid of size a x b
  return new float[a * b];
}

void solve(float* A){
  // The solver grid as optimised as possible - algorithm level - optimal algorithm
  // Idea is to divide the grid into alterantive dots and solve them in parallel - one batch of alteranative dots at a time
  bool done = false;
  while(!done){
    // for each dot we have to get the average of the suirrounding dots - and all of them are independent processing  - so these can be parallelsable irrespective of the location
    for_all(red_cells (i,j)){ // red and black cells are the alternative cells in the grid we were discussing about // DECOMPOSITION - processing the independent operations in parallel to workers
      float prev = A[i][j];
      A[i][j] = 0.2* (A[i][j] + A[i-1][j] + A[i+1][j] + A[i][j-1] + A[i][j+1]); 
      reduced_add(diff, abs(A[i][j] - prev)) // check for teh change in the value and accumulate this value in the ddifferene for each of the red_cells // ORCHESTRACTION - combining the results from all teh workers - with no race conditions
    } // ORCHESTRACTION - this. {} - is responsible for waiting till all the woekrs are done, returning to sequential execution
    for_all(black_cells (i,j)){
      float prev = A[i][j];
      A[i][j] = 0.2* (A[i][j] + A[i-1][j] + A[i+1][j] + A[i][j-1] + A[i][j+1]);
      reduced_add(diff, abs(A[i][j] - prev)) // check for teh change in the value and accumulate this value in the ddifferene for each of the red_cells
    } 
    // compare the total diff and check if we need more iteartions to converge
    if(diff/(n*n) < threshold){
      done = true;
    })
  }
}

int main(){
  const int n = 100;
  float* A = allocate(n+2, n+2);

  // now we have to converge the grid using our solver algorithm - and in the most optmial approach 
  solve(A);
}
