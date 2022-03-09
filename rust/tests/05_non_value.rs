// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#![feature(assert_matches)]
#![feature(get_mut_unchecked)]

#[cfg(test)]
mod tests {
  use lazy_static::lazy_static;
  use std::assert_matches::assert_matches;
  use std::rc::Rc;
  use std::sync::Arc;
  // use itertools::Itertools;

  use checkarg::{CheckArg, ValueType, RC};

  lazy_static! {
    static ref GLOBAL_CB_OPT: Arc<String> = Arc::new(String::from(""));
    static ref GLOBAL_CB_OPT2: std::sync::Weak<String> = Arc::downgrade(&GLOBAL_CB_OPT);
  }

  fn callback_b(_cx: &CheckArg, opt: &str, _val: &str) -> Result<(), ()> {
    let copt = &mut GLOBAL_CB_OPT2.upgrade().expect("BUG");
    unsafe {
      // I could not find any other way to solve this
      let copt2 = Arc::get_mut_unchecked(copt);
      copt2.clear();
      copt2.push_str(opt);
    }
    Ok(())
  }

  fn callback_d(_cx: &CheckArg, opt: &str, _val: &str) -> Result<(), ()> {
    let copt = &mut GLOBAL_CB_OPT2.upgrade().expect("BUG");
    unsafe {
      // I could not find any other way to solve this
      let copt2 = Arc::get_mut_unchecked(copt);
      copt2.clear();
      copt2.push_str(opt);
    }
    Ok(())
  }

  #[test]
  fn long_nonvalue_options() {
    let options = vec!["alpha", "beta", "gamma", "delta"];

    let mut ca = CheckArg::new("test05");
    ca.add('a', "alpha", "non-value opt a", ValueType::None, None);
    ca.add_cb(
      'b',
      "beta",
      "non-value opt b",
      callback_b,
      ValueType::None,
      None,
    );
    ca.add_long("gamma", "non-value long opt gamma", ValueType::None, None);
    ca.add_long_cb(
      "delta",
      "non-value long opt delta",
      callback_d,
      ValueType::None,
      None,
    );
    ca.add(
      'e',
      "epsilon",
      "non-value long opt epsilon",
      ValueType::None,
      None,
    );

    for opt in options {
      unsafe {
        // I could not find any other way to solve this
        let copt = &mut GLOBAL_CB_OPT2.upgrade().expect("BUG");
        Arc::get_mut_unchecked(copt).clear();
      }

      let xopt = format!("--{}", opt);
      let argv = vec!["/test05", &xopt];

      let ret = ca.parse(&argv);

      assert_matches!(ret, RC::Ok, "parsing failed");

      // the option was seen and parsed
      assert!(ca.isset(&opt));

      // this option never gets passed, so it should not be set
      assert!(!ca.isset("epsilon"));

      // FIXME: somtimes races, find better way to do this global thing
      /*if opt == "beta" || opt == "delta" {
        assert_eq!(**GLOBAL_CB_OPT, opt);
      } else {
        assert_eq!(**GLOBAL_CB_OPT, "");
      }*/
    }
  }

  #[test]
  fn long_nonvalue_options_with_lambdas() {
    let options = vec!["alpha", "beta", "gamma", "delta"];
    let cb_opt = Rc::new(String::from(""));
    let cb_opt2 = Rc::downgrade(&cb_opt);

    let mut ca = CheckArg::new("test05");
    ca.add('a', "alpha", "non-value opt a", ValueType::None, None);
    ca.add_cb(
      'b',
      "beta",
      "non-value opt b",
      |_cx, opt, _val| {
        let copt = &mut cb_opt2.upgrade().expect("BUG");
        unsafe {
          // I could not find any other way to solve this
          let copt2 = Rc::get_mut_unchecked(copt);
          copt2.clear();
          copt2.push_str(opt);
        }
        Ok(())
      },
      ValueType::None,
      None,
    );
    ca.add_long("gamma", "non-value long opt gamma", ValueType::None, None);
    ca.add_long_cb(
      "delta",
      "non-value long opt delta",
      |_cx, opt, _val| {
        let copt = &mut cb_opt2.upgrade().expect("BUG");
        unsafe {
          // I could not find any other way to solve this
          let copt2 = Rc::get_mut_unchecked(copt);
          copt2.clear();
          copt2.push_str(opt);
        }
        Ok(())
      },
      ValueType::None,
      None,
    );
    ca.add(
      'e',
      "epsilon",
      "non-value long opt epsilon",
      ValueType::None,
      None,
    );

    for opt in options {
      unsafe {
        // I could not find any other way to solve this
        let copt = &mut cb_opt2.upgrade().expect("BUG");
        Rc::get_mut_unchecked(copt).clear();
      }

      let xopt = format!("--{}", opt);
      let argv = vec!["/test05", &xopt];

      let ret = ca.parse(&argv);

      assert_matches!(ret, RC::Ok, "parsing failed");

      // the option was seen and parsed
      assert!(ca.isset(&opt));

      // this option never gets passed, so it should not be set
      assert!(!ca.isset("epsilon"));

      if opt == "beta" || opt == "delta" {
        assert_eq!(*cb_opt, opt);
      }
      else {
        assert_eq!(*cb_opt, "");
      }
    }
  }

