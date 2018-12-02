/***************************************************************************
 * Auth : lidongxiao@migu.cn
 *
 * Date : 11/22/2018
 *
 ***************************************************************************/

#include <iterator>
#include <time.h>

#include "search_words_util_impl.h"


using namespace rapidjson;


// json DOM object defined in rapidjson lib.
static Document* dom_base_config = NULL;
static Document* dom_resources_net = NULL;



bool InitResources(const char* base_config_file_name, const char* resources_net_file_name) {
  if (NULL == base_config_file_name || NULL == resources_net_file_name) {
    std::cout << "input parameters is null." << std::endl;
    return false;
  }
  SAFE_DELETE_OBJ(dom_base_config);
  dom_base_config = new Document;
  if (!ParseFileToJSONDom(base_config_file_name, dom_base_config)) {
  	SAFE_DELETE_OBJ(dom_base_config);
    return false;
  }

  SAFE_DELETE_OBJ(dom_resources_net);
  dom_resources_net = new Document;
  if (!ParseFileToJSONDom(resources_net_file_name, dom_resources_net)) {
  	SAFE_DELETE_OBJ(dom_base_config);
	SAFE_DELETE_OBJ(dom_resources_net);
    return false;
  }

  return true;
}

bool ReleaseResources(void) {
  SAFE_DELETE_OBJ(dom_base_config);
  SAFE_DELETE_OBJ(dom_resources_net);
  return true;
}

bool ParseFileToJSONDom(const char* file_name, Document* dom) {
  if (NULL == file_name || NULL == dom) {
    std::cout << "input parameters is NULL!";
    return false;
  }

  // read file to string
  std::string file_content_str;
  std::ifstream infile(file_name, std::ios::in);
  if (!infile.is_open()) {
    std::cout << "failed to open the file : " << file_name << std::endl;
    return false;
  } else {
    std::istreambuf_iterator<char> begin(infile);
    std::istreambuf_iterator<char> end;
    file_content_str.assign(begin, end);
    infile.close();
    infile.clear();
  }

  //std::wcout.imbue(std::locale(""));
  //std::cout << "Get file contents :" << std::endl << file_content_str << std::endl;
  //std::cout << "Length : " << file_content_str.length() << std::endl;

  dom->Parse(file_content_str.c_str());

  if (dom->HasParseError()) {
    std::cout << "HasParseError" << std::endl;
    return false;
  }

  return true;
}

std::set<std::string> GetWordsByKey(const std::string& key) {
  std::set<std::string> words;
  if (NULL == dom_base_config) {
    std::cout << "base_config file is not loaded!" << std::endl;
    return words;
  }

  const Value& value = (*dom_base_config)[key.c_str()];
  if (value.IsArray()) {
    for (SizeType i = 0; i < value.Size(); i++) {
      if(value[i].IsString()) {
        //std::cout << "JackColton string (index : " << i << ")" << value[i].GetString() << std::endl;
        words.insert(value[i].GetString());
      }
    }
  }

  return words;
}

std::map<double, std::set<std::string>, std::greater<double>> GetSortedWordsByKeys(const std::set<std::string>& keys) {
  std::map<double, std::set<std::string>, std::greater<double>> sorted_words;
  if (NULL == dom_resources_net) {
    std::cout << "resources_net file is not loaded!" << std::endl;
    return sorted_words;
  }
  // std::cout << "keys size : " << keys.size() << std::endl;

  std::map<std::string, double> repo;
  std::map<std::string, double>::iterator it_repo;
  for (std::set<std::string>::const_iterator it_keys = keys.begin(); it_keys != keys.end(); it_keys++) {
    if ((*dom_resources_net).HasMember((*it_keys).c_str())) {
      const Value& value = (*dom_resources_net)[(*it_keys).c_str()];
      for (Value::ConstMemberIterator itr = value.MemberBegin(); itr != value.MemberEnd(); itr++) {
        // name : value  --->  "Ann":0.6669958829879761
        std::string it_name = itr->name.GetString();
        const Value& it_value = itr->value;
        double val_double;
        if (it_value.IsString()) {
          val_double = ConvertStringToNum<double>(it_value.GetString());
        } else if (it_value.IsNumber()) {
          val_double = it_value.GetDouble();
        } else {
          std::cout << "value is not a string or number, ignore it!" << std::endl;
          continue;
        }
        double new_val = val_double;
        if ((it_repo = repo.find(it_name)) != repo.end()) {
          new_val = (it_repo->second + val_double)/2;
        }
        repo[it_name] = new_val;
      }
    }
  }


  std::map<double, std::set<std::string>>::iterator it_sw;
  for (it_repo = repo.begin(); it_repo != repo.end(); it_repo++) {
    std::string word = it_repo->first;
    double val = it_repo->second;
    if ((it_sw = sorted_words.find(val)) != sorted_words.end()) {
      it_sw->second.insert(word);
    } else {
      std::set<std::string> words;
      words.insert(word);
      sorted_words[val] = words;
    }
  }
  // PrintWords(sorted_words);
  return sorted_words;
}


bool PrintWords(const std::map<double, std::set<std::string>, std::greater<double>>& words) {
  std::cout << "Print Sorted Words :" << std::endl;
  std::map<double, std::set<std::string>, std::greater<double>>::const_iterator it_sw = words.begin();
  for (int i = 1; it_sw != words.end(); it_sw++, i++) {
    std::cout << "#" << i << std::fixed << std::setprecision(16) << "  value = "<< it_sw->first << std::endl;
    std::cout << "    words : ";
    std::set<std::string>::iterator it = it_sw->second.begin();
    for (; it != it_sw->second.end(); it++) {
      std::cout << *it << "; ";
    }
    std::cout << std::endl;
  }
  return true;
}


#if 0


#define PRINT_USED_TIME

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cout << "input parameters error!" << std::endl;
    return 1;
  }

#ifdef PRINT_USED_TIME
  std::cout << "[Used Time] : Start" << std::endl;
  clock_t start_clock = clock();
#endif

  std::string base_file_name = argv[1];
  std::string key = argv[2];
  std::string resource_file_name = argv[3];
  bool rslt = InitResources(base_file_name.c_str(), resource_file_name.c_str());

#ifdef PRINT_USED_TIME
  {
    clock_t end_clock = clock();
    double diff = (double)(end_clock - start_clock);
    std::cout << "[Used Time] : Parse 2 files to JSON DOM : "<< diff << "ms" << std::endl;
  }
#endif

  Document dom_base_config;
  if (rslt) {
    std::set<std::string> words = GetWordsByKey(key);

#ifdef PRINT_USED_TIME
    {
      clock_t end_clock = clock();
      double diff = (double)(end_clock - start_clock);
      std::cout << "[Used Time] : Get words from base_config JSON DOM by key: "<< diff << "ms" << std::endl;
    }
#endif

    int count = words.size();
    //std::cout << "Found " << count << " words by key : " << key << std::endl;
    if (count > 0) {
      std::map<double, std::set<std::string>> sorted_words = GetSortedWordsByKeys(words);

#ifdef PRINT_USED_TIME
      {
        clock_t end_clock = clock();
        double diff = (double)(end_clock - start_clock);
        std::cout << "[Used Time] : Get sorted words from resources_net by keys : "<< diff << "ms" << std::endl;
      }
#endif

      PrintWords(sorted_words);
    }
  }

#ifdef PRINT_USED_TIME
  clock_t end_clock = clock();
  double diff = (double)(end_clock - start_clock);
  std::cout << "[Used Time] Total : "<< diff << "ms" << std::endl;
#endif

  ReleaseResources();

  return 1;
}

#endif



