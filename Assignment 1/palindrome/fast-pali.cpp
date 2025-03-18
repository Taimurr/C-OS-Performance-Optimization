/// =========================================================================
/// Copyright (C) 2023 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================
/// Muhammad Taimur Rizwan | UCID: 30078941 | Assignment 1

#include <unistd.h>
#include <stdio.h>
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <vector>

// split string p_line into a vector of strings (words)
// the delimiters are 1 or more whitespaces
std::vector<std::string>
split( const std::string & p_line)
{
  auto line = p_line + " ";
  bool in_str = false;
  std::string curr_word = "";
  std::vector<std::string> res;
  for( auto c : line) {
    if( isspace(c)) {
      if( in_str)
        res.push_back(curr_word);
      in_str = false;
      curr_word = "";
    }
    else {
      curr_word.push_back(c);
      in_str = true;
    }
  }
  return res;
}

// global variable used in stdin_readline()
unsigned char buffer[1024*1024];

// reads in a line from STDIN
// reads until \n or EOF and result includes \n if present
// returns empty string on EOF
std::string
stdin_readline()
{
  std::string result;
  int buff_size = read( STDIN_FILENO, buffer, 1024*1024); //read 1MB worth rather than 1 bit each
  
  for(int i = 0; i < buff_size; i++) {
    
    if( buffer[i] == '\n')
    buffer[i] = ' '; //set buffer to empty constant 
    result.push_back(buffer[i]); //update result
  
  }
  return result;
}

// returns true if a word is palindrome
// palindrome is a string that reads the same forward and backward
//    after converting all characters to lower case
bool
is_palindrome( const std::string & s)
{
  for( size_t i = 0 ; i < s.size() / 2 ; i ++)
    if( tolower(s[i]) != tolower(s[s.size()-i-1]))
      return false;
  return true;
}

// Returns the longest palindrome on standard input.
// In case of ties for length, returns the first palindrome found.
//
// Algorithm:
// Input is broken into lines, each line into words, and each word
// is checked to see if it is palindrome, and if it is, whether it
// is longer than the largest palindrome encountered so far.
//
// A word is a sequence of characters separated by white space
// white space is whatever isspace() says it is
//    i.e. ' ', '\n', '\r', '\t', '\n', '\f'
std::string
get_longest_palindrome()
{
  std::string max_pali;
  while(1) {
    std::string line = stdin_readline();
    if( line.size() == 0) break;
    auto words = split( line);
    for( auto word : words) {
      if( word.size() <= max_pali.size())
        continue;
      if( is_palindrome(word))
        max_pali = word;
    }
  }
  return max_pali;
}

int
main()
{
  std::string max_pali = get_longest_palindrome();
  printf("Longest palindrome: %s\n", max_pali.c_str());
  return 0;
}

