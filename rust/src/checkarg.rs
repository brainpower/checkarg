// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#![allow(dead_code)]
use std::collections::BTreeMap;

use crate::{CheckArg, Opt, ValueType, RC};

type CheckArgFP = dyn Fn(&CheckArg, &str, &str) -> Result<(), ()>;

impl<'a> CheckArg<'a> {
  pub fn new(appname: &str) -> Self {
    Self {
      appname:  String::from(appname),
      appendix: None,
      descr:    None,
      usage:    format!("{} [options]", appname),

      posarg_help_descr: None,
      posarg_help_usage: None,

      next_is_val_of: None,

      valid_options: BTreeMap::new(),
      short2long:    BTreeMap::new(),

      callname:   String::new(),
      posargs:    Vec::new(),
      posarg_sep: false,
      cleared:    true,
    }
  }

  /*
  pub fn autonamed(argv: &Vec<&str>) -> Self {
    use std::path::Path;
    let name = args[0]
      .expect("argv must have at least one element");
    Self::new(name);
  }

  pub fn new_with_argv(argv: &Vec<&str>) -> Self {}
  // these 2 constructors dont work well with current API,
  // which has argv passed to parse, instead of passing them to constructors
  // which would enable multiple parsings with the same CheckArg object
  // currently incomplete, since parse should return a Result wit isset() and value()
  // instead of saving the results directly inside CheckArg.
  */

  pub fn add(
    &mut self,
    sopt: char,
    lopt: &str,
    help: &str,
    value_type: ValueType,
    value_name: Option<&str>,
  ) {
    let opt = Opt {
      sopt,
      lopt: String::from(lopt), // not needed?
      help: String::from(help),
      value: None,
      value_name: match value_name {
        None => match &value_type {
          ValueType::None => String::new(),
          _ => lopt.to_uppercase(),
        },
        Some(v) => String::from(v),
      },
      value_type,
      cb: None,
    };
    self.short2long.insert(sopt, opt.lopt.clone());
    self.valid_options.insert(opt.lopt.clone(), opt);
  }

  pub fn add_cb(
    &mut self,
    sopt: char,
    lopt: &str,
    help: &str,
    cb: impl 'a + FnMut(&CheckArg, &str, &str) -> Result<(), ()>,
    value_type: ValueType,
    value_name: Option<&str>,
  ) {
    let opt = Opt {
      sopt,
      lopt: String::from(lopt),
      help: String::from(help),
      value: None,
      value_name: match value_name {
        None => match &value_type {
          ValueType::None => String::new(),
          _ => lopt.to_uppercase(),
        },
        Some(v) => String::from(v),
      },
      value_type,
      cb: Some(Box::new(cb)),
    };
    self.short2long.insert(sopt, opt.lopt.clone());
    self.valid_options.insert(opt.lopt.clone(), opt);
  }

  pub fn add_long(
    &mut self,
    lopt: &str,
    help: &str,
    value_type: ValueType,
    value_name: Option<&str>,
  ) {
    let opt = Opt {
      sopt: 0 as char,
      lopt: String::from(lopt),
      help: String::from(help),
      value: None,
      value_name: match value_name {
        None => match &value_type {
          ValueType::None => String::new(),
          _ => lopt.to_uppercase(),
        },
        Some(v) => String::from(v),
      },
      value_type,
      cb: None,
    };
    self.valid_options.insert(lopt.to_string(), opt);
  }

  pub fn add_long_cb(
    &mut self,
    lopt: &str,
    help: &str,
    cb: impl 'a + FnMut(&CheckArg, &str, &str) -> Result<(), ()>,
    value_type: ValueType,
    value_name: Option<&str>,
  ) {
    let opt = Opt {
      sopt: 0 as char,
      lopt: String::from(lopt),
      help: String::from(help),
      value: None,
      value_name: match value_name {
        None => match &value_type {
          ValueType::None => String::new(),
          _ => lopt.to_uppercase(),
        },
        Some(v) => String::from(v),
      },
      value_type,
      cb: Some(Box::new(cb)),
    };
    self.valid_options.insert(lopt.to_string(), opt);
  }

  pub fn add_autohelp(&mut self) {
    let opt = Opt {
      sopt:       'h',
      lopt:       String::from("help"),
      help:       String::from("show this help message and exit"),
      value:      None,
      value_type: ValueType::None,
      value_name: String::from(""),
      cb:         Some(Box::new(|ca, _k, _v| -> Result<(), ()> {
        ca.show_help();
        std::process::exit(0);
      })),
    };
    self.short2long.insert('h', opt.lopt.clone());
    self.valid_options.insert(opt.lopt.clone(), opt);
  }

  pub fn set_appendix(&mut self, appendix: &str) { self.appendix = Some(String::from(appendix)); }

  pub fn set_description(&mut self, desc: &str) { self.descr = Some(String::from(desc)); }

