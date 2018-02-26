/******************************************************************* -*- C++ -*-
*
* author: Jesus Emeterio Navarro Barrientos.
* web: https://sites.google.com/site/jenavarrob/
* date: 2017-01-03
*******************************************************************************/

#include "ngramPlain.h"

NGramPlain::NGramPlain(U8 ngramSize, bool isCompressed)
  : _ngramsIDsDict()
  , _idStringsDict()
{
  initializeNGrams(ngramSize, isCompressed);
}

NGramPlain::NGramPlain(StdStringList& strList, U8 ngramSize, bool isCompressed)
  : _ngramsIDsDict()
  , _idStringsDict()
{
  S32 id = -1;
  initializeNGrams(ngramSize, isCompressed);

  for (StdStringList::iterator vi = strList.begin(),
       ve = strList.end(); vi != ve; ++vi) {
    ++id;

    if (addStringToNGramsDict(*vi, id)) {
      _idStringsDict[id] = *vi;
    }
  }
}

NGramPlain::NGramPlain(StdStringVector& strVector, U8 ngramSize, bool isCompressed)
  : _ngramsIDsDict()
  , _idStringsDict()
{
  S32 id = -1;
  initializeNGrams(ngramSize, isCompressed);

  for (StdStringVector::iterator vi = strVector.begin(),
       ve = strVector.end(); vi != ve; ++vi) {
    ++id;

    if (addStringToNGramsDict(*vi, id)) {
      _idStringsDict[id] = *vi;
    }
  }
}

// -----------------------------------------------------------------------------
NGramPlain::~NGramPlain()
{
  for (KeyStrVectorValuesMap::iterator it = _ngramsIDsDict.begin(); it != _ngramsIDsDict.end(); ++it) {
    delete it->second;
  }
}

// -----------------------------------------------------------------------------
void
NGramPlain::addNGramToDict(const StdString& str, S32 id)
{
  if (_ngramsIDsDict.find(str) != _ngramsIDsDict.end()) {
    //found the ngram, so append the id to the vector of ids
    VectorValues* values = _ngramsIDsDict[str];

    //We need to check if values already contains the id, the same ngram could be found twice or more in a string, we need only one id for the moment.
    //Consequently, information like frequency of ngram in string is lost, however, to use a map within each ngram with <id, freq>, makes more sense for sentences and paragraphs,
    //it doesn't makes much sense for short strings like for example states and street names.

    //if we want to find faster the ids and
    //we need then to sort ascending and do binary search.
    //For the moment we assume the ids are given/added in ascending sorted order
    if (!_isCompressed) {
      if (!(std::find(values->begin(), values->end(), id) != values->end())) {
        values->push_back(id);
      }
    }
    else { //decompress and compress ids for saving memory space.
      //decompress
      VectorValues decompressedKeys;
      decompressKeys(*values, decompressedKeys);

      //add to temporary vector
      decompressedKeys.push_back(id);

      //compress and adds newid to KVMap values
      compressKeys(decompressedKeys, *values);
    }
  }
  else { //new ngram so insert it together with its id
    VectorValues* values = new VectorValues();
    values->push_back(id);
    _ngramsIDsDict[str] = values;
  }
}

// -----------------------------------------------------------------------------
bool
NGramPlain::addStringToNGramsDict(StdString& str, S32 id)
{
  StdStringVector ngrams;

  getNGrams(str, ngrams);

  //add ngrams to map key=ngrams, value=ids
  for (StdStringVector::const_iterator ti = ngrams.begin(),
       te = ngrams.end(); ti != te; ++ti) {
    addNGramToDict(*ti, id);
  }

  return true;
}

// -----------------------------------------------------------------------------
void
NGramPlain::clear()
{
  _idStringsDict.clear();

  //delete first sets m1::Set<S32>
  for (KeyStrVectorValuesMap::iterator it = _ngramsIDsDict.begin(); it != _ngramsIDsDict.end(); ++it) {
    delete it->second;
  }

  _ngramsIDsDict.clear();
}