  #[test]
  fn short_nonvalue_options() {
    let options = vec!["alpha", "beta"];

    let mut ca = CheckArg::new("test05");
    ca.add('a', "alpha", "non-value opt a", ValueType::None, None);
    ca.add_cb(
      'b',
      "beta",
      "non-value opt b",
      callback_b,
      ValueType::None,
      None,
    );
    ca.add_long("gamma", "non-value long opt gamma", ValueType::None, None);
    ca.add_long_cb(
      "delta",
      "non-value long opt delta",
      callback_d,
      ValueType::None,
      None,
    );
    ca.add(
      'e',
      "epsilon",
      "non-value long opt epsilon",
      ValueType::None,
      None,
    );

    for opt in options {
      unsafe {
        // I could not find any other way to solve this
        let copt = &mut GLOBAL_CB_OPT2.upgrade().expect("BUG");
        Arc::get_mut_unchecked(copt).clear();
      }

      let xopt = format!("-{}", opt.chars().nth(0).unwrap());
      let argv = vec!["/test05", &xopt];

      let ret = ca.parse(&argv);

      assert_matches!(ret, RC::Ok, "parsing failed");

      // the option was seen and parsed
      assert!(ca.isset(&opt));

      // this option never gets passed, so it should not be set
      assert!(!ca.isset("gamma"));
      assert!(!ca.isset("delta"));
      assert!(!ca.isset("epsilon"));

      // FIXME: somtimes races, find better way to do this global thing
      /*if opt == "beta" {
        assert_eq!(**GLOBAL_CB_OPT, opt);
      } else {
        assert_eq!(**GLOBAL_CB_OPT, "");
      }*/
    }
  }

  #[test]
  fn short_nonvalue_options_with_lambdas() {
    let options = vec!["alpha", "beta"];
    let cb_opt = Rc::new(String::from(""));
    let cb_opt2 = Rc::downgrade(&cb_opt);

    let mut ca = CheckArg::new("test05");
    ca.add('a', "alpha", "non-value opt a", ValueType::None, None);
    ca.add_cb(
      'b',
      "beta",
      "non-value opt b",
      |_cx, opt, _val| {
        let copt = &mut cb_opt2.upgrade().expect("BUG");
        unsafe {
          // I could not find any other way to solve this
          let copt2 = Rc::get_mut_unchecked(copt);
          copt2.clear();
          copt2.push_str(opt);
        }
        Ok(())
      },
      ValueType::None,
      None,
    );
    ca.add_long("gamma", "non-value long opt gamma", ValueType::None, None);
    ca.add_long_cb(
      "delta",
      "non-value long opt delta",
      |_cx, opt, _val| {
        let copt = &mut cb_opt2.upgrade().expect("BUG");
        unsafe {
          // I could not find any other way to solve this
          let copt2 = Rc::get_mut_unchecked(copt);
          copt2.clear();
          copt2.push_str(opt);
        }
        Ok(())
      },
      ValueType::None,
      None,
    );
    ca.add(
      'e',
      "epsilon",
      "non-value long opt epsilon",
      ValueType::None,
      None,
    );

    for opt in options {
      unsafe {
        // I could not find any other way to solve this
        let copt = &mut cb_opt2.upgrade().expect("BUG");
        Rc::get_mut_unchecked(copt).clear();
      }

      let xopt = format!("-{}", opt.chars().nth(0).unwrap());
      let argv = vec!["/test05", &xopt];

      let ret = ca.parse(&argv);

      assert_matches!(ret, RC::Ok, "parsing failed");

      // the option was seen and parsed
      assert!(ca.isset(&opt));

      // this option never gets passed, so it should not be set
      assert!(!ca.isset("gamma"));
      assert!(!ca.isset("delta"));
      assert!(!ca.isset("epsilon"));

      if opt == "beta" {
        assert_eq!(*cb_opt, opt);
      }
      else {
        assert_eq!(*cb_opt, "");
      }
    }
  }

  #[test]
  fn value_given_to_nonvalue_options() {
    // this only works with long options of the form: --option=value
    // otherwise values would be interpreted as positional arguments
    // or invalid short options
    let args = vec!["alpha", "beta", "gamma", "delta"];

    let mut ca = CheckArg::new("test05");
    ca.add('a', "alpha", "non-value opt a", ValueType::None, None);
    ca.add_cb(
      'b',
      "beta",
      "non-value opt b",
      |_cx, _o, _v| Ok(()),
      ValueType::None,
      None,
    );
    ca.add_long("gamma", "non-value long opt gamma", ValueType::None, None);
    ca.add_long_cb(
      "delta",
      "non-value long opt delta",
      |_cx, _o, _v| Ok(()),
      ValueType::None,
      None,
    );
    ca.add_long("epsilon", "non-value long opt", ValueType::None, None);

    for arg in args {
      let opt = format!("--{}=value", arg);
      let argv = vec!["/test05", &opt];

      let ret = ca.parse(&argv);

      assert_matches!(ret, RC::InvVal);
    }
  }
}
