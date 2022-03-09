// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>

extern "C" {
#include "checkarg.h"
}

#include <cstring>
#include <iterator>  // needed for std::ostram_iterator
#include <memory>
#include <ostream>
#include <string>
#include <vector>

template<typename T>
std::ostream &
operator<<(std::ostream &out, const std::vector<T> &v) {
  if (!v.empty()) {
    out << '[' << '"';
    std::copy(v.cbegin(), v.cend(), std::ostream_iterator<T>(out, "\", \""));
    out << "\b\b\b]";
  }
  return out;
}

// include this after the operator<< so catch will pick it up
#include "catch2/catch.hpp"

using std::string;
using std::unique_ptr;
using std::vector;

typedef std::unique_ptr<CheckArg, decltype(&checkarg_free)> CheckArgUPtr;
