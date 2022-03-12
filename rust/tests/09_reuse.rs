// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#![feature(assert_matches)]

#[cfg(test)]
mod tests {
  use checkarg::{CheckArg, ValueType, RC};
  use std::assert_matches::assert_matches;

  #[test]
  fn general_reuse() {
    let argv1 = vec!["/test08", "-a", "a-val"];
    let argv2 = vec!["/test08", "--beta", "b-val"];
    let argv3 = vec!["/test08", "-c", "c-val"];
    let argv4 = vec!["/test08", "--delta", "d-val"];
    let argv5 = vec!["/test08", "-z", "z-val"];
    let argv6 = vec!["/test08", "--ag", "ag-val"];
    let argv7 = vec!["/test08", "--ng", "ng-val"];

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

    let mut rc = ca.parse(&argv1);
    assert_matches!(rc, RC::Ok, "parsing failed");
    assert!(ca.isset("alpha"));
    assert_matches!(ca.value("alpha"), Some("a-val"));

    rc = ca.parse(&argv2);
    assert_matches!(rc, RC::Ok, "parsing failed");
    assert!(!ca.isset("alpha"));
    assert_matches!(ca.value("alpha"), None);
    assert!(ca.isset("beta"));
    assert_matches!(ca.value("beta"), Some("b-val"));

    rc = ca.parse(&argv3);
    assert_matches!(rc, RC::Ok, "parsing failed");
    assert!(!ca.isset("alpha"));
    assert_matches!(ca.value("alpha"), None);
    assert!(!ca.isset("beta"));
    assert_matches!(ca.value("beta"), None);
    assert!(ca.isset("gamma"));
    assert_matches!(ca.value("gamma"), Some("c-val"));

    rc = ca.parse(&argv4);
    assert_matches!(rc, RC::Ok, "parsing failed");
    assert!(!ca.isset("alpha"));
    assert_matches!(ca.value("alpha"), None);
    assert!(!ca.isset("beta"));
    assert_matches!(ca.value("beta"), None);
    assert!(!ca.isset("gamma"));
    assert_matches!(ca.value("gamma"), None);
    assert!(ca.isset("delta"));
    assert_matches!(ca.value("delta"), Some("d-val"));

    rc = ca.parse(&argv5);
    assert_matches!(rc, RC::Ok, "parsing failed");
    assert!(!ca.isset("alpha"));
    assert_matches!(ca.value("alpha"), None);
    assert!(!ca.isset("beta"));
    assert_matches!(ca.value("beta"), None);
    assert!(!ca.isset("gamma"));
    assert_matches!(ca.value("gamma"), None);
    assert!(!ca.isset("delta"));
    assert_matches!(ca.value("delta"), None);
    assert!(ca.isset("ag"));
    assert_matches!(ca.value("ag"), Some("z-val"));

    rc = ca.parse(&argv6);
    assert_matches!(rc, RC::Ok, "parsing failed");
    assert!(!ca.isset("alpha"));
    assert_matches!(ca.value("alpha"), None);
    assert!(!ca.isset("beta"));
    assert_matches!(ca.value("beta"), None);
    assert!(!ca.isset("gamma"));
    assert_matches!(ca.value("gamma"), None);
    assert!(!ca.isset("delta"));
    assert_matches!(ca.value("delta"), None);
    assert!(ca.isset("ag"));
    assert_matches!(ca.value("ag"), Some("ag-val"));

    rc = ca.parse(&argv7);
    assert_matches!(rc, RC::Ok, "parsing failed");
    assert!(!ca.isset("alpha"));
    assert_matches!(ca.value("alpha"), None);
    assert!(!ca.isset("beta"));
    assert_matches!(ca.value("beta"), None);
    assert!(!ca.isset("gamma"));
    assert_matches!(ca.value("gamma"), None);
    assert!(!ca.isset("delta"));
    assert_matches!(ca.value("delta"), None);
    assert!(!ca.isset("ag"));
    assert_matches!(ca.value("ag"), None);
    assert!(ca.isset("ng"));
    assert_matches!(ca.value("ng"), Some("ng-val"));
  }
}
