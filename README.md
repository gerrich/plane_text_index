plane_text_index
================

Using memmapped TSV files as indices

Than one wants to operate text data in C++ he creates hist own format or use something 
like protobuf or boost::interprocess/boost::archive

Protobuf is crossplatform and fast enought
own formats and Boost::interprocess cretest binaries, that are not readeble on other machines
boost::archive is readeble averythere but only in c++

In text processing TSV could be the best format for may tasks because:
1) it's human readeble
2) supported by core utils (sort/grep/paste/join etc)
3) easyly processed in any programming language
4) fits for hadoop streaming 

so why not use memmaped TSV files in C++ code?

Bi-search algo for tab separated files:
search back for first occurence of '\n'
search for first occurence of '\t' - these points are End and Begin marks of record key

