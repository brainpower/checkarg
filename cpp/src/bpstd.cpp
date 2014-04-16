#include "bpstd.hpp"
namespace bpstd{ // some basic functions

// a simple format-string, for type-safety and more, use boost::format
std::string stringf(const char* format, ...){
  va_list arg_list;
  va_start(arg_list, format);

  // SUSv2 version doesn't work for buf NULL/size 0, so try printing
  // into a small buffer that avoids the double-rendering and alloca path too...
  char short_buf[256];
  const size_t needed = vsnprintf(short_buf, sizeof short_buf,
                                  format, arg_list) + 1;
  if (needed <= sizeof short_buf)
    return short_buf;

  // need more space...
  char* p = static_cast<char*>(alloca(needed));
  vsnprintf(p, needed, format, arg_list);
  return p;
}

  // split a string into lines using getline
void string_getlines(std::deque<std::string> &v, const std::string &str){
  std::stringstream stream(str);
  while (1){
    std::string line;
    std::getline(stream,line);
    if(line != "")
      v.push_back(line);
    if (!stream.good())
      break;
  }
}

  // split a string into lines using getline
std::deque<std::string> string_getlines( const std::string &str){
  std::deque<std::string> v;
  std::stringstream stream(str);
  while (1){
    std::string line;
    std::getline(stream,line);

    if(line != "")
      v.push_back(line);
    if (!stream.good())
      break;
  }
  return v;
}

  // split string at every occurance of "c"
void string_split(std::deque<std::string> &v, const char c, const std::string &str){
  int pos = 0;
  for(uint i = 0; i<str.size(); ++i){
    if(str[i] == c){
      if(i-pos > 0 || i == 0)
	      v.push_back(str.substr(pos, i-pos));
      pos = i+1;
    }
  }
  if(str.size()-pos > 0)
    v.push_back(str.substr(pos, str.size()-pos)); // add rest
}

// split string at every occurance of "c"
std::deque<std::string> string_split(const char c, const std::string &str){
  std::deque<std::string> v;
  int pos = 0; bool found = false;
  for(uint i = 0; i<str.size(); ++i){
    if(str[i] == c){
      found = true;
      if(i-pos > 0 || i == 0)
	v.push_back(str.substr(pos, i-pos));
      pos = i+1;
    }
  }
  if(str.size()-pos > 0)
    v.push_back(str.substr(pos, str.size()-pos)); // add rest
  return v;
}

void string_join(std::string &res, const char c, const std::deque<std::string> &v){
  res = v[0];
  for(uint i = 1; i<v.size(); ++i){
    res += c + v[i];
  }
}
void string_join(std::string &res, const std::deque<std::string> &v){
  res = "";
  for(auto &str : v){
    res += str;
  }
}
std::string string_join(const char c, const std::deque<std::string> &v){
  std::string res = v[0];
  for(uint i = 1; i<v.size(); ++i){
    res += c + v[i];
  }
  return res;
}
std::string string_join(const std::deque<std::string> &v){
  std::string res = "";
  for(auto &str : v){
    res += str;
  }
  return res;
}
std::string string_join(const std::deque<std::string> &v, uint s){
  std::string res = "";
  for(uint i = s; s<v.size(); ++i){
    res += v[i];
  }
  return res;
}
std::string string_join(const std::deque<std::string> &v, uint s, uint e){
  std::string res = "";
  for(uint i = s; s<e; ++i){
    res += v[i];
  }
  return res;
}

// strip line-comments out of an string.
// Here, only a single char can be used as delimiter
void strip_line_comments(std::string &s, const char c, const std::string &raw){
  std::deque<std::string> lines;
  string_getlines(lines, raw);
  for(auto &line : lines){
    std::deque<std::string> tmp;
    string_split(tmp, c, line);
    if(!tmp.empty())
      s += tmp[0] + '\n';
  }
}
// strip line-comments out of an string.
// Here, only a single char can be used as delimiter
std::string strip_line_comments(const char c, const std::string &raw){
  std::string s;
  std::deque<std::string> lines;
  string_getlines(lines, raw);
  for(auto &line : lines){
    std::deque<std::string> tmp;
    string_split(tmp, c, line);
    if(!tmp.empty())
      s += tmp[0] + '\n';
  }
  return s;
}


} // namespace bpstd
