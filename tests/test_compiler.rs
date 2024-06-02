use libcore::*;
use libloading::library_filename;
use std::mem::size_of;
use trc::compiler::{ast::ModuleUnit, *};

macro_rules! gen_test_env {
    ($test_code:expr, $env_name:ident) => {
        use trc::compiler::CompileOption;
        use trc::compiler::InputSource;
        let compiler = Compiler::new_string_compiler(
            CompileOption::new(false, InputSource::StringInternal),
            $test_code,
        );
        use std::{cell::RefCell, rc::Rc};
        let env_tmp = Rc::new(RefCell::new(trc::compiler::ModuleManager::new()));
        let token_lexer = Rc::new(RefCell::new(trc::compiler::token::TokenLex::new(
            compiler.compiler_impl.clone(),
        )));
        let mut $env_name = trc::compiler::ast::ModuleUnit::new(
            token_lexer,
            compiler.compiler_impl.clone(),
            env_tmp,
        );
    };
}

/// 前面有int_nums个int时的首地址
fn get_offset(int_nums: usize) -> Opidx {
    Opidx((size_of::<i64>() * int_nums) as OpidxInternal)
}

fn get_func_id(scope: &mut ModuleUnit, name: &str) -> Opidx {
    let idstr = scope
        .token_lexer
        .borrow()
        .get_constpool()
        .get_id(&name.to_string())
        .unwrap();
    let symid = scope
        .get_scope()
        .borrow()
        .get_sym(idstr)
        .expect("sym not found");
    Opidx(
        *scope
            .get_scope()
            .borrow()
            .get_function(symid)
            .unwrap()
            .downcast_rc::<RustFunction>()
            .unwrap()
            .buildin_id as OpidxInternal,
    )
}

const SPECIAL_FUNC_ID: Opidx = ARG_WRONG;

fn opcode_assert_eq(expect: Vec<Inst>, actual: Vec<Inst>) {
    for (counter, (i, j)) in expect.iter().zip(actual.iter()).enumerate() {
        assert_eq!(
            i.opcode,
            j.opcode,
            "\nnum {}, expected:{:?}\nactual:{:?}",
            counter + 1,
            expect,
            actual
        );
        if i.opcode == Opcode::CallNative && j.operand.0 == SPECIAL_FUNC_ID {
            continue;
        }
        assert_eq!(
            i.operand,
            j.operand,
            "\nnum {}, expected:{:?}\nactual:{:?}",
            counter + 1,
            expect,
            actual
        );
    }
}

#[test]
fn test_assign() {
    gen_test_env!(
        r#"a:=10
        a=10
        b:=90
        print("{}{}", a, b)"#,
        t
    );
    t.generate_code().unwrap();
    let fid = get_func_id(&mut t, "print");
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(10)),
            Inst::new_double(Opcode::StoreGlobal, get_offset(0), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(10)),
            Inst::new_double(Opcode::StoreGlobal, get_offset(0), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(90)),
            Inst::new_double(Opcode::StoreGlobal, get_offset(1), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadString, Opidx(0)),
            Inst::new_double(Opcode::LoadGlobalVar, get_offset(0), Opidx(intsz!())),
            Inst::new_single(Opcode::MoveInt, NO_ARG),
            Inst::new_double(Opcode::LoadGlobalVar, get_offset(1), Opidx(intsz!())),
            Inst::new_single(Opcode::MoveInt, NO_ARG),
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(2)),
            Inst::new_single(Opcode::CallNative, fid),
        ],
    )
}

#[test]
fn test_expr_easy1() {
    gen_test_env!(r#"(1)"#, t);
    t.generate_code().unwrap();
    opcode_assert_eq(
        t.staticdata.inst,
        vec![Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(1))],
    );
}

#[test]
fn test_expr_easy2() {
    gen_test_env!(r#"5+~6"#, t);
    t.generate_code().unwrap();
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(5)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(6)),
            Inst::new_single(Opcode::BitNotInt, NO_ARG),
            Inst::new_single(Opcode::AddInt, NO_ARG),
        ],
    );
}

#[test]
fn text_expr_easy3() {
    gen_test_env!(r#"9-8-8"#, t);
    t.generate_code().unwrap();
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(9)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(8)),
            Inst::new_single(Opcode::SubInt, NO_ARG),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(8)),
            Inst::new_single(Opcode::SubInt, NO_ARG),
        ],
    )
}

