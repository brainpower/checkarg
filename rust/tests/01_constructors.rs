// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#![feature(assert_matches)]

#[cfg(test)]
mod tests {
  use checkarg::{CheckArg, RC};
  use std::assert_matches::assert_matches;

  #[test]
  fn constructor_simple() {
    let args = vec!["/usr/bin/name"];

    let mut ca = CheckArg::new("name");
    let rc = ca.parse(&args);

    assert_matches!(rc, RC::Ok, "parsing failed");
    assert_eq!(ca.callname(), "/usr/bin/name", "callname() is wrong");
    assert_eq!(
      ca.usage(),
      "Usage: name [options]",
      "appname in usage() is wrong"
    );
  }

  /*
  #[test]
  fn constructor_appname_autodetect() {
    use std::path::Path;
    use std::ffi::OsStr;

    let args = vec!("/usr/bin/name", );

    let mut ca = CheckArg::new(
      Path::new(args[0])
        .file_name()
        .unwrap_or(OsStr::new("invalid"))
        .to_str().unwrap_or("invalid2")
    );
    let rc = ca.parse(&args);

    assert!(matches!(rc, RC::Ok), "parsing failed");
    assert_eq!(ca.callname(), "/usr/bin/name", "callname() is wrong");
    assert_eq!(ca.usage(), "Usage: name [options]", "appname in usage() is wrong");
  }
  */
}
