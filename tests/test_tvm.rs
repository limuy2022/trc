use libcore::*;
use std::ptr::null_mut;
use trc::compiler::*;
use trc::tvm::*;

#[test]
fn test_dyna_data() {
    // panic!("deojfoejfoe");
    let mut data = DynaData::new();
    data.alloc_var_space(10);
    assert_eq!(data.get_var_used(), 10);
    data.dealloc_var_space(10);
    assert_eq!(data.get_var_used(), 0);
    data.push_data(10i64);
    data.push_data(20.0f64);
    data.push_data(30i64);
    data.push_data(null_mut::<i32>());
    data.push_data(50);
    data.push_data(false);
    // assert_eq == false
    assert!(!data.pop_data::<bool>());
    assert_eq!(data.pop_data::<i32>(), 50);
    assert_eq!(data.pop_data::<*mut i32>(), null_mut::<i32>());
    assert_eq!(data.pop_data::<i64>(), 30);
    assert_eq!(data.pop_data::<f64>(), 20.0);
    assert_eq!(data.pop_data::<i64>(), 10);
}

macro_rules! gen_test_env {
    ($code:expr, $var:ident) => {
        let mut compiler = Compiler::new_string_compiler(
            CompileOption::new(false, InputSource::StringInternal),
            $code,
        );
        let com_tmp = compiler.lex().unwrap();
        // println!("{:?}", com_tmp.inst);
        // let tmp = com_tmp.return_static_data();
        let tmp = todo!();
        let mut $var = Vm::new(&tmp);
    };
}

#[test]
fn test_stdfunc() {
    gen_test_env!(r#"print("{},{},{},{}", 1, "h", 'p', true)"#, vm);
    vm.run().unwrap()
}

#[test]
fn test_var_define() {
    gen_test_env!(
        r#"a:=10
        a=10
        print("{}", a)"#,
        vm
    );
    vm.run().unwrap()
}

#[test]
fn test_if_easy() {
    gen_test_env!(r#"if 1==1 { println("ok") }"#, vm);
    vm.run().unwrap()
}

#[test]
fn test_if_easy2() {
    gen_test_env!(r#"if 1==1 { println("ok") } else { println("error") }"#, vm);
    vm.run().unwrap()
}

#[test]
fn test_if_final() {
    gen_test_env!(
        r#"a:=90
if a==90 {
  println("i equal to 90")
}
if a < 89 {
  println("i less than 90")
} else {
  if a % 2 == 0 {
    println("i is even")
  } else {
    println("i is odd")
  }
}"#,
        vm
    );
    vm.run().unwrap()
}

#[test]
fn test_while() {
    gen_test_env!(
        r#"a:=10
while a > 0 {
    print("{}", a)
    a = a - 1
}"#,
        vm
    );
    vm.run().unwrap()
}

#[test]
fn test_for() {
    gen_test_env!(
        r#"for i := 0; i <= 10; i=i+1 {
    print("{}", i)
}"#,
        vm
    );
    vm.run().unwrap()
}

#[test]
fn test_call_custom_function() {
    gen_test_env!(
        r#"
func t() {
    print("90")
}
t()
t()
        "#,
        vm
    );
    vm.run().unwrap()
}

#[test]
fn test_call_custom_function2() {
    gen_test_env!(
        r#"func tt() {
  println("I am function2")
}
func t() {
  println("I am function1")
  tt()
}
t()"#,
        vm
    );
    vm.run().unwrap()
}
