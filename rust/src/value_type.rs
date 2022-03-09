// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
use std::fmt;

use crate::ValueType;

impl fmt::Display for ValueType {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    let name = match self {
      ValueType::None => "ValueType::None",
      ValueType::Required => "ValueType::Required",
    };
    write!(f, "{}", name)
  }
}