#[test]
fn test_expr_easy4() {
    gen_test_env!(r#"(8-9)*7"#, t);
    t.generate_code().unwrap();
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(8)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(9)),
            Inst::new_single(Opcode::SubInt, NO_ARG),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(7)),
            Inst::new_single(Opcode::MulInt, NO_ARG),
        ],
    )
}

#[test]
fn test_expr() {
    gen_test_env!(r#"1+9-10*7**6"#, t);
    t.generate_code().unwrap();
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(1)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(9)),
            Inst::new_single(Opcode::AddInt, NO_ARG),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(10)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(7)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(6)),
            Inst::new_single(Opcode::PowerInt, NO_ARG),
            Inst::new_single(Opcode::MulInt, NO_ARG),
            Inst::new_single(Opcode::SubInt, NO_ARG),
        ],
    );
}

#[test]
fn test_expr_final() {
    gen_test_env!(r#"(1+-2)*3//4**(5**6)==1||7==(8&9)"#, t);
    t.generate_code().unwrap();
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(1)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(2)),
            Inst::new_single(Opcode::SelfNegativeInt, NO_ARG),
            Inst::new_single(Opcode::AddInt, NO_ARG),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(3)),
            Inst::new_single(Opcode::MulInt, NO_ARG),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(4)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(5)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(6)),
            Inst::new_single(Opcode::PowerInt, NO_ARG),
            Inst::new_single(Opcode::PowerInt, NO_ARG),
            Inst::new_single(Opcode::ExactDivInt, NO_ARG),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(1)),
            Inst::new_single(Opcode::EqInt, NO_ARG),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(7)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(8)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(9)),
            Inst::new_single(Opcode::BitAndInt, NO_ARG),
            Inst::new_single(Opcode::EqInt, NO_ARG),
            Inst::new_single(Opcode::OrBool, NO_ARG),
        ],
    );
}

#[test]
fn test_expr_in_arg() {
    gen_test_env!(
        r#"
a:=90
print("{}", a+90)"#,
        t
    );
    t.generate_code().unwrap();
    let fid = get_func_id(&mut t, "print");
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(90)),
            Inst::new_double(Opcode::StoreGlobal, get_offset(0), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadString, Opidx(0)),
            Inst::new_double(Opcode::LoadGlobalVar, get_offset(0), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(90)),
            Inst::new_single(Opcode::AddInt, NO_ARG),
            Inst::new_single(Opcode::MoveInt, NO_ARG),
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(1)),
            Inst::new_single(Opcode::CallNative, fid),
        ],
    )
}

#[test]
fn test_call_builtin_function() {
    gen_test_env!(r#"print("hello world!")"#, t);
    t.generate_code().unwrap();
    let fid = get_func_id(&mut t, "print");
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadString, Opidx(0)),
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(0)),
            Inst::new_single(Opcode::CallNative, fid),
        ],
    )
}

#[test]
#[should_panic(expected = "OperatorError")]
fn test_wrong_type() {
    gen_test_env!(r#"1.0+9"#, t);
    t.generate_code().unwrap();
    println!("{:?}", t.staticdata.inst);
}

#[test]
#[should_panic(expected = "OperatorError")]
fn test_wrong_type2() {
    gen_test_env!(r#"1+"90""#, t);
    t.generate_code().unwrap();
    println!("{:?}", t.staticdata.inst);
}

#[test]
#[should_panic(expected = "OperatorError")]
fn test_wrong_type3() {
    gen_test_env!(r#""90"+28"#, t);
    t.generate_code().unwrap();
    println!("{:?}", t.staticdata.inst);
}

#[test]
fn test_if_easy() {
    gen_test_env!(r#"if 1==1 { print("hello world") }"#, t);
    t.generate_code().unwrap();
    let fid = get_func_id(&mut t, "print");
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadInt, Opidx(1)),
            Inst::new_single(Opcode::LoadInt, Opidx(1)),
            Inst::new_single(Opcode::EqInt, NO_ARG),
            Inst::new_single(Opcode::JumpIfFalse, Opidx(7)),
            Inst::new_single(Opcode::LoadString, Opidx(0)),
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(0)),
            Inst::new_single(Opcode::CallNative, fid),
        ],
    )
}

