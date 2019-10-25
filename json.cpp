#ifndef JSON_CPP
#define JSON_CPP

#include "json.h"

std::string json_element::json_string(std::string string) {
	std::string res = "\"";
	for (auto it=string.begin(), end=string.end(); it!=end; ++it) {
		switch (*it) {
			case '\\': {
				res += "\\\\";
			} break;
			case '\b': {
				res += "\\b";
			} break;
			case '\f': {
				res += "\\f";
			} break;
			case '\n': {
				res += "\\n";
			} break;
			case '\r': {
				res += "\\r";
			} break;
			case '\t': {
				res += "\\t";
			} break;
			case '"': {
				res += "\\\"";
			} break;
			default: res += *it;
		}
	}
	return res + '"';
}

json_element::json_element() {
	type_code = JSON_NULL;
}

json_element::json_element(double number) {
	content.number = number;
	this->type_code = JSON_NUMBER;
}

json_element::json_element(bool boolean) {
	content.boolean = boolean;
	this->type_code = JSON_BOOLEAN;
}

json_element::json_element(std::string* string) {
	content.string = string;
	this->type_code = JSON_STRING;
}

json_element::json_element(std::vector <json_element*>* array) {
	content.array = array;
	this->type_code = JSON_ARRAY;
}

json_element::json_element(std::unordered_map <std::string, json_element*>* object) {
	content.object = object;
	this->type_code = JSON_OBJECT;
}

std::string json_element::type() {
	switch (type_code) {
		case JSON_BOOLEAN: return "boolean";
		case JSON_NUMBER:  return "number";
		case JSON_STRING:  return "string";
		case JSON_NULL:    return "null";
		case JSON_ARRAY:   return "array";
		case JSON_OBJECT:  return "object";
	}
	return "undefined";
}

json_element* json_element::attr(std::string name) {
	if (type_code != JSON_OBJECT) {
		return nullptr;
	}
	std::unordered_map <std::string, json_element*> &object = *content.object;
	if (object.find(name) == object.end()) {
		return nullptr;
	}
	return object[name];
}

json_element* json_element::get(int index) {
	if (type_code != JSON_ARRAY) return nullptr;
	std::vector <json_element*> &array = *content.array;
	if (index < 0 || index >= (int)array.size()) return nullptr;
	return array[index];
}

int json_element::length() {
	if (type_code != JSON_ARRAY) return -1;
	return content.array->size();
}

bool json_element::is_boolean() {
	return type_code == JSON_BOOLEAN;
}

bool json_element::is_number() {
	return type_code == JSON_NUMBER;
}

bool json_element::is_string() {
	return type_code == JSON_STRING;
}

bool json_element::is_null() {
	return type_code == JSON_NULL;
}

bool json_element::is_array() {
	return type_code == JSON_ARRAY;
}

bool json_element::is_object() {
	return type_code == JSON_OBJECT;
}

bool json_element::boolean_val() {
	return content.boolean;
}

double json_element::number_val() {
	return content.number;
}

std::string json_element::string_val() {
	return *content.string;
}

std::unordered_map <std::string, json_element*>::iterator json_element::begin() {
	return content.object->begin();
}

std::unordered_map <std::string, json_element*>::iterator json_element::end() {
	return content.object->end();
}

std::string json_element::toString() {
	switch (type_code) {
		case JSON_BOOLEAN: {
			return content.boolean? "true": "false";
		};
		case JSON_NULL: {
			return "null";
		};
		case JSON_NUMBER: {
			char str[64];
			sprintf(str, "%g", content.number);
			return std::string(str);
		};
		case JSON_STRING: {
			return json_string(*content.string);
		};
		case JSON_ARRAY: {
			std::string res = "[";
			int i = 0;
			for (auto it=content.array->begin(), end=content.array->end(); it!=end; ++it, ++i) {
				if (i) res += ",";
				res += (*it)->toString();
			}
			return res + "]";
		};
		case JSON_OBJECT: {
			std::string res = "{";
			int i = 0;
			for (auto it=content.object->begin(), end=content.object->end(); it!=end; ++it, ++i) {
				if (i) res += ",";
				res += json_string(it->first);
				res += ":" + it->second->toString();
			}
			return res + "}";
		};
	}
	return "undefined";
}

