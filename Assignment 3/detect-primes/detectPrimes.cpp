/// ============================================================================
/// Copyright (C) 2023 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// ============================================================================
///
/// You must modify this file and then submit it for grading to D2L.
///
/// You can delete all contents of this file and start from scratch if
/// you wish, as long as you implement the detect_primes() function as
/// defined in "detectPrimes.h".
//Name: Muhammad Taimur Rizwan | Assignment 3

#include "detectPrimes.h"
#include <cmath>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <iostream>
#include <thread>
#include <atomic>
using namespace std;





// C++ barrier class (from lecture notes).
// You do not have to use it in your implementation. If you don't use it, you
// may delete it.
class simple_barrier {
  std::mutex m_;
  std::condition_variable cv_;
  int n_remaining_, count_;
  bool coin_;

  public:
  simple_barrier(int count = 1) { init(count); }
  void init(int count)
  {
    count_ = count;
    n_remaining_ = count_;
    coin_ = false;
  }
  bool wait()
  {
    if (count_ == 1) return true;
    std::unique_lock<std::mutex> lk(m_);
    if (n_remaining_ == 1) {
      coin_ = !coin_;
      n_remaining_ = count_;
      cv_.notify_all();
      return true;
    }
    auto old_coin = coin_;
    n_remaining_--;
    cv_.wait(lk, [&]() { return old_coin != coin_; });
    return false;
  }
};

// returns true if n is prime, otherwise returns false
// -----------------------------------------------------------------------------
// to get full credit for this assignment, you will need to adjust or even
// re-write the code in this function to make it multithreaded.
static bool is_prime(int64_t n)
{
  // handle trivial cases
  if (n < 2) return false;
  if (n <= 3) return true; // 2 and 3 are primes
  if (n % 2 == 0) return false; // handle multiples of 2
  if (n % 3 == 0) return false; // handle multiples of 3
  // try to divide n by every number 5 .. sqrt(n)
  int64_t i = 5;
  int64_t max = sqrt(n);
  while (i <= max) {
    if (n % i == 0) return false;
    if (n % (i + 2) == 0) return false;
    i += 6;
  }
  // didn't find any divisors, so it must be a prime
  return true;
}

static std::atomic<bool> primeFound(false);

void newThread(std::vector<int64_t> & res, uint64_t & i, simple_barrier & barrier, int64_t nThreads, int64_t nThread, int64_t max, int64_t checkPrime){

  int m = 5;

  while (1)
  {
    
    if(checkPrime % ((nThread * 6) + 2 + m) == 0 || checkPrime % ((nThread * 6) + m) == 0) {

      primeFound = false;

    } 
    //leave thread if no prime
    if(!primeFound || (nThread * 6) + 2 + m > max && ((nThread * 6) + m > max )){

    break;
  }


    m += (nThreads*6);
  }
    if(barrier.wait()){

      if(primeFound) {

        res.push_back(checkPrime);

      }

    }
    barrier.wait();
  
  


}




// This function takes a list of numbers in nums[] and returns only numbers that
// are primes.
//
// The parameter n_threads indicates how many threads should be created to speed
// up the computation.
// -----------------------------------------------------------------------------
// You will most likely need to re-implement this function entirely.
// Note that the current implementation ignores n_threads. Your multithreaded
// implementation must use this parameter.

std::vector<int64_t> detect_primes(const std::vector<int64_t> & nums, int n_threads){



  std::thread tot_threads[n_threads];
  simple_barrier barrier(n_threads);
  uint64_t i = 0;
  int64_t checkPrime = 0;
  int64_t flag = false;
  std::vector<int64_t> result;

  if(n_threads >= 2){

    flag = true;

  }

  while(1){

    if(i == nums.size()){ //done

      break;

    }

    else if(i != nums.size()){ 

      checkPrime = nums[i++];

      if((flag == false || checkPrime < 100000)){ //checking for prime

        if(is_prime(checkPrime)){

          result.push_back(checkPrime);

        }

        continue;

      }

      if(checkPrime % 2 == 0 || checkPrime % 3 == 0){ //for numbers that are divisible by 2 or 3

        continue;
      }


    }
 
    primeFound = true; //found a Prime if number in file is not divisible

    int64_t j = 0;
    while(j < n_threads){
      //update total number of threads for each j
      tot_threads[j] = std::thread(newThread, std::ref(result), std::ref(i), std::ref(barrier), n_threads, j, sqrt(checkPrime), checkPrime);
      
      j++;

    }

    for(auto & n : tot_threads){ //joining threads together

      n.join();
      
    }

  }
  return result;

}