// -----------------------------------------------------------------------------
bool
NGramPlain::compressKeys(VectorValues& decompressedKeys, VectorValues& compressedKeys)
{
  if (decompressedKeys.size() == 0) {
    return false;
  }

  compressedKeys.clear();
  compressedKeys.push_back(decompressedKeys.front());

  for (size_t i = 1; i < decompressedKeys.size(); i++) {
    compressedKeys.push_back(decompressedKeys[i] - decompressedKeys[i - 1]);
  }

  return true;
}

// -----------------------------------------------------------------------------
bool
NGramPlain::decompressKeys(VectorValues& compressedKeys, VectorValues& decompressedKeys)
{
  if (compressedKeys.size() == 0) {
    return false;
  }

  decompressedKeys.clear();
  decompressedKeys.push_back(compressedKeys.front());

  for (size_t i = 1; i < compressedKeys.size(); i++) {
    decompressedKeys.push_back(compressedKeys[i] + decompressedKeys[i - 1]);
  }

  return true;
}

// -----------------------------------------------------------------------------
void
NGramPlain::getIDsWithSimilarString(StdString& str, S32 numIdsToRetrieve, VectorIdSim& ids)
{
  //get ngrams for searched string str
  StdStringVector ngrams;
  this->getNGrams(str, ngrams);

  //GET MAP WITH THE  FREQUENCIES OF THE NGRAMS
  KeyIdValueFreq keyValueMap;
  getNGramFreq(ngrams, keyValueMap);

  //ToDO: find a way to speed up process by selecting from keyValueMap only those with largest values (frequencies), to avoid large VectorIdfreq items for sorting

  //SORT SEGMENT DICTIONARY
  typedef vector<IdFreq> VectorIdFreq;
  VectorIdFreq items;
  //get Keys and Values, for this, note that we swap key/value to sort by values
  for (map<S32, S32>::iterator mit = keyValueMap.begin(); mit != keyValueMap.end(); ++mit) {
    items.push_back(IdFreq(mit->second, mit->first));  //mit->second is the freq, mit->first is the id
  }

  //sorts in ascending order, recall we want to grab items from last to first, i.e. in descending order
  std::sort(items.begin(), items.end());
  //std::partial_sort(items.begin(), items.begin() + numIdsToRetrieve, items.end(), IdFreqGreaterSimCompare());

  //RETURN MOST SIMILAR SEARCH TERMS
  //ToDo: include different options for similarity measurement
  //for the moment we use a simple formula, but we can include other similarity measures, for example edit distance
  if (numIdsToRetrieve > 0) {
    S32 co = 0; //counter for maximal number of retrieved similar strings
    for (VectorIdFreq::iterator it = items.begin(); it != items.end() && co < numIdsToRetrieve ; ++it, ++co) {
      F32 sim = (F32)it->first / (F32)ngrams.size();
      ids.push_back(IdSim(it->second, sim)); //rit->second is the id, rit->first is the freq
    }
  }
  else {
    for (VectorIdFreq::iterator it = items.begin(); it != items.end(); ++it) {
      F32 sim = (F32)it->first / (F32)ngrams.size();
      ids.push_back(IdSim(it->second, sim)); //rit->second is the id, rit->first is the freq
    }
  }
}

// -----------------------------------------------------------------------------
KeyIdValueStrMap
NGramPlain::getIdsAndStrings()
{
  KeyIdValueStrMap res;

  for (KeyIdValueStrMap::iterator ti = _idStringsDict.begin(),
       te = _idStringsDict.end(); ti != te; ++ti) {
    res[ti->first] = ti->second;
  }

  return res;
}