json_element::~json_element() {
	switch (type_code) {
		case JSON_STRING:
			delete content.string;
		break;
		case JSON_ARRAY: {
			std::vector <json_element*> &array = *content.array;
			for (auto i=array.begin(), e=array.end(); i!=e; ++i) {
				delete *i;
			}
			delete content.array;
		} break;
		case JSON_OBJECT:
			std::unordered_map <std::string, json_element*> &object = *content.object;
			for (auto i=object.begin(), e=object.end(); i!=e; ++i) {
				delete i->second;
			}
			delete content.object;
		break;
	}
}

json_parse_result::json_parse_result(const char* src, int err_index, json_element* res) {
	this->src = src;
	this->err_index = err_index;
	this->res = res;
}

int json_parse_result::error_index() {
	return err_index;
}

json_element* json_parse_result::result() {
	return res;
}

void json_parse_result::count_row_col(int &row, int &col) {
	int r = 1;
	int c = 1;
	int e = err_index;
	const char* ptr = src;
	for (int i=0; i<e; ++i) {
		if (ptr[i] == '\n') {
			c = 1;
			++ r;
		} else {
			++ c;
		}
	}
	row = r;
	col = c;
}

json_parse_result::~json_parse_result() {
	if (res) delete res;
}

char json::is_whitespace[256] = {
	0, 0, 0, 0,	0, 0, 0, 0,
	0, 1, 1, 0,	0, 1, 0, 0,
	0, 0, 0, 0,	0, 0, 0, 0,
	0, 0, 0, 0,	0, 0, 0, 0,
	1
};

bool json::skip_whitespace(const char* &src_ptr) {
	const char* ptr = src_ptr;
	for (;;) {
		while (is_whitespace[(unsigned char)*ptr]) ++ ptr;
		if (*ptr != '/') break;
		char chr = *++ptr;
		if (chr == '*') {
			for (++ptr;;) {
				while ((chr=*ptr) && chr != '*') ++ ptr;
				if (!chr) {
					src_ptr = ptr;
					return false;
				}
				if (*++ptr == '/') {
					++ ptr;
					break;
				}
			}
		} else if (chr == '/') {
			while ((chr=*++ptr) && chr != '\n');
			if (chr) ++ ptr;
		} else {
			src_ptr = ptr;
			return false;
		}
	}
	src_ptr = ptr;
	return true;
}

bool json::parse_string(const char* &src_ptr, std::string &string) {
	const char* ptr = src_ptr;
	char chr;
	for (;;) {
		switch (chr=*++ptr) {
			case '"': {
				ptr = ptr + 1;
				src_ptr = ptr;
				return true;
			}
			case '\0': {
				src_ptr = ptr;
				return false;
			}
			case '\\': {
				switch (*++ptr) {
					case '"': string += '"'; break;
					case '\\': string += '\\'; break;
					case '/': string += '/'; break;
					case 'b': string += '\b'; break;
					case 'f': string += '\f'; break;
					case 'n': string += '\n'; break;
					case 'r': string += '\r'; break;
					case 't': string += '\t'; break;
					case 'u': {
						int i = 4;
						unsigned int v = 0;
						while (i--) {
							chr = *++ptr;
							v <<= 4;
							if (chr >= 'a' && chr <= 'z') {
								v |= (chr - ('a' - 10));
							} else if (chr >= 'A' && chr <= 'Z') {
								v |= (chr - ('A' - 10));
							} else if (chr >= '0' && chr <= '9') {
								v |= chr & 15;
							} else {
								src_ptr = ptr;
								return false;
							}
						}
						if (v <= 0b01111111) {
							string += (char) v;
						} else if (v <= 0b11111111111) {
							string += 0b11000000 | (v >> 6);
							string += 0b10000000 | (v & 0b00111111);
						} else {
							string += 0b11100000 | (v >> 12);
							string += 0b10000000 | ((v >> 6) & 0b00111111);
							string += 0b10000000 | ((   v  ) & 0b00111111);
						}
					} break;
					default: {
						src_ptr = ptr;
						return false;
					}
				}
			} break;
			default: string += chr; break;
		}
	}
}

