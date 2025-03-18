/// -------------------------------------------------------------------------------------
/// this is the only file you need to edit
/// -------------------------------------------------------------------------------------
///
/// (c) 2023, Pavol Federl, pfederl@ucalgary.ca
/// Do not distribute this file.
//Name: Muhammad Taimur Rizwan | UCID: 30078941 | Assignment 6

#include "memsim.h"
#include <cassert>
#include <iostream>
#include <set>
#include <list>
#include <unordered_map>
#include <math.h>
#include <queue>
#include <deque>
#include <cmath>
using namespace std;

struct Partition {

  int tag;

  int64_t size, addr;
  Partition(int64_t size, int64_t addr){

    this->size = size;

    this->addr = addr;
    
  }
};

typedef std::list<Partition>::iterator ListPartIter; 

struct scmp { //from A6-P23 PDF
  bool operator()(const ListPartIter & c1, const ListPartIter &c2) const {
    if(c1->size == c2->size){
      return c1->addr < c2->addr;
    }
    
    else{
      return c1->size > c2->size;
    }
    
  }
};

std::set<ListPartIter, scmp> free_parts; //stores iterators to the linked list nodes

std::list<Partition> all_parts; //a doubly linked list to maintain the order of all partitions by their addresses


std::unordered_map<int64_t, std::vector<ListPartIter>> tagged_parts; //hash table that contains iterators to the linked list nodes

int64_t page_req = 0; 

int64_t new_page_size = 0; //to use for constructor and iterate page_size

MemSimResult result;

