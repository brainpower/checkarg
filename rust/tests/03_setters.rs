// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#![feature(assert_matches)]

#[cfg(test)]
mod tests {
  use checkarg::{CheckArg, ValueType, RC};
  use std::assert_matches::assert_matches;

  #[test]
  fn positional_argument_help() {
    let argv = vec!["/test02"];

    let mut ca = CheckArg::new("test02");

    ca.add_autohelp();
    ca.add(
      'i',
      "input",
      "file to read from",
      ValueType::Required,
      Some(""),
    );
    ca.add('t', "test", "run tests", ValueType::Required, Some(""));

    ca.set_posarg_help("[files...]", "one or more output files");

    let rc = ca.parse(&argv);

    assert_matches!(rc, RC::Ok, "parsing failed");

    assert_eq!(ca.usage(), "Usage: test02 [options] [files...]");
    assert_eq!(
      ca.autohelp(),
      "\
Usage: test02 [options] [files...]

Options:
   -h, --help   show this help message and exit
   -i, --input  file to read from
   -t, --test   run tests

Positional Arguments:
one or more output files\n"
    );
  }

  #[test]
  fn override_usage_line() {
    let argv = vec!["/test02"];

    let mut ca = CheckArg::new("test02");

    ca.add_autohelp();
    ca.add(
      'i',
      "input",
      "file to read from",
      ValueType::Required,
      Some(""),
    );
    ca.add('t', "test", "run tests", ValueType::Required, Some(""));

    ca.set_usage_line("this is a really stupid usage line");

    let rc = ca.parse(&argv);

    assert_matches!(rc, RC::Ok, "parsing failed");

    assert_eq!(ca.usage(), "Usage: this is a really stupid usage line");
    assert_eq!(
      ca.autohelp(),
      "\
Usage: this is a really stupid usage line

Options:
   -h, --help   show this help message and exit
   -i, --input  file to read from
   -t, --test   run tests\n"
    );
  }
}
