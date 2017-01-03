/******************************************************************* -*- C++ -*-
*
* author: Jesus Emeterio Navarro Barrientos.
* web: https://sites.google.com/site/jenavarrob/
* date: 2017-01-03
*******************************************************************************/

#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void readLinesFromFile(ifstream& in, vector<std::string>& lines)
{
  string line;
  //read file and save lines in a list
  while (std::getline(in, line)) {
    lines.push_back(line);
  //  std::cout << line << std::endl;
  }
}

void getRandomListElemenst(vector<std::string>& lines, vector<std::string>& randLines, double percentage)
{
  int numLines = lines.size();
  int numRandElems = int(percentage * numLines);
  std::cout << "numRandElems: " << numRandElems << std::endl;

  for (int i = 0; i < numRandElems; ++i) {
    int rndIndex = rand() % numLines;
    randLines.push_back(lines[rndIndex]);
  }
}

#endif // UTILS_HPP