// I suggest you implement the simulator as a class, like the one below.
// If you decide not to use this class, feel free to remove it.
struct Simulator {
  Simulator(int64_t page_size)
  {
    // constructor
    new_page_size = page_size; //storing page size in a new variable

    Partition vacant_part = Partition(0,0); //new partition object

    vacant_part.tag = -1;

    all_parts.push_front(vacant_part);

  }
  void allocate(int tag, int size) {
    // Pseudocode for allocation request:
    // - search through the list of partitions from start to end, and
    //   find the largest partition that fits requested size
    //     - in case of ties, pick the first partition found
    // - if no suitable partition found:
    //     - get minimum number of pages from OS, but consider the
    //       case when last partition is free
    //     - add the new memory at the end of partition list
    //     - the last partition will be the best partition
    // - split the best partition in two if necessary
    //     - mark the first partition occupied, and store the tag in it
    //     - mark the second partition free

    ListPartIter partition_list = all_parts.end();

    if(free_parts.size() > 0){ //if free partitions contains iterators

      if((*(free_parts.begin()))->size >= size){

        partition_list = *free_parts.begin();
        //splitting the best partition 
        if(partition_list->size != size){

          Partition vacant_part = Partition(partition_list->size - size, partition_list->addr + size);
          vacant_part.tag = -1;
          free_parts.erase(partition_list);
          partition_list->size = size;
          partition_list->tag = tag;

          all_parts.insert(std::next(partition_list), vacant_part);
          free_parts.insert(std::next(partition_list));

        }

        else if(partition_list->size == size){

          free_parts.erase(partition_list);
          partition_list->tag = tag;

        }

        if(tagged_parts.find(partition_list->tag) == tagged_parts.end()){

          std::vector<ListPartIter> new_part_list;
          
          new_part_list.push_back(partition_list);

          tagged_parts.insert({partition_list->tag, new_part_list}); //insert tag and new_part_list to list

        }

        else if(tagged_parts.find(partition_list->tag) != tagged_parts.end()){

          tagged_parts[partition_list->tag].push_back(partition_list);

        }
        return;

      }

    }
    //for no suitable partitions
    if(partition_list == all_parts.end()){
      //case if the last partition's free
      auto b_mem = std::prev(all_parts.end());
      int64_t a_mem = 0;

      if(b_mem->tag == -1) a_mem = a_mem + b_mem->size;

      page_req += static_cast<int64_t>(std::ceil(static_cast<double>(size - a_mem) / new_page_size));

      if(a_mem == 0){

        Partition free_partition(static_cast<int64_t>(std::ceil(static_cast<double>(size - a_mem) / new_page_size)) * new_page_size, b_mem->addr + b_mem->size);

        free_partition.tag = -1;

        all_parts.emplace_back(free_partition);

        free_parts.insert(std::prev(all_parts.end()));

      }

      else if(a_mem != 0){

        free_parts.erase(b_mem);

        b_mem->size = b_mem->size + (static_cast<int64_t>(std::ceil(static_cast<double>(size - a_mem) / new_page_size)) * new_page_size);

        free_parts.insert(b_mem); //add memory  to the end of the list

      }

      partition_list = *free_parts.begin();
    } //splitting the best partition 
      if(partition_list->size != size){

        Partition vacant_part = Partition(partition_list->size - size, partition_list->addr + size);

        vacant_part.tag = -1;

        free_parts.erase(partition_list);

        partition_list->size = size;

        partition_list->tag = tag;

        all_parts.insert(std::next(partition_list), vacant_part);
        free_parts.insert(std::next(partition_list));

      }

      else if(partition_list->size == size){

        free_parts.erase(partition_list);

        partition_list->tag = tag;

      }

      if(tagged_parts.find(partition_list->tag) == tagged_parts.end()){

        std::vector<ListPartIter> new_part_list;

        new_part_list.push_back(partition_list);

        tagged_parts.insert({partition_list->tag, new_part_list}); //insert tag and new_part_list to list

      }

      else if(tagged_parts.find(partition_list->tag) != tagged_parts.end()){

        tagged_parts[partition_list->tag].push_back(partition_list);

      }

      return;

    }

  
  void deallocate(int64_t tag) {
    // Pseudocode for deallocation request:
    // - for every partition
    //     - if partition is occupied and has a matching tag:
    //         - mark the partition free
    //         - merge any adjacent free partitions

    if(tagged_parts.find(tag) == tagged_parts.end()) return;

    auto parts_list = tagged_parts.find(tag)->second;

    auto it = parts_list.begin();

    while(it != parts_list.end()){ //for every partition

      auto& n_part = *it;

      n_part->tag = -1;

      auto before = std::prev(n_part);

      auto aft = std::next(n_part);

      bool flag_bef = false;

      bool flag_aft = false;

      if(before->tag == -1) flag_bef = true;
      else if(n_part == all_parts.begin()) flag_bef = false;

      if(aft->tag == -1) flag_aft = true;
      else if(aft == all_parts.end()) flag_aft = false;

      if(!flag_bef){

        if(flag_aft){

          free_parts.erase(aft);

          n_part->tag = -1;

          n_part->size = n_part->size + aft->size;

          all_parts.erase(aft);

          free_parts.insert(n_part);
          

        }

      }

      if(flag_bef){

        if(flag_aft){

          free_parts.erase(before);

          free_parts.erase(aft);

          before->size = before->size + n_part->size + aft->size;

          all_parts.erase(aft);

          all_parts.erase(n_part);

          free_parts.emplace(before);

        }

      }

      if(!flag_bef){

        if(!flag_aft){

          n_part->tag = -1;

          free_parts.emplace(n_part);

        }

      }

      if(flag_bef){

        if(!flag_aft){

          free_parts.erase(before);

          before->size = before->size + n_part->size;

          all_parts.erase(n_part);

          free_parts.emplace(before);

        }

      }
      it++;

    }

    tagged_parts.erase(tag);


  }
  MemSimResult getStats()
  {
    // hard-coded results
    if(!free_parts.empty()){

      result.max_free_partition_address = (*free_parts.begin())->addr;

      result.max_free_partition_size = (*free_parts.begin())->size;

    }

    result.n_pages_requested = page_req;
    
    return result;

  }
  void check_consistency()
  {
    // mem_sim() calls this after every request to make sure all data structures
    // are consistent. Since this will probablly slow down your code, I suggest
    // you comment out the call below before submitting your code for grading.
    check_consistency_internal();
  }
  void check_consistency_internal()
  {
    // you do not need to implement this method at all - this is just my suggestion
    // to help you with debugging

    // here are some suggestions for consistency checks (see appendix also):

    // make sure the sum of all partition sizes in your linked list is
    // the same as number of page requests * page_size

    // make sure your addresses are correct

    // make sure the number of all partitions in your tag data structure +
    // number of partitions in your free blocks is the same as the size
    // of the linked list

    // make sure that every free partition is in free blocks

    // make sure that every partition in free_blocks is actually free

    // make sure that none of the partition sizes or addresses are < 1
  }
};

// re-implement the following function
// ===================================
// parameters:
//    page_size: integer in range [1..1,000,000]
//    requests: array of requests
// return:
//    some statistics at the end of simulation
MemSimResult mem_sim(int64_t page_size, const std::vector<Request> & requests)
{
  // if you decide to use the simulator class above, you likely do not need
  // to modify the code below at all
  Simulator sim(page_size);
  for (const auto & req : requests) {
    if (req.tag < 0) {
      sim.deallocate(-req.tag);
    } else {
      sim.allocate(req.tag, req.size);
    }
    sim.check_consistency();
  }
  return sim.getStats();
}