json_element* json::parse_null(const char* &src_ptr) {
	const char* ptr = src_ptr;
	static char str_null[] = "null";
	char* cmp = str_null;
	while (*++cmp) {
		if (*cmp != *++ptr) {
			src_ptr = ptr;
			return nullptr;
		}
	}
	++ ptr;
	src_ptr = ptr;
	return new json_element();
}

json_element* json::parse_boolean(const char* &src_ptr) {
	const char* ptr = src_ptr;
	static char str_true[] = "true";
	static char str_false[] = "false";
	char* cmp = *ptr == 't'? str_true: str_false;
	while (*++cmp) {
		if (*cmp != *++ptr) {
			src_ptr = ptr;
			return nullptr;
		}
	}
	++ ptr;
	json_element* res = new json_element(ptr[-2] == 'u');
	src_ptr = ptr;
	return res;
}

json_element* json::parse_number(const char* &src_ptr) {
	const char* ptr = src_ptr;
	if (*ptr == '0') {
		src_ptr = ptr + 1;
		return new json_element((double)0);
	}
	int i = (*ptr) & 15;
	char chr;
	for (;;) {
		chr = *++ptr;
		if (chr < '0' || chr > '9') break;
		i = i*10 + (chr&15);
	}
	double val = i;
	if (*ptr == '.') {
		double f = 0;
		double p = 1;
		for (;;) {
			chr = *++ptr;
			if (chr < '0' || chr > '9') break;
			f = f*10 + (chr&15);
			p *= 10;
		}
		if (p == 1) {
			src_ptr = ptr;
			return nullptr;
		}
		val += f/p;
	}
	chr = *ptr;
	if (chr == 'E' || chr == 'e') {
		int neg = 0;
		chr = *++ptr;
		if (chr == '+') {
			chr = *++ptr;
		} else if (chr == '-') {
			chr = *++ptr;
			neg = 1;
		}
		if (chr < '0' || chr > '9') {
			src_ptr = ptr;
			return nullptr;
		}
		int p = chr&15;
		for (;;) {
			chr = *++ptr;
			if (chr < '0' || chr > '9') break;
			p = p*10 + (chr&15);
		}
		if (neg) p = -p;
		val = val*pow(10, p);
	}
	json_element* res = new json_element(val);
	src_ptr = ptr;
	return res;
}

json_element* json::parse_string(const char* &src_ptr) {
	const char* ptr = src_ptr;
	std::string* string = new std::string;
	if (parse_string(ptr, *string)) {
		src_ptr = ptr;
		return new json_element(string);
	}
	delete string;
	src_ptr = ptr;
	return nullptr;
}

json_element* json::parse_value(const char* &src_ptr) {
	const char* ptr = src_ptr;
	json_element* res = nullptr;
	switch (*ptr) {
		case 'n': res = parse_null(ptr); break;
		case 't': res = parse_boolean(ptr); break;
		case 'f': res = parse_boolean(ptr); break;
		case '0': res = parse_number(ptr); break;
		case '1': res = parse_number(ptr); break;
		case '2': res = parse_number(ptr); break;
		case '3': res = parse_number(ptr); break;
		case '4': res = parse_number(ptr); break;
		case '5': res = parse_number(ptr); break;
		case '6': res = parse_number(ptr); break;
		case '7': res = parse_number(ptr); break;
		case '8': res = parse_number(ptr); break;
		case '9': res = parse_number(ptr); break;
		case '"': res = parse_string(ptr); break;
		case '[': res = parse_array(ptr); break;
		case '{': res = parse_object(ptr); break;
	}
	if (res && !skip_whitespace(ptr)) {
		delete res;
		res = nullptr;
	}
	src_ptr = ptr;
	return res;
}

