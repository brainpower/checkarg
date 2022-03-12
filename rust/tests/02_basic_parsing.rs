// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#![feature(assert_matches)]

#[cfg(test)]
mod tests {
  use checkarg::{CheckArg, ValueType, RC};
  use std::assert_matches::assert_matches;

  fn triggering_help(option: &str) {
    let argv = vec!["/test01", option];

    let mut cb_ran = 0;
    let mut out = String::new();
    {
      let mut ca = CheckArg::new("test01");
      // the default callback calls exit(0), which is bad here
      // ca.add_autohelp();
      ca.add_cb(
        'h',
        "help",
        "show this help message and exit",
        |cx, _opt, _val| {
          cb_ran += 1;
          out = cx.autohelp();
          Ok(())
        },
        ValueType::None,
        None,
      );
      ca.add(
        'i',
        "input",
        "file to read from",
        ValueType::Required,
        Some(""),
      );

      let rc = ca.parse(&argv);

      assert_matches!(rc, RC::Ok, "parsing failed");
    }
    assert_eq!(cb_ran, 1, "callback did not run");

    assert_eq!(
      out,
      "\
Usage: test01 [options]

Options:
   -h, --help   show this help message and exit
   -i, --input  file to read from\n"
    );
  }

  #[test]
  fn triggering_help_long_option() { triggering_help("--help") }

  #[test]
  fn triggering_help_short_option() { triggering_help("-h") }

  fn setup_simple_ca() -> CheckArg<'static> {
    let mut ca = CheckArg::new("name");
    ca.add(
      'i',
      "input",
      "file to read from",
      ValueType::Required,
      Some(""),
    );
    ca
  }

  #[test]
  fn correct_option_and_value() {
    let mut ca = setup_simple_ca();

    let argv = vec!["/usr/bin/name", "-i", "input.in"];

    let rc = ca.parse(&argv);

    assert_matches!(rc, RC::Ok, "parsing failed");
    assert!(ca.isset("input"));
    assert_matches!(ca.value("input"), Some(input) if input == "input.in");
    assert_eq!(ca.value("input").unwrap(), "input.in");
  }

  #[test]
  fn invalid_option() {
    let mut ca = setup_simple_ca();

    let argv = vec!["/usr/bin/name", "-x"];

    let rc = ca.parse(&argv);

    assert_matches!(rc, RC::InvOpt, "parse should detect invalid option");
  }

  #[test]
  fn missing_value() {
    let mut ca = setup_simple_ca();

    let argv = vec!["/usr/bin/name", "-i"];

    let rc = ca.parse(&argv);

    assert_matches!(rc, RC::MissVal, "parse should detect missing value");
  }

  #[test]
  #[should_panic(expected = "argv must have at least one element")]
  fn empty_argv_vector() {
    let mut ca = setup_simple_ca();

    let argv = vec![];

    ca.parse(&argv);
  }
}
