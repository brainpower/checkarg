// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#![feature(termination_trait_lib)]
#![feature(process_exitcode_placeholder)]
#![feature(assert_matches)]

use std::collections::BTreeMap;

mod checkarg;
mod opt;
mod rc;
mod value_type;

#[derive(Debug)]
pub enum RC {
  Ok,
  Err,
  InvOpt,
  InvVal,
  MissVal,
  Callback,
}

#[derive(Debug)]
pub enum ValueType {
  None,
  Required,
  //Optional,
}

#[derive(Debug)]
pub struct CheckArg<'a> {
  #[allow(dead_code)]
  appname:  String,
  appendix: Option<String>,
  descr:    Option<String>,
  usage:    String,

  posarg_help_descr: Option<String>,
  posarg_help_usage: Option<String>,

  valid_options: BTreeMap<String, Opt<'a>>,
  short2long:    BTreeMap<char, String>,

  callname:   String,
  posargs:    Vec<String>,
  posarg_sep: bool,

  next_is_val_of: Option<String>, // change to &str or &String of lopt, e.g. a key of valid_options
}

// #[derive(Debug)]
struct Opt<'a> {
  sopt:       char,
  lopt:       String,
  help:       String,
  value:      Option<String>,
  value_name: String,
  value_type: ValueType,
  cb:         Option<Box<dyn FnMut(&CheckArg, &str, &str) -> Result<(), ()> + 'a>>,
}
