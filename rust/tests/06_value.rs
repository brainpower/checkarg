// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#![feature(assert_matches)]
#![feature(get_mut_unchecked)]

#[cfg(test)]
mod tests {
  use std::assert_matches::assert_matches;
  use std::rc::Rc;
  // use itertools::Itertools;

  use checkarg::{CheckArg, ValueType, RC};

  #[test]
  fn short_value() {
    let options = vec!["alpha", "beta"];
    let cb_option_rc = Rc::new(String::from(""));
    let cb_value_rc = Rc::new(String::from(""));
    let cb_option = Rc::downgrade(&cb_option_rc);
    let cb_value = Rc::downgrade(&cb_value_rc);

    let mut ca = CheckArg::new("test06");
    ca.add('a', "alpha", "value opt a", ValueType::Required, None);
    ca.add_cb(
      'b',
      "beta",
      "value opt b",
      |_, o, v| {
        let copt = &mut cb_option.upgrade().expect("BUG");
        let cval = &mut cb_value.upgrade().expect("BUG");
        unsafe {
          // I could not find any other way to solve this
          let copt = Rc::get_mut_unchecked(copt);
          let cval = Rc::get_mut_unchecked(cval);
          copt.clear();
          copt.push_str(o);
          cval.clear();
          cval.push_str(v);
        }
        Ok(())
      },
      ValueType::Required,
      None,
    );
    ca.add_long("gamma", "value long opt gamma", ValueType::Required, None);
    ca.add_long_cb(
      "delta",
      "value long opt delta",
      |_, o, v| {
        let copt = &mut cb_option.upgrade().expect("BUG");
        let cval = &mut cb_value.upgrade().expect("BUG");
        unsafe {
          // I could not find any other way to solve this
          let copt = Rc::get_mut_unchecked(copt);
          let cval = Rc::get_mut_unchecked(cval);
          copt.clear();
          copt.push_str(o);
          cval.clear();
          cval.push_str(v);
        }
        Ok(())
      },
      ValueType::Required,
      None,
    );
    ca.add('e', "epsilon", "value opt e", ValueType::Required, None);

    for opt in options {
      let arg1 = format!("-{}", opt.chars().nth(0).unwrap());
      let arg2 = format!("val-{}", opt);
      let argv = vec!["/test06_value", &arg1, &arg2];

      let ret = ca.parse(&argv);

      assert_matches!(ret, RC::Ok);

      assert!(!ca.isset("gamma"));
      assert!(!ca.isset("delta"));
      assert!(!ca.isset("epsilon"));

      assert_eq!(ca.value(opt).unwrap(), arg2);

      if opt == "beta" {
        assert_eq!(*cb_option_rc, "beta");
        assert_eq!(*cb_value_rc, arg2);
      }
    }
  }

  #[test]
  fn short_value_combined() {
    let options = vec!["alpha", "beta"];
    let cb_option_rc = Rc::new(String::from(""));
    let cb_value_rc = Rc::new(String::from(""));
    let cb_option = Rc::downgrade(&cb_option_rc);
    let cb_value = Rc::downgrade(&cb_value_rc);

    let mut ca = CheckArg::new("test06");
    ca.add('a', "alpha", "value opt a", ValueType::Required, None);
    ca.add_cb(
      'b',
      "beta",
      "value opt b",
      |_, o, v| {
        let copt = &mut cb_option.upgrade().expect("BUG");
        let cval = &mut cb_value.upgrade().expect("BUG");
        unsafe {
          // I could not find any other way to solve this
          let copt = Rc::get_mut_unchecked(copt);
          let cval = Rc::get_mut_unchecked(cval);
          copt.clear();
          copt.push_str(o);
          cval.clear();
          cval.push_str(v);
        }
        Ok(())
      },
      ValueType::Required,
      None,
    );
    ca.add_long("gamma", "value long opt gamma", ValueType::Required, None);
    ca.add_long_cb(
      "delta",
      "value long opt delta",
      |_, o, v| {
        let copt = &mut cb_option.upgrade().expect("BUG");
        let cval = &mut cb_value.upgrade().expect("BUG");
        unsafe {
          // I could not find any other way to solve this
          let copt = Rc::get_mut_unchecked(copt);
          let cval = Rc::get_mut_unchecked(cval);
          copt.clear();
          copt.push_str(o);
          cval.clear();
          cval.push_str(v);
        }
        Ok(())
      },
      ValueType::Required,
      None,
    );
    ca.add('e', "epsilon", "value opt e", ValueType::Required, None);

    for opt in options {
      let arg2 = format!("val-{}", opt);
      let arg = format!("-{}{}", opt.chars().nth(0).unwrap(), arg2);
      let argv = vec!["/test06_value", &arg];

      let ret = ca.parse(&argv);

      assert_matches!(ret, RC::Ok);

      assert!(!ca.isset("gamma"));
      assert!(!ca.isset("delta"));
      assert!(!ca.isset("epsilon"));

      assert_eq!(ca.value(opt).unwrap(), arg2);

      if opt == "beta" {
        assert_eq!(*cb_option_rc, "beta");
        assert_eq!(*cb_value_rc, arg2);
      }
    }
  }

