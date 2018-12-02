g++ -shared -fPIC -ID:\Python37\include -LD:\Python37\libs search_words_util.cpp search_words_util_impl.cpp -o search_words_util.pyd -lpython37

copy /Y search_words_util.pyd D:\Python37\DLLs