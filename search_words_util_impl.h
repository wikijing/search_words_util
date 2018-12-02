/***************************************************************************
 * Auth : lidongxiao@migu.cn
 *
 * Date : 11/22/2018
 *
 ***************************************************************************/

#ifndef HEADER_SEARCH_WORDS_UTIL_H
#define HEADER_SEARCH_WORDS_UTIL_H


#include <fstream>
#include <iomanip> // for std::setprecision
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>


#include "rapidjson/document.h"


using namespace rapidjson;


#define SAFE_DELETE_OBJ(x) \
	do { if ((x)) {delete (x); (x) = NULL;} } while(0)

/* for example :
double val = ConvertStringToNum<double>("0.7245164513587952");
std::cout << std::fixed << std::setprecision(16) << "val : "<< val << std::endl;
*/
template <class Type>
Type ConvertStringToNum(const std::string& str) {
  std::istringstream iss(str);
  Type num;
  iss >> num;
  return num;
}

bool InitResources(const char* base_config_file_name, const char* resources_net_file_name);
bool ReleaseResources(void);

bool ParseFileToJSONDom(const char* file_name, Document* dom);
std::set<std::string> GetWordsByKey(const std::string& key);
std::map<double, std::set<std::string>, std::greater<double>> GetSortedWordsByKeys(const std::set<std::string>& keys);
bool PrintWords(const std::map<double, std::set<std::string>, std::greater<double>>& words);


#endif /* HEADER_SEARCH_WORDS_H */

