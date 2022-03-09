// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#![feature(assert_matches)]

#[cfg(test)]
mod tests {
  use itertools::Itertools;
  use std::assert_matches::assert_matches;

  use checkarg::{CheckArg, ValueType, RC};

  #[test]
  fn general_usage() {
    let argv = vec!["/test04", "-i", "input.bin", "--", "file1", "file2"];

    let mut ca = CheckArg::new("test04");
    ca.add('i', "input", "file to read from", ValueType::Required, None);

    let rc = ca.parse(&argv);

    assert_matches!(rc, RC::Ok, "parsing failed");

    assert_eq!(
      ca.pos_args(),
      &vec!("file1".to_string(), "file2".to_string())
    );
  }

  #[test]
  fn args_with_dashes() {
    // these -- and - args must not be interpreted as options
    // if they appear after a --
    // when parsed as options these would trigger RC::InvOpt
    // hence failing the assert_matches!() below
    let all = vec![
      "--file1", "file1", "-f", "-fg", "--file2", "file2", "-g", "-hi", "--file3", "file3", "-i",
      "-jk",
    ];

    let mut ca = CheckArg::new("test04");
    ca.add('i', "input", "file to read from", ValueType::Required, None);

    let it = all.into_iter().permutations(3);
    for args in it {
      let argv = vec![
        "/test04",
        "-i",
        "input.bin",
        "--",
        args[0],
        args[1],
        args[2],
      ];
      //eprintln!("argv: {:?}", argv);
      let ret = ca.parse(&argv);

      assert_matches!(ret, RC::Ok, "parsing failed");
      assert_eq!(ca.pos_args(), &vec!(args[0], args[1], args[2]),);
    }
  }

  #[test]
  fn missing_value_before_double_dash() {
    // if the last option before a '--' has a value which was not given
    // the "--" shall be the value instead of the separator
    // the '-j' must be interpreted as an option
    //
    // Maybe consider an alternative here, though:
    // always have '--' be the separator
    // which would trigger a CA_MISSVAL and not interpret the '-j'
    // This is probably closer to the users intention

    let argv = vec!["/test04", "-i", "--", "file1", "-j", "file2"];

    let mut ca = CheckArg::new("test04");
    ca.add('i', "input", "file to read from", ValueType::Required, None);
    ca.add('j', "jay", "just a jay", ValueType::None, None);

    let rc = ca.parse(&argv);

    // should it be this?
    //assert_matches!(rc, RC::MissVal, "parsing failed");
    //assert!(!ca.isset("jay"));

    // or this?
    assert_matches!(rc, RC::Ok, "parsing failed");
    assert_eq!(ca.value("input").unwrap(), "--");
    assert!(ca.isset("jay"));

    assert_eq!(
      ca.pos_args(),
      &vec!("file1".to_string(), "file2".to_string())
    );
  }
}