  #[test]
  fn long_value() {
    let options = vec!["alpha", "beta", "gamma", "delta"];
    let cb_option_rc = Rc::new(String::from(""));
    let cb_value_rc = Rc::new(String::from(""));
    let cb_option = Rc::downgrade(&cb_option_rc);
    let cb_value = Rc::downgrade(&cb_value_rc);

    let mut ca = CheckArg::new("test06");
    ca.add('a', "alpha", "value opt a", ValueType::Required, None);
    ca.add_cb(
      'b',
      "beta",
      "value opt b",
      |_, o, v| {
        let copt = &mut cb_option.upgrade().expect("BUG");
        let cval = &mut cb_value.upgrade().expect("BUG");
        unsafe {
          // I could not find any other way to solve this
          let copt = Rc::get_mut_unchecked(copt);
          let cval = Rc::get_mut_unchecked(cval);
          copt.clear();
          copt.push_str(o);
          cval.clear();
          cval.push_str(v);
        }
        Ok(())
      },
      ValueType::Required,
      None,
    );
    ca.add_long("gamma", "value long opt gamma", ValueType::Required, None);
    ca.add_long_cb(
      "delta",
      "value long opt delta",
      |_, o, v| {
        let copt = &mut cb_option.upgrade().expect("BUG");
        let cval = &mut cb_value.upgrade().expect("BUG");
        unsafe {
          // I could not find any other way to solve this
          let copt = Rc::get_mut_unchecked(copt);
          let cval = Rc::get_mut_unchecked(cval);
          copt.clear();
          copt.push_str(o);
          cval.clear();
          cval.push_str(v);
        }
        Ok(())
      },
      ValueType::Required,
      None,
    );
    ca.add('e', "epsilon", "value opt e", ValueType::Required, None);

    for opt in options {
      let arg1 = format!("--{}", opt);
      let arg2 = format!("val-{}", opt);
      let argv = vec!["/test06_value", &arg1, &arg2];

      let ret = ca.parse(&argv);

      assert_matches!(ret, RC::Ok);

      assert!(!ca.isset("epsilon"));

      assert_eq!(ca.value(opt).unwrap(), arg2);

      if opt == "beta" || opt == "delta" {
        assert_eq!(*cb_option_rc, opt);
        assert_eq!(*cb_value_rc, arg2);
      }
    }
  }

  #[test]
  fn long_value_combined() {
    let options = vec!["alpha", "beta", "gamma", "delta"];
    let cb_option_rc = Rc::new(String::from(""));
    let cb_value_rc = Rc::new(String::from(""));
    let cb_option = Rc::downgrade(&cb_option_rc);
    let cb_value = Rc::downgrade(&cb_value_rc);

    let mut ca = CheckArg::new("test06");
    ca.add('a', "alpha", "value opt a", ValueType::Required, None);
    ca.add_cb(
      'b',
      "beta",
      "value opt b",
      |_, o, v| {
        let copt = &mut cb_option.upgrade().expect("BUG");
        let cval = &mut cb_value.upgrade().expect("BUG");
        unsafe {
          // I could not find any other way to solve this
          let copt = Rc::get_mut_unchecked(copt);
          let cval = Rc::get_mut_unchecked(cval);
          copt.clear();
          copt.push_str(o);
          cval.clear();
          cval.push_str(v);
        }
        Ok(())
      },
      ValueType::Required,
      None,
    );
    ca.add_long("gamma", "value long opt gamma", ValueType::Required, None);
    ca.add_long_cb(
      "delta",
      "value long opt delta",
      |_, o, v| {
        let copt = &mut cb_option.upgrade().expect("BUG");
        let cval = &mut cb_value.upgrade().expect("BUG");
        unsafe {
          // I could not find any other way to solve this
          let copt = Rc::get_mut_unchecked(copt);
          let cval = Rc::get_mut_unchecked(cval);
          copt.clear();
          copt.push_str(o);
          cval.clear();
          cval.push_str(v);
        }
        Ok(())
      },
      ValueType::Required,
      None,
    );
    ca.add('e', "epsilon", "value opt e", ValueType::Required, None);

    for opt in options {
      let arg2 = format!("val-{}", opt);
      let arg = format!("--{}={}", opt, arg2);
      let argv = vec!["/test06_value", &arg];

      let ret = ca.parse(&argv);

      assert_matches!(ret, RC::Ok);

      assert!(!ca.isset("epsilon"));

      assert_eq!(ca.value(opt).unwrap(), arg2);

      if opt == "beta" || opt == "delta" {
        assert_eq!(*cb_option_rc, opt);
        assert_eq!(*cb_value_rc, arg2);
      }
    }
  }

