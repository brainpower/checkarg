// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
use crate::Opt;

impl<'a> std::fmt::Debug for Opt<'a> {
  fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
    write!(
      f,
      "Opt {{\n  sopt: {:?},\n  lopt: {:?},\n  help: {:?},\n  value: {:?},\n  value_type: {},\n  \
       value_name: {:?}\n}}",
      self.sopt, self.lopt, self.help, self.value, self.value_type, self.value_name,
    )
  }
}