// -----------------------------------------------------------------------------
void
NGramPlain::getNGramFreq(StdStringList& ngrams, KeyIdValueFreq& keyValueMap)
{
  VectorValues decompressedKeys;
  //for-loop all ngrams of str and get ids from keymap the ngram ids
  for (StdStringList::const_iterator ti = ngrams.begin(),
       te = ngrams.end(); ti != te; ++ti) {

    if (_ngramsIDsDict.find(*ti) != _ngramsIDsDict.end()) {
      //ngram exists, get the ids
      VectorValues* values = _ngramsIDsDict[*ti];
      VectorValues::const_iterator vi, ve;

      if (!_isCompressed) {
        vi = values->begin();
        ve = values->end();
      }
      else {
        //decompress
        decompressedKeys.clear();
        decompressKeys(*values, decompressedKeys);
        vi = decompressedKeys.begin();
        ve = decompressedKeys.end();
      }

      for (; vi != ve; ++vi) {

        if (keyValueMap.find(*vi) != keyValueMap.end()) {
          //found the id, increase freq counter
          ++keyValueMap[*vi];
        }
        else {
          //new id index found, add new freq counter
          keyValueMap[*vi] = 1;
        }
      }
    }
    else {
      //ngram not found in dict.
      //ToDo: maybe we can include ngrams that haven't been found somehow in similarity measure
    }
  }
}

// -----------------------------------------------------------------------------
void
NGramPlain::getNGramFreq(StdStringVector& ngrams, KeyIdValueFreq& keyValueMap)
{
  VectorValues decompressedKeys;
  //for-loop all ngrams of str and get ids from keymap the ngram ids
  for (StdStringVector::const_iterator ti = ngrams.begin(),
       te = ngrams.end(); ti != te; ++ti) {

    if (_ngramsIDsDict.find(*ti) != _ngramsIDsDict.end()) {
      //ngram exists, get the ids
      VectorValues* values = _ngramsIDsDict[*ti];
      VectorValues::const_iterator vi, ve;

      if (!_isCompressed) {
        vi = values->begin();
        ve = values->end();
      }
      else {
        //decompress
        decompressedKeys.clear();
        decompressKeys(*values, decompressedKeys);
        vi = decompressedKeys.begin();
        ve = decompressedKeys.end();
      }

      for (; vi != ve; ++vi) {

        if (keyValueMap.find(*vi) != keyValueMap.end()) {
          //found the id, increase freq counter
          ++keyValueMap[*vi];
        }
        else {
          //new id index found, add new freq counter
          keyValueMap[*vi] = 1;
        }
      }
    }
    else {
      //ngram not found in dict.
      //ToDo: maybe we can include ngrams that haven't been found somehow in similarity measure
    }
  }
}


// -----------------------------------------------------------------------------
void
NGramPlain::getNGrams(StdString str, StdStringList& ngrams) const
{
  str.insert(0, _ngramBorders);
  str.insert(str.length(), _ngramBorders);

  size_t sizeStr = str.length() - _ngramLength;

  for (size_t i = 0; i <= sizeStr; ++i) {
    ngrams.push_back(str.substr(i, _ngramLength));
  }
}

// -----------------------------------------------------------------------------
void
NGramPlain::getNGrams(StdString str, StdStringVector& ngrams) const
{
  str.insert(0, _ngramBorders);
  str.insert(str.length(), _ngramBorders);

  size_t sizeStr = str.length() - _ngramLength;

  for (size_t i = 0; i <= sizeStr; ++i) {
    ngrams.push_back(str.substr(i, _ngramLength));
  }
}

// -----------------------------------------------------------------------------
StdString
NGramPlain::getStringByID(S32 id)
{
  StdString resStr = "";

  if (_idStringsDict.find(id) != _idStringsDict.end()) {
    //entry id->string exists
    resStr = _idStringsDict[id];

  }

  return resStr;
}

// -----------------------------------------------------------------------------
void
NGramPlain::initializeNGrams(U8 ngramSize, bool isCompressed)
{
  _ngramLength = ngramSize;

  //repeat a single character _ngramSize-1 times to establish the borders of strings
  StdString tmpBorders(_ngramLength - 1, _ngramBorder);
  _ngramBorders.append(tmpBorders);
  _isCompressed = isCompressed;
}
