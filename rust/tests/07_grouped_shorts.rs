// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>

#![feature(assert_matches)]
#![feature(get_mut_unchecked)]

#[cfg(test)]
mod tests {
  use std::assert_matches::assert_matches;
  // use std::rc::Rc;
  // use itertools::Itertools;

  use checkarg::{CheckArg, ValueType, RC};

  #[test]
  fn grouped_short_options() {
    // long options cannot be grouped, so those dont apply
    let argvs = vec![
      vec!["/test07", "-abcd"],
      vec!["/test07", "-ab", "-cd"],
      vec!["/test07", "-abc", "-d"],
      vec!["/test07", "-abc", "--delta"],
      vec!["/test07", "-ab", "--gamma", "-d"],
      vec!["/test07", "-ac", "--beta", "--delta"],
      vec!["/test07", "--alpha", "-bcd"],
    ];

    let mut ca = CheckArg::new("test07");
    ca.add('a', "alpha", "opt a", ValueType::None, None);
    ca.add_cb(
      'b',
      "beta",
      "opt b",
      |_, _, _| Ok(()),
      ValueType::None,
      None,
    );
    ca.add('c', "gamma", "opt gamma", ValueType::None, None);
    ca.add_cb(
      'd',
      "delta",
      "opt delta",
      |_, _, _| Ok(()),
      ValueType::None,
      None,
    );
    ca.add('e', "epsy", "opt e", ValueType::None, None);

    for argv in argvs {
      let ret = ca.parse(&argv);

      assert_matches!(ret, RC::Ok);

      assert!(ca.isset("alpha"));
      assert!(ca.isset("beta"));
      assert!(ca.isset("gamma"));
      assert!(ca.isset("delta"));
      assert!(!ca.isset("epsy"));
    }
  }
}