json_element* json::parse_array(const char* &src_ptr) {
	const char* ptr = src_ptr + 1;
	if (!skip_whitespace(ptr)) {
		src_ptr = ptr;
		return nullptr;
	}
	std::vector <json_element*>* array;
	array = new std::vector <json_element*>();
	if (*ptr == ']') {
		++ ptr;
	} else {
		for (;;) {
			json_element* item = parse_value(ptr);
			if (!item) goto failed;
			array->push_back(item);
			if (*ptr == ',') {
				++ ptr;
				if (!skip_whitespace(ptr)) {
					goto failed;
				}
				continue;
			}
			if (*ptr == ']') {
				++ ptr;
				break;
			}
			goto failed;
		}
	}
	src_ptr = ptr;
	return new json_element(array);
	failed:;
	for (auto it=array->begin(), end=array->end(); it!=end; ++it) {
		delete *it;
	}
	delete array;
	src_ptr = ptr;
	return nullptr;
}

json_element* json::parse_object(const char* &src_ptr) {
	const char* ptr = src_ptr + 1;
	if (!skip_whitespace(ptr)) {
		src_ptr = ptr;
		return nullptr;
	}
	std::unordered_map <std::string, json_element*>* object;
	object = new std::unordered_map <std::string, json_element*>();
	if (*ptr == '}') {
		++ptr;
	} else {
		for (;;) {
			if (*ptr != '"') goto failed;
			std::string name;
			if (!parse_string(ptr, name)) {
				goto failed;
			}
			if (!skip_whitespace(ptr)) goto failed;
			if (*ptr != ':') goto failed;
			++ ptr;
			if (!skip_whitespace(ptr)) goto failed;
			json_element* item = parse_value(ptr);
			if (!item) goto failed;
			(*object)[name] = item;
			char chr = *ptr;
			if (chr == ',') {
				++ ptr;
				if (!skip_whitespace(ptr)) goto failed;
				continue;
			}
			if (chr == '}') {
				++ ptr;
				break;
			}
			goto failed;
		}
	}
	src_ptr = ptr;
	return new json_element(object);
	failed:;
	for (auto it=object->begin(), end=object->end(); it!=end; ++it) {
		delete it->second;
	}
	delete object;
	src_ptr = ptr;
	return nullptr;
}

json_parse_result json::parse(const char* ptr) {
	const char* src = ptr;
	json_element* res = nullptr;
	if (skip_whitespace(ptr)) {
		res = parse_value(ptr);
	}
	if (res && *ptr) {
		delete res;
		res = nullptr;
	}
	return json_parse_result(src, ptr - src, res);
}

std::string json::stringify(std::string str) {
	auto i = str.begin();
	auto e = str.end();
	std::string res = "\"";
	for (; i != e; ++i) {
		unsigned char chr = *i;
		if (chr > 31) {
			if (chr < 128) {
				if (chr == '"') {
					res += "\\\"";
				} else {
					res += (char) chr;
				}
				continue;
			} else {
				goto to_hex;
			}
		}
		switch (chr) {
			case 10: str += "\\n"; break;
			case  9: str += "\\t"; break;
			case 13: str += "\\r"; break;
			case  8: str += "\\b"; break;
			case 12: str += "\\f"; break;
			default: goto to_hex;
		}
		continue;
		to_hex:
		res += "\\u00";
		char h0 = chr & 0xf;
		char h1 = chr >> 4;
		res += h1 < 10? (h1 | '0'): (h1 + ('a' - 10));
		res += h0 < 10? (h0 | '0'): (h0 + ('a' - 10));
	}
	return res + '"';
}

#endif