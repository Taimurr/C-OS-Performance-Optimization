// -------------------------------------------------------------------------------------
// this is the only file you need to edit
// -------------------------------------------------------------------------------------
//
// (c) 2023, Pavol Federl, pfederl@ucalgary.ca
// Do not distribute this file.
//Name: Muhammad Taimur Rizwan | Assignment 5

#include "fatsim.h"
#include <iostream>
#include <algorithm>
#include <stack>
#include <queue>
#include <deque>
#include <deque>
#include <vector>
using namespace std;


class Graph{
  private:
  std::vector<std::pair<std::vector<long>, long>> nlist;
  std::vector<long> visitedNodes;
  std::vector<long> res; //contains the result
  long longestChain, currChainLength = 0; //holds the longest and current chain lengths searched
  std::stack<long> newStack;

  public:
  std::vector<long> chainEnd; //nodes that end at -1
  void updateList(const std::vector<long> & fat); 
  std::vector<long> Search();
  std::vector<long> chainBegin;
  std::vector<bool> flag;
} graph;


// reimplement this function
std::vector<long> fat_check(const std::vector<long> & fat){

  
  graph.updateList(fat);

  
  if(graph.chainEnd.empty()) return{};

  
  return graph.Search();

}




std::vector<long> Graph::Search(){ //iterate through the chainEnd

  auto it = chainEnd.begin();
  while(it != chainEnd.end()){
    const auto& m = *it;

    it = it + 1;
  
    if(visitedNodes[m] == -1){
    newStack.push(m);
    currChainLength = currChainLength + 1; //update current chain length by m

    while(true){ //simulation loop

      long current = newStack.top();
      newStack.pop();


      if(nlist[current].second != 0){ //setting the chain length at a value
        if(nlist[current].second != -1){ currChainLength = nlist[current].second;} }


  if(nlist[current].first.size() > 1){ // settong curChainLength to where a node has links to multiple nodes (child nodes)
    nlist[current].second = currChainLength;

    auto it = nlist[current].first.begin();
    while(it != nlist[current].first.end()){
      nlist[*it].second = currChainLength + 1;
      it = it + 1;
    }
  }




      if(nlist[current].first.empty()){ //first node

        if(currChainLength == longestChain) res.push_back(current); //current chain is the longest chain

        else if (longestChain < currChainLength){ //update the longest chain to the current chain if current is bigger
          longestChain = currChainLength;
          res.clear();
          res.push_back(current);
        }

        if(newStack.empty()){
          currChainLength = 0; //reset
          break;
        }

      }
      
      auto it = nlist[current].first.begin();
      while(it != nlist[current].first.end()){
        const auto& n = *it;
      


        
        if(visitedNodes[n] == -1){ //setting the chain length at a value
        if(nlist[current].second != 0){
        if(nlist[current].second != -1){ currChainLength = nlist[current].second;} }
        newStack.push(n);
        currChainLength = currChainLength + 1;
        }

        else if(visitedNodes[n] != -1){
          continue;
        }

        it = it + 1;

      }
    }

    }

    else if(visitedNodes[m] != -1) continue;

  }

  std::sort(res.begin(), res.end());
  return res; //return the resulting chain


}


void Graph::updateList(const std::vector<long> & fat){ //to populate the list
  std::vector<long> temp;
  long i = 0;
  std::pair<std::vector<long>, long> nPair;
  visitedNodes.resize(fat.size(), -1);
  nlist.resize(fat.size());
  
  auto it = fat.begin();
  while(it != fat.end()){
    const auto& f = *it;


    if(f == -1){ //node is added to the end
      chainEnd.push_back(i);
      i = i + 1;
    }

    else if(f != -1){ //node is not added to the end
      temp = nlist[f].first; //adding new element into list
      temp.push_back(i); 
      nPair = std::make_pair(temp, -1); //second node is set as the end pair
      nlist[f] = nPair;
      i = i + 1;
      
    }

    it++;
  }

}


