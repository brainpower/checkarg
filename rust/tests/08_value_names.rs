// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#![feature(assert_matches)]

#[cfg(test)]
mod tests {
  use checkarg::{CheckArg, ValueType, RC};
  use std::assert_matches::assert_matches;

  #[test]
  fn general_usage() {
    let argv = vec!["/test08"];

    let mut ca = CheckArg::new("test08");
    ca.add_autohelp();
    ca.add('z', "ag", "auto-generated name", ValueType::Required, None);
    ca.add('y', "ng", "no value name", ValueType::Required, Some(""));
    ca.add(
      'i',
      "input",
      "file to read from",
      ValueType::Required,
      Some("IPT"),
    );
    ca.add('a', "alpha", "alpha option", ValueType::Required, Some("A"));
    ca.add_long("beta", "beta option", ValueType::Required, Some("B"));

    ca.add_cb(
      'c',
      "gamma",
      "gamma option",
      |_ca, _k, _v| Ok(()),
      ValueType::Required,
      Some("C"),
    );
    ca.add_long_cb(
      "delta",
      "delta option",
      |_ca, _k, _v| Ok(()),
      ValueType::Required,
      Some("D"),
    );

    let rc = ca.parse(&argv);

    assert_matches!(rc, RC::Ok, "parsing failed");

    assert_eq!(
      ca.autohelp(),
      "\
Usage: test08 [options]

Options:
   -z, --ag=AG      auto-generated name
   -a, --alpha=A    alpha option
       --beta=B     beta option
       --delta=D    delta option
   -c, --gamma=C    gamma option
   -h, --help       show this help message and exit
   -i, --input=IPT  file to read from
   -y, --ng         no value name\n"
    );
  }
}
