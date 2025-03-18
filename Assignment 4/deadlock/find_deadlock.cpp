/// =========================================================================
/// Copyright (C) 2023 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================
/// this is the ONLY file you should edit and submit to D2L
//Name: Muhammad Taimur Rizwan | Assignment 4 Q1 
#include "find_deadlock.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
using namespace std;
/// feel free to use the code below if you like:
/// ----------------------------------------------------------------
/// split(s) splits string s into vector of strings (words)
/// the delimiters are 1 or more whitespaces
static std::vector<std::string> split(const std::string & s)
{
    auto linec = s + " ";
    std::vector<std::string> res;
    bool in_str = false;
    std::string curr_word = "";
    for (auto c : linec) {
        if (isspace(c)) {
            if (in_str)
                res.push_back(curr_word);
            in_str = false;
            curr_word = "";
        } else {
            curr_word.push_back(c);
            in_str = true;
        }
    }
    return res;
}

/// feel free to use the code below if you like:
/// ----------------------------------------------------------------
/// Word2Int is a class that can convert words to unique integers
/// get(word) returns the same number given the same word
///           the numbers will start at 0, then 1, 2, ...
///
/// example:
///   Word2Int w2i;
///   w2i.get("hello") = 0
///   w2i.get("world") = 1
///   w2i.get("hello") = 0
///
/// Word2Int "may" help you get a bit more performance
/// in your cycle finding algorithm, as arrays are faster
/// than hash tables...
///
class Word2Int {
    int counter = 0;
    std::unordered_map<std::string, int> myset;

public:
    int get(const std::string & w)
    {
        auto f = myset.find(w);
        if (f == myset.end()) {
            myset[w] = counter;
            return counter++;
        }
        return f->second;
    }
};

//To finish under 10s on medium sized files
class Graph {
    public:
    std::vector<std::vector<int>> adj_list;
    std::vector<int> out_counts;   
}; 

/// this is the function you need to (re)implement
/// -------------------------------------------------------------------------
/// parameter edges[] contains a list of request- and assignment- edges
///   example of a request edge, process "p1" resource "r1"
///     "p1 -> r1"
///   example of an assignment edge, process "XYz" resource "XYz"
///     "XYz <- XYz"
///
/// You need to process edges[] one edge at a time, and run a deadlock
/// detection after each edge. As soon as you detect a deadlock, your function
/// needs to stop processing edges and return an instance of Result structure
/// with 'index' set to the index that caused the deadlock, and 'procs' set
/// to contain names of processes that are in the deadlock.
///
/// To indicate no deadlock was detected after processing all edges, you must
/// return Result with index=-1 and empty procs.
///
Result find_deadlock(const std::vector<std::string> & edges)
{
    //initializing variables
    std::vector<std::string> integerToWordObj;
    Graph newGraphObj; //initialize empty graph
    Result result;
    int convEdgeSizetoInt = (int)edges.size(); //for debug issues
    int i = 0; //iterate main loop
    int x = 0;
    Word2Int wordToIntObj; //object to call Dr. Pavol's function
    std::vector<int> zeros; //to find all nodes in a graph w/out_degree = 0  

    while(i < convEdgeSizetoInt){ //iterate for all edges 


        //edge processing and splitting
        std::vector<std::string> temp = split(edges.at(i));
        temp.at(2) = temp.at(2) + '*'; 


        int procs = wordToIntObj.get(temp[0]); //temp at index 0 is set to process
        int res = wordToIntObj.get(temp[2]); //temp at index 2 is set to resource



        if((int)integerToWordObj.size() <= procs) integerToWordObj.push_back(temp.at(0));
        
        if((int)integerToWordObj.size() <= res) integerToWordObj.push_back(temp.at(2));
        
        x = std::max(x, std::max(procs, res));



        newGraphObj.out_counts.resize( 1 + x);



        newGraphObj.adj_list.resize(1 + x);
    

        //case if edge <-
        if(temp.at(1) == "<-"){
            newGraphObj.adj_list.at(procs).push_back(res); //populating the graph with res
            newGraphObj.out_counts.at(res) = newGraphObj.out_counts.at(res) + 1; //increment outcounts in the graph by resource
        }
        //case if edge ->
        else if(temp.at(1) == "->"){
            newGraphObj.adj_list.at(res).push_back(procs); //populating the graph with procs
            newGraphObj.out_counts.at(procs) = newGraphObj.out_counts.at(procs) + 1; //increment outcounts in the graph by procsess
        }

        std::vector<int> outDegree = newGraphObj.out_counts; //to copy out_counts so we can modify it

        int convOutDegSizetpInt = (int)outDegree.size();


        int m = 0;
        while(m < convOutDegSizetpInt){
            if(outDegree.at(m) == 0) zeros.push_back(m);

            m++;
        }
        //topological sort using Dr. Pavol's Assignment 3 PDF hints
        while(!zeros.empty()){
            int n = zeros.back(); //removeing an entry from zeros
            zeros.pop_back(); 

            for (int n2 : newGraphObj.adj_list.at(n)){
                outDegree.at(n2) =  outDegree.at(n2) - 1; //decrement outDegree
                if(outDegree.at(n2) == 0) zeros.push_back(n2); //appending n2 to zeros
                
            }

        }
        int j = 0;
        while(j < convOutDegSizetpInt){ //Finding a cycle
            if(integerToWordObj.at(j).back() != '*'){

            
                if(outDegree.at(j) != 0) result.procs.push_back(integerToWordObj.at(j)); //pushback nodes to result.procs
            }
            j++;
        }

        if(!result.procs.empty()){ //record index i if procs is not empty
            result.index = i;
            return result;
        }
        i++;
    }
    result.index = -1;
    return result;
}
