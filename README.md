# ngram-studio
This project reads some input text and creates a search-index  using n-grams for fast text retrieval.

ngram-studio contains a *ngramStudio.cpp* that allows you to perform tests, and *ngramPlain.cpp* which contains the methods of the class declared in *ngramPlain.h*. This class is used to specify the input text to be indexed and the size of the n-gram: unigram, bigram, trigram, etc. The creator function of the class then creates a dictionary which contains the ngrams found in the text and pointers to the text entries lines.
Last but not least, the ngram-index can be compressed by specifying this in the corresponding parameter in the constructor of the class *NGramPlain*.

## Introduction
Basically there are the two types of text retrieval strategies:
* **_Querying_**: where the user enters a (keyword) query and the system returns the relevant text (documents). This works well when the user knows what keywords to use.
* **_Browsing_**: where the user navigates into relevant information by following a path enabled by the structures on the text (documents). This works well when the user wants to explore information, doesn’t know what keywords to use, or can’t conveniently enter a query.

This project focus on the *querying* strategy to perform text retrieval. For this:
* A collection of text (documents) exists 
* The user gives a query to express the information needed 
* The search engine system returns relevant text (documents) to the user 

The advantages on using ngram-indexing for text retrieval are the ability to match linguistically similar terms, faster performance with a smaller index, accurate matches and high precision. On the other hand, the disadvantages of ngram-indexing are the need of space to save the index and a low recall.

Finally, note that text retrieval is an empirically defined problem. This means, it can’t be mathematically proven that one method is better than another. Therefore, proof must rely on empirical evaluation involving users. 

## How to compile and link in one step:
```
g++-5 -Wall -o ngramsStudio ngramStudio.cpp ngramPlain.cpp -std=c++11
```
## How to compile and link separately:
### How to compile:
```
g++-5 -Wall -c ngramStudio.cpp ngramPlain.cpp -std=c++11
```
### How to link:
```
g++-5 -Wall -o ngramStudio ngramStudio.o ngramPlain.o 
```

## How to run in Visual Studio:
1) New project > Build from source code
2) Run/ Debug on x64 environment