#[test]
fn test_if_easy2() {
    gen_test_env!(
        r#"
        if 1==1 {
            print("hello world")
        } else if 1==2 {

        } else {
            print("hello world")
        }
        "#,
        t
    );
    t.generate_code().unwrap();
}

#[test]
fn test_if() {
    gen_test_env!(
        r#"a:=9 
if a<8{

} else if a>11 {

} else {
    if 8 == 7 {

    } else {

    }
}"#,
        t
    );
    t.generate_code().unwrap();
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(9)),
            Inst::new_double(Opcode::StoreGlobal, get_offset(0), Opidx(intsz!())),
            Inst::new_double(Opcode::LoadGlobalVar, Opidx(0), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(8)),
            Inst::new_single(Opcode::LtInt, Opidx(0)),
            Inst::new_single(Opcode::JumpIfFalse, Opidx(7)),
            Inst::new_single(Opcode::Jump, Opidx(17)),
            Inst::new_double(Opcode::LoadGlobalVar, Opidx(0), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(11)),
            Inst::new_single(Opcode::GtInt, Opidx(0)),
            Inst::new_single(Opcode::JumpIfFalse, Opidx(12)),
            Inst::new_single(Opcode::Jump, Opidx(17)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(8)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(7)),
            Inst::new_single(Opcode::EqInt, Opidx(0)),
            Inst::new_single(Opcode::JumpIfFalse, Opidx(17)),
            Inst::new_single(Opcode::Jump, Opidx(17)),
        ],
    )
}

#[test]
fn test_var_params() {
    gen_test_env!(r#"print("{}{}{}", 1, 2, 3)"#, t);
    t.generate_code().unwrap();
    let fid = get_func_id(&mut t, "print");
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadString, Opidx(0)),
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(1)),
            Inst::new_single(Opcode::MoveInt, NO_ARG),
            Inst::new_single(Opcode::LoadInt, Opidx(2)),
            Inst::new_single(Opcode::MoveInt, NO_ARG),
            Inst::new_single(Opcode::LoadInt, Opidx(3)),
            Inst::new_single(Opcode::MoveInt, NO_ARG),
            Inst::new_single(Opcode::LoadInt, Opidx(3)),
            Inst::new_single(Opcode::CallNative, fid),
        ],
    )
}

#[test]
fn test_while_1() {
    gen_test_env!(r#"while 1==1 { print("hello world") }"#, t);
    t.generate_code().unwrap();
    let fid = get_func_id(&mut t, "print");
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadInt, Opidx(1)),
            Inst::new_single(Opcode::LoadInt, Opidx(1)),
            Inst::new_single(Opcode::EqInt, NO_ARG),
            Inst::new_single(Opcode::JumpIfFalse, Opidx(8)),
            Inst::new_single(Opcode::LoadString, Opidx(0)),
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(0)),
            Inst::new_single(Opcode::CallNative, fid),
            Inst::new_single(Opcode::Jump, Opidx(0)),
        ],
    )
}

#[test]
fn test_for_1() {
    gen_test_env!(r#"for i:=0; i<10; i=i+1 { print("hello world") }"#, t);
    t.generate_code().unwrap();
    let fid = get_func_id(&mut t, "print");
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(0)),
            Inst::new_double(Opcode::StoreGlobal, get_offset(0), Opidx(intsz!())),
            Inst::new_double(Opcode::LoadGlobalVar, get_offset(0), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(10)),
            Inst::new_single(Opcode::LtInt, NO_ARG),
            Inst::new_single(Opcode::JumpIfFalse, Opidx(14)),
            Inst::new_single(Opcode::LoadString, Opidx(0)),
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(0)),
            Inst::new_single(Opcode::CallNative, fid),
            Inst::new_double(Opcode::LoadGlobalVar, get_offset(0), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(1)),
            Inst::new_single(Opcode::AddInt, NO_ARG),
            Inst::new_double(Opcode::StoreGlobal, get_offset(0), Opidx(intsz!())),
            Inst::new_single(Opcode::Jump, Opidx(2)),
        ],
    )
}