  pub fn set_posarg_help(&mut self, usage: &str, desc: &str) {
    self.posarg_help_usage = Some(String::from(usage));
    self.posarg_help_descr = Some(String::from(desc));
  }

  pub fn set_usage_line(&mut self, usage: &str) { self.usage = String::from(usage); }

  pub fn callname(&self) -> &str { self.callname.as_str() }

  pub fn argv0(&self) -> &str { &self.callname[..] }

  pub fn pos_args(&self) -> &Vec<String> { &self.posargs }

  pub fn value(&self, option: &str) -> Option<&str> {
    // println!("option: {}", option);
    match self.valid_options.get(option) {
      Some(o) => match &o.value {
        Some(v) => Some(&v[..]),
        None => None,
      },
      None => None,
    }
  }

  pub fn isset(&self, option: &str) -> bool {
    // println!("option: {}", option);
    match self.valid_options.get(option) {
      Some(opt) => match &opt.value {
        Some(_) => true,
        None => false,
      },
      None => false,
    }
  }

  pub fn strerr(code: &RC) -> &'static str {
    match code {
      RC::Ok => "Everything is fine",
      RC::Err => "An error occurred",
      RC::InvOpt => "Unknown command line option",
      RC::InvVal => "Value given to non-value option",
      RC::MissVal => "Missing value of option",
      RC::Callback => "Callback returned with error code",
    }
  }

  fn ca_error(code: RC, message: &str) -> RC {
    eprintln!("Error: {}: {}", Self::strerr(&code), message);
    code
  }

  pub fn usage(&self) -> String {
    match &self.posarg_help_usage {
      None => format!("Usage: {}", self.usage),
      Some(s) => format!("Usage: {} {}", self.usage, s),
    }
  }

  pub fn autohelp(&self) -> String {
    let mut ss: String;
    let mut space: usize = 0;
    for (lopt, opt) in &self.valid_options {
      let vsize: usize = match opt.value_type {
        ValueType::None => 0,
        _ => match opt.value_name.len() {
          0 => 0,
          l => l + 1, // account for the '=' sign
        },
      };
      space = std::cmp::max(space, vsize + lopt.len());
    }
    space += 2; // add space between options and help messages

    ss = format!("{}\n", self.usage());
    // generously allocate space, shrink_to_fit later,
    ss.reserve(self.valid_options.len() * (space + 255));

    // add description if exists
    if let Some(desc) = &self.descr {
      ss += format!("\n{}\n", desc).as_str();
    }

    // add options with their help messages
    ss.push_str("\nOptions:\n");
    for (lopt, opt) in &self.valid_options {
      match opt.sopt {
        '\0' => ss += "      ",
        _ => ss += format!("   -{},", opt.sopt).as_str(),
      }
      ss += " --";
      ss += lopt.as_str();
      ss += match opt.value_type {
        ValueType::None => std::iter::repeat(' ')
          .take(space - lopt.len())
          .collect::<String>(),
        ValueType::Required => match opt.value_name.len() {
          0 => std::iter::repeat(' ')
            .take(space - lopt.len())
            .collect::<String>(),
          x => format!("={}{:2$}", opt.value_name, " ", space - x - 1 - lopt.len()),
        },
      }
      .as_str();
      ss += opt.help.as_str();
      ss += "\n";
    }

    if let Some(ph) = &self.posarg_help_descr {
      ss += format!("\nPositional Arguments:\n{}\n", ph).as_str();
    }
    if let Some(ap) = &self.appendix {
      ss += format!("\n{}\n", ap).as_str()
    }

    ss.shrink_to_fit();
    ss
  }

  pub fn show_help(&self) {
    println!("{}", self.autohelp());
  }

  pub fn show_usage(&self) {
    println!("{}", self.usage());
  }

  fn produce_error(&self) -> RC { RC::Err }

  pub fn reset(&mut self) {
    // clear and reset positional args in case of reuse / a second parse
    self.posarg_sep = false;
    self.posargs.clear();
    self.next_is_val_of = None;
    for (_, opt) in &mut self.valid_options {
      opt.value = None;
    }
    self.cleared = true;
  }

  pub fn parse(&mut self, argv: &Vec<&str>) -> RC {
    let mut ret = RC::Ok;

    assert!(argv.len() > 0, "argv must have at least one element"); // the "callname"

    // clear and reset state in case of reuse / a second parse
    if !self.cleared {
      self.reset();
    }
    self.cleared = false;

    self.callname = argv
      .first()
      .expect("argv must have at least one element")
      .to_string();

    for arg in &argv[1..] {
      //println!("Found: {:?}", arg);
      ret = self.arg(arg);
    }
    if matches!(ret, RC::Ok) && !matches!(self.next_is_val_of, None) {
      self.next_is_val_of = None; // reset before returning, in case of reuse / a second parse
      return Self::ca_error(
        RC::MissVal,
        match argv.last() {
          Some(s) => s,
          _ => "",
        },
      );
    }
    ret
  }

  fn arg(&mut self, arg: &str) -> RC {
    if !self.posarg_sep {
      let next_is_val_of = self.next_is_val_of.take();
      //println!("next_is_val_of: {:?}, arg: {}", next_is_val_of, arg);

      // arg is expected to be the value of the previous option
      if let Some(lopt) = &next_is_val_of {
        //println!("is value of {:?}: {:?}", lopt, arg);
        let opt = self
          .valid_options
          .get_mut(lopt)
          .expect("BUG: next_is_val_of should always contain a valid option or None");
        opt.value = Some(String::from(arg));

        let ret = self.call_cb(&lopt);
        self.next_is_val_of = None;
        return ret;
      }

      // check for options
      if arg.starts_with('-') {
        if arg.starts_with("--") {
          //println!("is long opt: {:?}", arg);
          return self.arg_long(&arg[2..]);
        }
        //println!("is short opt: {:?}", arg);
        return self.arg_short(&arg[1..]);
      }
    }

    //println!("debug: adding posarg: {}", arg);
    // everything else is a positional argument
    self.posargs.push(String::from(arg));
    RC::Ok
  }

  fn arg_short(&mut self, arg: &str) -> RC {
    for (i, c) in arg.char_indices() {
      let lopt = match self.short2long.get(&c) {
        None => return Self::ca_error(RC::InvOpt, String::from(c).as_str()),
        Some(lopt) => lopt,
      };
      let opt = self
        .valid_options
        .get_mut(lopt)
        .expect("BUG: this should never happen");
      // println!("{}: fount matching long opt: {}", c, lopt);

      if let ValueType::None = opt.value_type {
        //println!("short opt has no value: -{}", arg);
        opt.value = Some(String::new());
        let cbopt = lopt.clone();
        if let RC::Callback = self.call_cb(&cbopt) {
          return RC::Callback;
        }
      }
      else {
        // println!("{}: short opt has value: {}", c, arg);
        // option has value, treat the rest of arg as value
        // if we're not at the end
        let mut j = i + 1;
        //println!("j: {}, i: {}, arg.len(): {}", j, i, arg.len());
        if j >= arg.len() {
          // println!("{}: value is next arg", c);
          self.next_is_val_of = Some(lopt.clone());
        }
        else {
          //println!("value stuck to option: -{}", arg);
          while !arg.is_char_boundary(j) {
            j += 1;
          }

          opt.value = Some(String::from(&arg[j..]));

          let cbopt = lopt.clone();
          return self.call_cb(&cbopt);
        }
        return RC::Ok;
      }
    }
    RC::Ok
  }

  fn arg_long(&mut self, arg: &str) -> RC {
    // println!("longarg: {}", arg);
    if arg.is_empty() {
      // got '--', the positional args separator
      self.posarg_sep = true;
      return RC::Ok;
    }

    let (opt, val) = match arg.split_once('=') {
      None => (arg, None),
      Some((o, v)) => (o, Some(v)),
    };
    //println!("opt: {}, val: {}", opt, val.unwrap());

    let vopt = match self.valid_options.get_mut(opt) {
      None => return Self::ca_error(RC::InvOpt, format!("--{}!", arg).as_str()),
      Some(o) => o,
    };

    if let ValueType::Required = vopt.value_type {
      // println!("{}: has required value", opt);
      if let Some(val) = val {
        // value was given using '='
        vopt.value = Some(val.to_string());
        // println!("{}: value was given using '='", opt);
        return self.call_cb(&opt.to_string());
      }
      else {
        // value is in the next arg, clone because of borrow and lifetime checker :(
        // opts will live as long as CheckArg anyway, there is no removal functions!
        // but I have not found a good way to convince rustc of that :/
        self.next_is_val_of = Some(vopt.lopt.clone());
      }
    }
    else {
      // option has no value
      if val.is_some() {
        // but a value was given!
        return Self::ca_error(RC::InvVal, format!("--{}!", opt).as_str());
      }

      vopt.value = Some(String::new());

      if self.next_is_val_of.is_none() {
        return self.call_cb(&opt.to_string());
      }
    }
    RC::Ok
  }

  fn call_cb(&mut self, lopt: &String) -> RC {
    let opt = self
      .valid_options
      .get_mut(lopt)
      // we have checked the existence of lopt before using it here
      // possible to pass value as parameter here?
      .expect("BUG: this should always get a valid lopt");
    let val = match &opt.value {
      None => String::new(),
      Some(v) => v.clone(),
    };

    let callback = opt.cb.take();
    match callback {
      None => RC::Ok,
      Some(mut cb) => match cb(self, lopt.as_str(), val.as_str()) {
        Result::Ok(()) => RC::Ok,
        Result::Err(()) => RC::Callback,
      },
    }
  }
}
