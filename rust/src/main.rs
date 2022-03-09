// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#![feature(termination_trait_lib)]
use checkarg::{CheckArg, ValueType, RC};

fn main() -> RC {
  let mut ca = CheckArg::new("test");
  ca.add('a', "alpha", "option alpha", ValueType::Required, None);
  ca.add_long("beta", "option beta", ValueType::None, None);
  ca.add_cb(
    'c',
    "gamma",
    "option gamma",
    |_ca, o, _v| {
      println!("cb: {}", o);
      Ok(())
    },
    ValueType::None,
    None,
  );
  ca.add_autohelp();

  let ret = ca.parse(&vec!["-catest", "1", "2", "3"]);
  println!("{}", ca.autohelp());
  println!("{}", ret);
  println!("{:?}", ca.pos_args());
  ret
}