#[test]
fn test_func_def_easy1() {
    gen_test_env!(
        r#"
func f1() {
    print("i am function that is called")
}
func f() {
    print("hello world")
    f1()
}"#,
        t
    );
    t.generate_code().unwrap();
    let fid = get_func_id(&mut t, "print");
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::Stop, NO_ARG),
            Inst::new_single(Opcode::LoadString, Opidx(0)),
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(0)),
            Inst::new_single(Opcode::CallNative, fid),
            Inst::new_single(Opcode::PopFrame, NO_ARG),
            Inst::new_single(Opcode::LoadString, Opidx(1)),
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(0)),
            Inst::new_single(Opcode::CallNative, fid),
            Inst::new_single(Opcode::CallCustom, Opidx(0)),
            Inst::new_single(Opcode::PopFrame, NO_ARG),
        ],
    )
}

#[test]
fn test_call_custom_func_easy1() {
    gen_test_env!(
        r#"
func f() {
    print("hello world")
}
f()"#,
        t
    );
    t.generate_code().unwrap();
    let fid = get_func_id(&mut t, "print");
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::CallCustom, Opidx(0)),
            Inst::new_single(Opcode::Stop, NO_ARG),
            Inst::new_single(Opcode::LoadString, Opidx(0)),
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(0)),
            Inst::new_single(Opcode::CallNative, fid),
            Inst::new_single(Opcode::PopFrame, NO_ARG),
        ],
    )
}

#[test]
fn test_func_call_with_args() {
    gen_test_env!(
        r#"
func a1(a: int, b: int) {
    print("{}{}", a, b)
}
a1(0, 1)
a:=1
b:=0
print("{}{}", a, b)
        "#,
        t
    );
    t.generate_code().unwrap();
    let fid = get_func_id(&mut t, "print");
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadInt, Opidx(0)),
            Inst::new_single(Opcode::LoadInt, Opidx(1)),
            Inst::new_single(Opcode::CallCustom, Opidx(0)),
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(1)),
            Inst::new_double(Opcode::StoreGlobal, get_offset(0), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(0)),
            Inst::new_double(Opcode::StoreGlobal, get_offset(1), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadString, Opidx(0)),
            Inst::new_double(Opcode::LoadGlobalVar, get_offset(0), Opidx(intsz!())),
            Inst::new_single(Opcode::MoveInt, NO_ARG),
            Inst::new_double(Opcode::LoadGlobalVar, get_offset(1), Opidx(intsz!())),
            Inst::new_single(Opcode::MoveInt, NO_ARG),
            Inst::new_single(Opcode::LoadInt, Opidx(2)),
            Inst::new_single(Opcode::CallNative, fid),
            Inst::new_single(Opcode::Stop, NO_ARG),
            Inst::new_double(Opcode::StoreLocal, get_offset(0), Opidx(intsz!())),
            Inst::new_double(Opcode::StoreLocal, get_offset(1), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadString, Opidx(0)),
            Inst::new_double(Opcode::LoadLocalVar, get_offset(1), Opidx(intsz!())),
            Inst::new_single(Opcode::MoveInt, NO_ARG),
            Inst::new_double(Opcode::LoadLocalVar, get_offset(0), Opidx(intsz!())),
            Inst::new_single(Opcode::MoveInt, NO_ARG),
            Inst::new_single(Opcode::LoadInt, Opidx(2)),
            Inst::new_single(Opcode::CallNative, fid),
            Inst::new_single(Opcode::PopFrame, NO_ARG),
        ],
    )
}

