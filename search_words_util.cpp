/***************************************************************************
 * Auth : lidongxiao@migu.cn
 *
 * Date : 11/22/2018
 *
 ***************************************************************************/


#include <math.h>


#include "Python.h"
#include "search_words_util_impl.h"

#ifdef __cplusplus
extern "C"{
#endif


PyObject* init_resources(PyObject* self, PyObject* args) {
  char* base_config_file_name;
  char* resources_net_file_name;
  if(!PyArg_ParseTuple(args, "ss", &base_config_file_name, &resources_net_file_name)) {
    // failed, return 0
    return Py_BuildValue("i",0);;
  }
  if (!InitResources(base_config_file_name, resources_net_file_name)) {
    // failed, return 0
    return Py_BuildValue("i",0);
  }

  // success, return 1
  return Py_BuildValue("i",1);
}

PyObject* release_resources(PyObject* self, PyObject* args) {
  ReleaseResources();
  Py_RETURN_NONE;
}

PyObject* get_words_by_key(PyObject* self, PyObject* args) {
  char* ckey;
  if(!PyArg_ParseTuple(args, "s", &ckey)) {
    // failed, return 0
    return Py_BuildValue("i",0);
  }
  std::string key = ckey;
  std::set<std::string> words = GetWordsByKey(key);
  Py_ssize_t len = words.size();
  PyObject* word_list = PyList_New(len);
  std::set<std::string>::iterator it_words = words.begin();
  int index = 0;
  for (; index < len && it_words != words.end(); index++, it_words++) {
    PyList_SetItem(word_list, index, Py_BuildValue("s", (*it_words).c_str()));
  }
  return word_list; // must decref by outside
}

PyObject* get_sorted_words_by_keys(PyObject* self, PyObject* args) {
  PyObject* list = NULL;  
  if (!PyArg_ParseTuple(args, "O", &list)) {
    // failed, return 0
    std::cout << "PyArg_ParseTuple O" << std::endl;
    return Py_BuildValue("i",0);
  }
  if (!PyList_Check(list)) {
    // failed, return 0
    std::cout << "PyList_Check failed" << std::endl;
    return Py_BuildValue("i",0);
  }

  std::set<std::string> keys;
  Py_ssize_t len = PyList_Size(list);
  // std::cout << "PyList_Size len = " << len << std::endl;
  for (int i = 0; i < len; i++) {
    PyObject* pValue = PyList_GetItem(list, i);	
	// std::cout << "Is Unicode type : " << PyUnicode_Check(pValue) << std::endl;
	// std::cout << "Get unicode length : " << PyUnicode_GetLength(pValue) << std::endl;
	std::string str_utf8 = PyUnicode_AsUTF8(pValue);
	// std::cout << "PyUnicode_AsUTF8 : " << str_utf8 << std::endl;

	// char* data = (char*)PyUnicode_DATA(pValue);
	// std::string str_data = data;
	// std::cout << "PyUnicode_DATA : " << str_data << std::endl;
    if (!str_utf8.empty()) {
      keys.insert(str_utf8);
    }
  }
  if (keys.empty()) {
	return Py_BuildValue("i",0);
  }

  std::map<double, std::set<std::string>, std::greater<double>> sorted_words = GetSortedWordsByKeys(keys);
  int total = 0;
  std::map<double, std::set<std::string>, std::greater<double>>::const_iterator it_sw = sorted_words.begin();
  for (; it_sw != sorted_words.end(); it_sw++) {
    total += it_sw->second.size();
  }
  // std::cout << "Total : " << total << std::endl;
  if (0 == total) {
	return Py_BuildValue("i",0);
  }

  PyObject* sorted_word_list = PyList_New(total);
  int index = 0;
  for (it_sw = sorted_words.begin(); it_sw != sorted_words.end(); it_sw++) {
    double value = it_sw->first;
    std::set<std::string> word_set = it_sw->second;
    std::set<std::string>::const_iterator it_set = word_set.begin();
    for (; it_set != word_set.end(); it_set++) {
      PyList_SetItem(sorted_word_list, index, Py_BuildValue("(sf)", (*it_set).c_str(), value));
      index++;
    }
  }

  return sorted_word_list;
}


PyMethodDef search_words_utilMethods[]= {
  {"init_resources", init_resources, METH_VARARGS, "preload resource files into RAM(DOM object), only need to call it once before starting."},
  {"release_resources", release_resources, METH_VARARGS, "release the DOM object, MUST call it before you exit."},
  {"get_words_by_key", get_words_by_key, METH_VARARGS, "find words from base_config file using a given key."},
  {"get_sorted_words_by_keys", get_sorted_words_by_keys, METH_VARARGS, "search and return sorted words from resources_net file using given keys."},
  {NULL,NULL,0,NULL}
};

struct PyModuleDef search_words_util_module = {
    PyModuleDef_HEAD_INIT,
    "search_words_util", /* name of module */
    "" , /* module documentation, may be NULL */
    -1 , /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    search_words_utilMethods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC PyInit_search_words_util(void) {
  return PyModule_Create(&search_words_util_module);
}



#ifdef __cplusplus
}
#endif