  #[test]
  fn missing_value() {
    let options = vec!["-a", "--alpha", "-b", "--beta", "--gamma", "--delta"];

    let mut ca = CheckArg::new("test06");
    ca.add('a', "alpha", "non-value opt a", ValueType::Required, None);
    ca.add_cb(
      'b',
      "beta",
      "non-value opt b",
      |_, _, _| Ok(()),
      ValueType::Required,
      None,
    );
    ca.add_long(
      "gamma",
      "non-value long opt gamma",
      ValueType::Required,
      None,
    );
    ca.add_long_cb(
      "delta",
      "non-value long opt delta",
      |_, _, _| Ok(()),
      ValueType::Required,
      None,
    );
    ca.add('e', "epsilon", "non-value opt e", ValueType::Required, None);

    for opt in options {
      let argv = vec!["/test06", opt];

      // println!("argv: {:?}", argv);

      let ret = ca.parse(&argv);

      assert_matches!(ret, RC::MissVal);
    }
  }

  #[test]
  fn empty_value() {
    let options = vec!["-a", "--alpha", "-b", "--beta", "--gamma", "--delta"];

    let mut ca = CheckArg::new("test06");
    ca.add('a', "alpha", "non-value opt a", ValueType::Required, None);
    ca.add_cb(
      'b',
      "beta",
      "non-value opt b",
      |_, _, _| Ok(()),
      ValueType::Required,
      None,
    );
    ca.add_long(
      "gamma",
      "non-value long opt gamma",
      ValueType::Required,
      None,
    );
    ca.add_long_cb(
      "delta",
      "non-value long opt delta",
      |_, _, _| Ok(()),
      ValueType::Required,
      None,
    );
    ca.add('e', "epsilon", "non-value opt e", ValueType::Required, None);

    for opt in options {
      let argv = vec!["/test06", opt, ""];

      let ret = ca.parse(&argv);

      // an empty string as value should not trigger a RC::MissVal
      assert_matches!(ret, RC::Ok);
    }
  }

  #[test]
  fn empty_value_combined() {
    // this does not work with short options, there is no '=' for those
    let options = vec!["--alpha=", "--beta=", "--gamma=", "--delta="];

    let mut ca = CheckArg::new("test06");
    ca.add('a', "alpha", "non-value opt a", ValueType::Required, None);
    ca.add_cb(
      'b',
      "beta",
      "non-value opt b",
      |_, _, _| Ok(()),
      ValueType::Required,
      None,
    );
    ca.add_long(
      "gamma",
      "non-value long opt gamma",
      ValueType::Required,
      None,
    );
    ca.add_long_cb(
      "delta",
      "non-value long opt delta",
      |_, _, _| Ok(()),
      ValueType::Required,
      None,
    );
    ca.add('e', "epsilon", "non-value opt e", ValueType::Required, None);

    for opt in options {
      let argv = vec!["/test06", opt];

      let ret = ca.parse(&argv);

      // an empty string as value should not trigger a RC::MissVal
      assert_matches!(ret, RC::Ok);
    }
  }

  #[test]
  fn special_values() {
    let values = vec!["=test", "-test", "--test", "-", "--"];

    let mut ca = CheckArg::new("test06");
    ca.add('i', "input", "input file", ValueType::Required, None);

    for val in values {
      let arg3 = format!("--input={}", val);
      let arg4 = format!("-i{}", val);
      let options = vec![
        vec!["/test06", "--input", val],
        vec!["/test06", "-i", val],
        vec!["/test06", &arg3],
        vec!["/test06", &arg4],
      ];

      for argv in options {
        let ret = ca.parse(&argv);

        assert_matches!(ret, RC::Ok);
        assert_eq!(ca.value("input").unwrap(), val);
      }
    }
  }
}