#[test]
fn test_for_break() {
    gen_test_env!(
        r#"for i:=0;i<=10;i=i+1{
  if i==3{
    continue
  }
  if i==5{
    break
  }
  println("{}", i)
}
a:=0
while a<10{
  a=a+1
  if a==3{
    continue
  }
  println("{}", a)
  if a==5{
    break
  }
}
"#,
        t
    );
    t.generate_code().unwrap();
    let fid = get_func_id(&mut t, "println");
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(0)),
            Inst::new_double(Opcode::StoreGlobal, Opidx(0), Opidx(intsz!())),
            Inst::new_double(Opcode::LoadGlobalVar, Opidx(0), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(10)),
            Inst::new_single(Opcode::LeInt, Opidx(0)),
            Inst::new_single(Opcode::JumpIfFalse, Opidx(26)),
            Inst::new_double(Opcode::LoadGlobalVar, Opidx(0), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(3)),
            Inst::new_single(Opcode::EqInt, Opidx(0)),
            Inst::new_single(Opcode::JumpIfFalse, Opidx(11)),
            Inst::new_single(Opcode::Jump, Opidx(21)),
            Inst::new_double(Opcode::LoadGlobalVar, Opidx(0), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(5)),
            Inst::new_single(Opcode::EqInt, Opidx(0)),
            Inst::new_single(Opcode::JumpIfFalse, Opidx(16)),
            Inst::new_single(Opcode::Jump, Opidx(26)),
            Inst::new_single(Opcode::LoadString, Opidx(0)),
            Inst::new_double(Opcode::LoadGlobalVar, Opidx(0), Opidx(intsz!())),
            Inst::new_single(Opcode::MoveInt, Opidx(0)),
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(1)),
            Inst::new_single(Opcode::CallNative, fid),
            Inst::new_double(Opcode::LoadGlobalVar, Opidx(0), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(1)),
            Inst::new_single(Opcode::AddInt, Opidx(0)),
            Inst::new_double(Opcode::StoreGlobal, Opidx(0), Opidx(intsz!())),
            Inst::new_single(Opcode::Jump, Opidx(2)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(0)),
            Inst::new_double(Opcode::StoreGlobal, Opidx(8), Opidx(intsz!())),
            Inst::new_double(Opcode::LoadGlobalVar, Opidx(8), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(10)),
            Inst::new_single(Opcode::LtInt, Opidx(0)),
            Inst::new_single(Opcode::JumpIfFalse, Opidx(52)),
            Inst::new_double(Opcode::LoadGlobalVar, Opidx(8), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(1)),
            Inst::new_single(Opcode::AddInt, Opidx(0)),
            Inst::new_double(Opcode::StoreGlobal, Opidx(8), Opidx(intsz!())),
            Inst::new_double(Opcode::LoadGlobalVar, Opidx(8), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(3)),
            Inst::new_single(Opcode::EqInt, Opidx(0)),
            Inst::new_single(Opcode::JumpIfFalse, Opidx(41)),
            Inst::new_single(Opcode::Jump, Opidx(28)),
            Inst::new_single(Opcode::LoadString, Opidx(0)),
            Inst::new_double(Opcode::LoadGlobalVar, Opidx(8), Opidx(intsz!())),
            Inst::new_single(Opcode::MoveInt, Opidx(0)),
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(1)),
            Inst::new_single(Opcode::CallNative, fid),
            Inst::new_double(Opcode::LoadGlobalVar, Opidx(8), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(5)),
            Inst::new_single(Opcode::EqInt, Opidx(0)),
            Inst::new_single(Opcode::JumpIfFalse, Opidx(51)),
            Inst::new_single(Opcode::Jump, Opidx(52)),
            Inst::new_single(Opcode::Jump, Opidx(28)),
        ],
    );
}

#[test]
fn test_import_std() {
    gen_test_env!(
        r#"import "std.math"
print("{}", math::sin(9.8))
        "#,
        t
    );
    t.generate_code().unwrap();
    let fid = get_func_id(&mut t, "print");
    assert_eq!(
        t.modules_dll(),
        vec![library_filename("stdlib").to_str().unwrap().to_string()]
    );
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadString, Opidx(1)),
            Inst::new_single(Opcode::LoadFloat, Opidx(0)),
            Inst::new_single(Opcode::CallNative, SPECIAL_FUNC_ID),
            Inst::new_single(Opcode::MoveFloat, Opidx(0)),
            Inst::new_single(Opcode::LoadInt, Opidx(1)),
            Inst::new_single(Opcode::CallNative, fid),
        ],
    );
}

