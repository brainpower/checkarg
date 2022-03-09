// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
use std::fmt;
use std::process::ExitCode;
use std::process::Termination;

use crate::CheckArg;
use crate::RC;

impl Termination for RC {
  fn report(self) -> ExitCode { ExitCode::from(self as u8) }
}

impl fmt::Display for RC {
  fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
    write!(f, "RC::{:?}: {}", &self, CheckArg::strerr(&self))
  }
}
