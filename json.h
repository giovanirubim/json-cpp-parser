#ifndef JSON_H
#define JSON_H

#include <cmath>
#include <cstring>
#include <stdint.h>

#include <string>
#include <vector>
#include <unordered_map>

#define JSON_BOOLEAN 0x1
#define JSON_NUMBER  0x2
#define JSON_STRING  0x3
#define JSON_NULL    0x4
#define JSON_ARRAY   0x5
#define JSON_OBJECT  0x6

class json_element {
private:

	int type_code;
	union {
		bool boolean;
		double number;
		std::string* string;
		std::vector <json_element*>* array;
		std::unordered_map <std::string, json_element*>* object;
	} content;

	static std::string json_string(std::string string);

public:

	json_element();
	
	json_element(double number);
	
	json_element(bool boolean);
	
	json_element(std::string* string);
	
	json_element(std::vector <json_element*>* array);
	
	json_element(std::unordered_map <std::string, json_element*>* object);
	
	std::string type();
	
	json_element* attr(std::string name);
	
	json_element* get(int index);
	
	int length();
	
	bool is_boolean();
	
	bool is_number();
	
	bool is_string();
	
	bool is_null();
	
	bool is_array();
	
	bool is_object();
	
	bool boolean_val();
	
	double number_val();
	
	std::string string_val();
	
	// Object attribute iterator
	std::unordered_map <std::string, json_element*>::iterator begin();
	
	// Object attribute iterator
	std::unordered_map <std::string, json_element*>::iterator end();

	std::string toString();
	
	~json_element();

};

class json_parse_result {
private:

	char const* src;
	int err_index;
	json_element* res;

public:

	json_parse_result(char const* src, int err_index, json_element* res);

	int error_index();

	json_element* result();

	void count_row_col(int &row, int &col);

	~json_parse_result();

};

class json {
private:

	static char is_whitespace[256];

	static bool skip_whitespace(const char* &ptr);

	static bool skip_comment(const char* &ptr);

	static bool parse_string(const char* &ptr, std::string &string);

	static json_element* parse_null(const char* &ptr);

	static json_element* parse_boolean(const char* &ptr);

	static json_element* parse_number(const char* &ptr);

	static json_element* parse_string(const char* &ptr);

	static json_element* parse_value(const char* &ptr);

	static json_element* parse_array(const char* &ptr);

	static json_element* parse_object(const char* &ptr);

public:

	static json_parse_result parse(const char* ptr);

};

#endif