#[test]
fn test_match_int() {
    gen_test_env!(
        r#"
a:=90
match a{
1 -> {
println("{}", a)
}
2|90 -> {
println("{}", a)
}
_ -> {
println("final case")
}
}
println("out of range")
"#,
        t
    );
    t.generate_code().unwrap();
    let fid = get_func_id(&mut t, "println");
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(90)),
            Inst::new_double(Opcode::StoreGlobal, Opidx(0), Opidx(intsz!())),
            Inst::new_double(Opcode::LoadGlobalVar, Opidx(0), Opidx(intsz!())),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(1)),
            Inst::new_single(Opcode::EqIntWithoutPop, Opidx(0)),
            Inst::new_single(Opcode::JumpIfTrue, Opidx(7)),
            Inst::new_single(Opcode::Jump, Opidx(13)),
            Inst::new_single(Opcode::LoadString, Opidx(0)),
            Inst::new_double(Opcode::LoadGlobalVar, Opidx(0), Opidx(intsz!())),
            Inst::new_single(Opcode::MoveInt, Opidx(0)),
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(1)),
            Inst::new_single(Opcode::CallNative, fid),
            Inst::new_single(Opcode::Jump, Opidx(32)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(2)),
            Inst::new_single(Opcode::EqIntWithoutPop, Opidx(0)),
            Inst::new_single(Opcode::JumpIfTrue, Opidx(20)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(90)),
            Inst::new_single(Opcode::EqIntWithoutPop, Opidx(0)),
            Inst::new_single(Opcode::JumpIfTrue, Opidx(20)),
            Inst::new_single(Opcode::Jump, Opidx(26)),
            Inst::new_single(Opcode::LoadString, Opidx(0)),
            Inst::new_double(Opcode::LoadGlobalVar, Opidx(0), Opidx(intsz!())),
            Inst::new_single(Opcode::MoveInt, Opidx(0)),
            Inst::new_single(Opcode::LoadInt, convert_usize_to_oparg(1)),
            Inst::new_single(Opcode::CallNative, fid),
            Inst::new_single(Opcode::Jump, Opidx(32)),
            Inst::new_single(Opcode::Jump, Opidx(28)),
            Inst::new_single(Opcode::Jump, Opidx(32)),
            Inst::new_single(Opcode::LoadString, Opidx(1)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(0)),
            Inst::new_single(Opcode::CallNative, fid),
            Inst::new_single(Opcode::Jump, Opidx(32)),
            Inst::new_single(Opcode::LoadString, Opidx(2)),
            Inst::new_single(Opcode::LoadInt, convert_int_constval_to_oparg(0)),
            Inst::new_single(Opcode::CallNative, fid),
        ],
    )
}

#[test]
fn test_match_string() {
    gen_test_env!(
        r#"
a:="hello"
match a{
"hello"|"world" -> {
    println("{}", a)
}
_ -> {
println("run final!")
}
}
"#,
        t
    );
    t.generate_code().unwrap();
    let fid = get_func_id(&mut t, "println");
    opcode_assert_eq(
        t.staticdata.inst,
        vec![
            Inst::new_single(Opcode::LoadString, Opidx(0)),
            Inst::new_double(Opcode::StoreGlobal, Opidx(0), Opidx(strsz!())),
            Inst::new_double(Opcode::LoadGlobalVar, Opidx(0), Opidx(strsz!())),
            Inst::new_single(Opcode::LoadString, Opidx(0)),
            Inst::new_single(Opcode::EqStrWithoutPop, Opidx(0)),
            Inst::new_single(Opcode::JumpIfTrue, Opidx(10)),
            Inst::new_single(Opcode::LoadString, Opidx(1)),
            Inst::new_single(Opcode::EqStrWithoutPop, Opidx(0)),
            Inst::new_single(Opcode::JumpIfTrue, Opidx(10)),
            Inst::new_single(Opcode::Jump, Opidx(16)),
            Inst::new_single(Opcode::LoadString, Opidx(2)),
            Inst::new_double(Opcode::LoadGlobalVar, Opidx(0), Opidx(strsz!())),
            Inst::new_single(Opcode::MoveStr, Opidx(0)),
            Inst::new_single(Opcode::LoadInt, Opidx(1)),
            Inst::new_single(Opcode::CallNative, fid),
            Inst::new_single(Opcode::Jump, Opidx(22)),
            Inst::new_single(Opcode::Jump, Opidx(18)),
            Inst::new_single(Opcode::Jump, Opidx(22)),
            Inst::new_single(Opcode::LoadString, Opidx(3)),
            Inst::new_single(Opcode::LoadInt, Opidx(0)),
            Inst::new_single(Opcode::CallNative, fid),
            Inst::new_single(Opcode::Jump, Opidx(22)),
        ],
    )
}
