/******************************************************************* -*- C++ -*-
*
* author: Jesus Emeterio Navarro Barrientos.
* web: https://sites.google.com/site/jenavarrob/
* date: 2017-01-03
*******************************************************************************/

#include <fstream>  //std::ifstream
#include <iostream>  // std::cout
#include <assert.h>
#include <time.h>

#include "ngramPlain.h"
#include "utils.hpp"

using namespace std;

void test10Strings();
void testFile(ifstream& in);
void testNGramSearch(NGramPlain& ngrams, StdStringVector& lines);

int main(int argc, char * argv[]) {
  ////simple test 10 fixed strings
  test10Strings();

  ////test for all street names in Germany (OSM data)
  string fileName = "strassenOSM.txt";
  ifstream inFile;
  inFile.open(fileName.c_str(), ifstream::in);

  if (!inFile) {
    cout << "\ncould not open file: " << fileName << endl;
    return 1;
  }
  testFile(inFile);

return 0;
}

void showInput(const KeyIdValueStrMap& mapInputs)
{
  cout << "ID\t STRING" << endl;

  for (KeyIdValueStrMap::const_iterator ti = mapInputs.begin(),
       te = mapInputs.end(); ti != te; ++ti) {
    cout << ti->first << "\t" << ti->second << endl;
  }
}

void showResults(NGramPlain& ngrams1, VectorIdSim simIds)
{
  for (VectorIdSim::const_iterator ti = simIds.begin(),
       te = simIds.end(); ti != te; ++ti) {
    cout << "id: " << ti->first
         << "\tstring:" << ngrams1.getStringByID(ti->first)
         << "\tsimilarity:" << ti->second << endl;
  }
}


void test10Strings()
{
  StdStringList listStrings;

  listStrings.push_back("Benzhauser"); //0
  listStrings.push_back("Berlin");
  listStrings.push_back("Bernau"); //2
  listStrings.push_back("Biesterfeld");  //3
  listStrings.push_back("Binnerfeld");
  listStrings.push_back("Prenzlau");  //5
  listStrings.push_back("Prinzessinweg");
  listStrings.push_back("Sterntorbrücke"); //7
  listStrings.push_back("Stralau");
  listStrings.push_back("Zwischenhausen"); //9

  bool isCompressed = true;

  //create object of class NGrams to index strings using trigrams for fast-retrieval
  NGramPlain ngrams1(listStrings, 3, isCompressed);
  showInput(ngrams1.getIdsAndStrings());

  string searchTerm = "alau";
  int numresults = 5;

  VectorIdSim  simIds;
  ngrams1.getIDsWithSimilarString(searchTerm, numresults, simIds);
  cout << "\nSearch for similar terms: " << searchTerm  << endl;
  showResults(ngrams1, simIds);
}

void testFile(ifstream& in)
{
  assert(in);

  StdStringVector lines;
  readLinesFromFile(in, lines);

  StdStringVector randLinesSearch;
  getRandomListElemenst(lines, randLinesSearch, 0.0001); //0.0001 percentage of lines to take for testing

  //CREATE NGRAM-INDEX
  bool isCompressed = false;

  //create object of class NGrams to index strings using trigrams for fast-retrieval
  StdStringVector randLinesIndex;
  getRandomListElemenst(lines, randLinesIndex, 0.1);
  NGramPlain ngrams(randLinesIndex, 3, isCompressed); //3 for trigrams
  //showInput(ngrams25.getIdsAndStrings());

  ///TEST for vector of lines on different sizes of input data
  //test ngrams
  const clock_t startTime = clock();
  testNGramSearch(ngrams, randLinesSearch); //
  double cpuTime = (float(clock() - startTime) / CLOCKS_PER_SEC);
  cout << "Running Time = " << cpuTime << endl;
  system("PAUSE");
}

void testNGramSearch(NGramPlain& ngrams, StdStringVector& lines)
{
  VectorIdSim simIds;
  int numIds = 2;
  for (StdStringVector::iterator it = lines.begin(), ite = lines.end();
       it != ite; ++it) {
    ngrams.getIDsWithSimilarString(*it, numIds, simIds);
    for (VectorIdSim::const_iterator ti = simIds.begin(),
         te = simIds.end(); ti != te; ++ti) {
      ngrams.getStringByID(ti->first);
                cout << "id: " << ti->first
                      << "\tstring:" << ngrams.getStringByID(ti->first)
                      << "\tsimilarity:" << ti->second << endl;
    }
  }
}
