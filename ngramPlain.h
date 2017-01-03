/******************************************************************* -*- C++ -*-
*
* author: Jesus Emeterio Navarro Barrientos.
* web: https://sites.google.com/site/jenavarrob/
* date: 2017-01-03
*******************************************************************************/

#ifndef NGRAMPLAIN_H
#define NGRAMPLAIN_H

#include <algorithm>
#include <cstdlib>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>

using namespace std;

typedef u_int8_t U8;
typedef int32_t S32;
typedef float F32;
typedef pair<S32, F32> IdSim; //for id and similarity
typedef vector<IdSim> VectorIdSim; //for vector of id and similarity
typedef vector<S32> VectorValues;
typedef string StdString;
typedef list<StdString> StdStringList;
typedef vector<StdString> StdStringVector;
typedef unordered_map<StdString, VectorValues*> KeyStrVectorValuesMap;
typedef map<S32, StdString> KeyIdValueStrMap;
typedef map<S32, S32> KeyIdValueFreq;
typedef std::pair<S32, S32> IdFreq;

struct IdFreqGreaterSimCompare {
  bool operator()(const IdFreq& idSim1, const IdFreq& idSim2)
  {
    return idSim1.first > idSim2.first;
  }
};

class NGramPlain {
public:
  typedef KeyIdValueStrMap::iterator idStrIterator;
  enum RebuildOption {
    NO_REINDEXING = 0,
    REINDEXING
  };

  NGramPlain(U8 ngramSize, bool isCompressed);
  NGramPlain(StdStringList& strList, U8 ngramSize, bool isCompressed);
  NGramPlain(StdStringVector& strVector, U8 ngramSize, bool isCompressed);
  ~NGramPlain();

  void clear(); //clear maps: _keyStrValueSetMap & _keyIdValueStrMap;
  void getIDsWithSimilarString(StdString& str, S32 numIdsToRetrieve, VectorIdSim& ids);
  void getNGrams(StdString str, StdStringList& ngrams) const;
    void getNGrams(StdString str, StdStringVector& ngrams) const;
  StdString getStringByID(S32 id);
  KeyIdValueStrMap getIdsAndStrings();

  //static functions
  static bool compressKeys(VectorValues& decompressedKeys, VectorValues& compressedKeys);
  static bool decompressKeys(VectorValues& compressedKeys, VectorValues& decompressedKeys);

private:
  void addNGramToDict(const StdString& str, S32 id);
  bool addStringToNGramsDict(StdString& str, S32 id);
  void getNGramFreq(StdStringList& ngrams, KeyIdValueFreq& keyValueMap);
    void getNGramFreq(StdStringVector& ngrams, KeyIdValueFreq& keyValueMap);
  void initializeNGrams(U8 ngramSize, bool isCompressed);

  KeyStrVectorValuesMap _ngramsIDsDict;  //ngrams dictionary: ngrams and ids
  KeyIdValueStrMap _idStringsDict;  //ids and strings

  size_t _ngramLength;
  const static char _ngramBorder = '_'; //char for string borders
  StdString _ngramBorders; //borders for signilizing beginning and end of strings
  bool _isCompressed;
};

#endif // NGRAMPLAIN_H
