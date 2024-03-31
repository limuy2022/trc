use std::mem::size_of;
use trc::base::stdlib::*;
use trc::{base::codegen::*, compiler::*};

macro_rules! gen_test_env {
    ($test_code:expr, $env_name:ident) => {
        use trc::compiler::CompileOption;
        use trc::compiler::InputSource;
        let mut compiler = Compiler::new_string_compiler(
            CompileOption::new(false, InputSource::StringInternal),
            $test_code,
        );
        let token_lexer = trc::compiler::token::TokenLex::new(&mut compiler);
        let mut $env_name = trc::compiler::ast::AstBuilder::new(token_lexer);
    };
}

/// 前面有int_nums个int时的首地址
fn get_offset(int_nums: usize) -> usize {
    size_of::<i64>() * int_nums
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
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(Opcode::StoreGlobalInt, get_offset(0)),
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(Opcode::StoreGlobalInt, get_offset(0)),
            Inst::new(Opcode::LoadInt, 3),
            Inst::new(Opcode::StoreGlobalInt, get_offset(1)),
            Inst::new(Opcode::LoadString, 0),
            Inst::new(Opcode::LoadGlobalVarInt, get_offset(0)),
            Inst::new(Opcode::MoveInt, NO_ARG),
            Inst::new(Opcode::LoadGlobalVarInt, get_offset(1)),
            Inst::new(Opcode::MoveInt, NO_ARG),
            Inst::new(Opcode::LoadInt, 4),
            Inst::new(
                Opcode::CallNative,
                get_prelude_function("print").unwrap().buildin_id
            ),
        ],
    )
}

#[test]
fn test_expr_easy1() {
    gen_test_env!(r#"(1)"#, t);
    t.generate_code().unwrap();
    assert_eq!(
        t.staticdata.inst,
        vec![Inst::new(Opcode::LoadInt, INT_VAL_POOL_ONE)]
    );
}

#[test]
fn test_expr_easy2() {
    gen_test_env!(r#"5+~6"#, t);
    t.generate_code().unwrap();
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(Opcode::LoadInt, 3),
            Inst::new(Opcode::BitNotInt, NO_ARG),
            Inst::new(Opcode::AddInt, NO_ARG)
        ]
    );
}

#[test]
fn text_expr_easy3() {
    gen_test_env!(r#"9-8-8"#, t);
    t.generate_code().unwrap();
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(Opcode::LoadInt, 3),
            Inst::new(Opcode::SubInt, NO_ARG),
            Inst::new(Opcode::LoadInt, 3),
            Inst::new(Opcode::SubInt, NO_ARG)
        ]
    )
}

#[test]
fn test_expr_easy4() {
    gen_test_env!(r#"(8-9)*7"#, t);
    t.generate_code().unwrap();
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(Opcode::LoadInt, 3),
            Inst::new(Opcode::SubInt, NO_ARG),
            Inst::new(Opcode::LoadInt, 4),
            Inst::new(Opcode::MulInt, NO_ARG)
        ]
    )
}

#[test]
fn test_expr() {
    gen_test_env!(r#"1+9-10*7**6"#, t);
    t.generate_code().unwrap();
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadInt, INT_VAL_POOL_ONE),
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(Opcode::AddInt, NO_ARG),
            Inst::new(Opcode::LoadInt, 3),
            Inst::new(Opcode::LoadInt, 4),
            Inst::new(Opcode::LoadInt, 5),
            Inst::new(Opcode::PowerInt, NO_ARG),
            Inst::new(Opcode::MulInt, NO_ARG),
            Inst::new(Opcode::SubInt, NO_ARG),
        ]
    );
}

#[test]
fn test_expr_final() {
    gen_test_env!(r#"(1+-2)*3//4**(5**6)==1||7==(8&9)"#, t);
    t.generate_code().unwrap();
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadInt, INT_VAL_POOL_ONE),
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(Opcode::SelfNegativeInt, NO_ARG),
            Inst::new(Opcode::AddInt, NO_ARG),
            Inst::new(Opcode::LoadInt, 3),
            Inst::new(Opcode::MulInt, NO_ARG),
            Inst::new(Opcode::LoadInt, 4),
            Inst::new(Opcode::LoadInt, 5),
            Inst::new(Opcode::LoadInt, 6),
            Inst::new(Opcode::PowerInt, NO_ARG),
            Inst::new(Opcode::PowerInt, NO_ARG),
            Inst::new(Opcode::ExactDivInt, NO_ARG),
            Inst::new(Opcode::LoadInt, 1),
            Inst::new(Opcode::EqInt, NO_ARG),
            Inst::new(Opcode::LoadInt, 7),
            Inst::new(Opcode::LoadInt, 8),
            Inst::new(Opcode::LoadInt, 9),
            Inst::new(Opcode::BitAndInt, NO_ARG),
            Inst::new(Opcode::EqInt, NO_ARG),
            Inst::new(Opcode::OrBool, NO_ARG),
        ]
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
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(Opcode::StoreGlobalInt, get_offset(0)),
            Inst::new(Opcode::LoadString, 0),
            Inst::new(Opcode::LoadGlobalVarInt, get_offset(0)),
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(Opcode::AddInt, NO_ARG),
            Inst::new(Opcode::MoveInt, NO_ARG),
            Inst::new(Opcode::LoadInt, INT_VAL_POOL_ONE),
            Inst::new(
                Opcode::CallNative,
                get_prelude_function("print").unwrap().buildin_id
            ),
        ]
    )
}

#[test]
fn test_call_builtin_function() {
    gen_test_env!(r#"print("hello world!")"#, t);
    t.generate_code().unwrap();
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadString, 0),
            Inst::new(Opcode::LoadInt, INT_VAL_POOL_ZERO),
            Inst::new(
                Opcode::CallNative,
                get_prelude_function("print").unwrap().buildin_id
            ),
        ]
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
}

#[test]
#[should_panic(expected = "OperatorError")]
fn test_wrong_type3() {
    gen_test_env!(r#""90"+28"#, t);
    t.generate_code().unwrap();
}

#[test]
fn test_if_easy() {
    gen_test_env!(r#"if 1==1 { print("hello world") }"#, t);
    t.generate_code().unwrap();
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadInt, 1),
            Inst::new(Opcode::LoadInt, 1),
            Inst::new(Opcode::EqInt, NO_ARG),
            Inst::new(Opcode::JumpIfFalse, 7),
            Inst::new(Opcode::LoadString, 0),
            Inst::new(Opcode::LoadInt, INT_VAL_POOL_ZERO),
            Inst::new(
                Opcode::CallNative,
                get_prelude_function("print").unwrap().buildin_id
            ),
        ]
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
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(Opcode::StoreGlobalInt, 0),
            Inst::new(Opcode::LoadGlobalVarInt, 0),
            Inst::new(Opcode::LoadInt, 3),
            Inst::new(Opcode::LtInt, 0),
            Inst::new(Opcode::JumpIfFalse, 6),
            Inst::new(Opcode::Jump, 17),
            Inst::new(Opcode::LoadGlobalVarInt, 0),
            Inst::new(Opcode::LoadInt, 4),
            Inst::new(Opcode::GtInt, 0),
            Inst::new(Opcode::JumpIfFalse, 11),
            Inst::new(Opcode::Jump, 17),
            Inst::new(Opcode::LoadInt, 3),
            Inst::new(Opcode::LoadInt, 5),
            Inst::new(Opcode::EqInt, 0),
            Inst::new(Opcode::JumpIfFalse, 16),
            Inst::new(Opcode::Jump, 17)
        ]
    )
}

#[test]
fn test_var_params() {
    gen_test_env!(r#"print("{}{}{}", 1, 2, 3)"#, t);
    t.generate_code().unwrap();
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadString, 0),
            Inst::new(Opcode::LoadInt, INT_VAL_POOL_ONE),
            Inst::new(Opcode::MoveInt, NO_ARG),
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(Opcode::MoveInt, NO_ARG),
            Inst::new(Opcode::LoadInt, 3),
            Inst::new(Opcode::MoveInt, NO_ARG),
            Inst::new(Opcode::LoadInt, 3),
            Inst::new(
                Opcode::CallNative,
                get_prelude_function("print").unwrap().buildin_id
            ),
        ]
    )
}

#[test]
fn test_while_1() {
    gen_test_env!(r#"while 1==1 { print("hello world") }"#, t);
    t.generate_code().unwrap();
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadInt, 1),
            Inst::new(Opcode::LoadInt, 1),
            Inst::new(Opcode::EqInt, NO_ARG),
            Inst::new(Opcode::JumpIfFalse, 8),
            Inst::new(Opcode::LoadString, 0),
            Inst::new(Opcode::LoadInt, INT_VAL_POOL_ZERO),
            Inst::new(
                Opcode::CallNative,
                get_prelude_function("print").unwrap().buildin_id
            ),
            Inst::new(Opcode::Jump, 0)
        ]
    )
}

#[test]
fn test_for_1() {
    gen_test_env!(r#"for i:=0; i<10; i=i+1 { print("hello world") }"#, t);
    t.generate_code().unwrap();
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadInt, 0),
            Inst::new(Opcode::StoreGlobalInt, get_offset(0)),
            Inst::new(Opcode::LoadGlobalVarInt, get_offset(0)),
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(Opcode::LtInt, NO_ARG),
            Inst::new(Opcode::JumpIfFalse, 14),
            Inst::new(Opcode::LoadString, 0),
            Inst::new(Opcode::LoadInt, INT_VAL_POOL_ZERO),
            Inst::new(
                Opcode::CallNative,
                get_prelude_function("print").unwrap().buildin_id
            ),
            Inst::new(Opcode::LoadGlobalVarInt, get_offset(0)),
            Inst::new(Opcode::LoadInt, 1),
            Inst::new(Opcode::AddInt, NO_ARG),
            Inst::new(Opcode::StoreGlobalInt, get_offset(0)),
            Inst::new(Opcode::Jump, 2)
        ]
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
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::Stop, NO_ARG),
            Inst::new(Opcode::LoadString, 0),
            Inst::new(Opcode::LoadInt, INT_VAL_POOL_ZERO),
            Inst::new(
                Opcode::CallNative,
                get_prelude_function("print").unwrap().buildin_id
            ),
            Inst::new(Opcode::PopFrame, NO_ARG),
            Inst::new(Opcode::LoadString, 1),
            Inst::new(Opcode::LoadInt, INT_VAL_POOL_ZERO),
            Inst::new(
                Opcode::CallNative,
                get_prelude_function("print").unwrap().buildin_id
            ),
            Inst::new(Opcode::CallCustom, 0),
            Inst::new(Opcode::PopFrame, NO_ARG)
        ]
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
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::CallCustom, 0),
            Inst::new(Opcode::Stop, NO_ARG),
            Inst::new(Opcode::LoadString, 0),
            Inst::new(Opcode::LoadInt, INT_VAL_POOL_ZERO),
            Inst::new(
                Opcode::CallNative,
                get_prelude_function("print").unwrap().buildin_id
            ),
            Inst::new(Opcode::PopFrame, NO_ARG)
        ]
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
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadInt, 0),
            Inst::new(Opcode::LoadInt, 1),
            Inst::new(Opcode::CallCustom, 0),
            Inst::new(Opcode::LoadInt, INT_VAL_POOL_ONE),
            Inst::new(Opcode::StoreGlobalInt, get_offset(0)),
            Inst::new(Opcode::LoadInt, INT_VAL_POOL_ZERO),
            Inst::new(Opcode::StoreGlobalInt, get_offset(1)),
            Inst::new(Opcode::LoadString, 0),
            Inst::new(Opcode::LoadGlobalVarInt, get_offset(0)),
            Inst::new(Opcode::MoveInt, NO_ARG),
            Inst::new(Opcode::LoadGlobalVarInt, get_offset(1)),
            Inst::new(Opcode::MoveInt, NO_ARG),
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(
                Opcode::CallNative,
                get_prelude_function("print").unwrap().buildin_id
            ),
            Inst::new(Opcode::Stop, NO_ARG),
            Inst::new(Opcode::StoreLocalInt, get_offset(0)),
            Inst::new(Opcode::StoreLocalInt, get_offset(1)),
            Inst::new(Opcode::LoadString, 0),
            Inst::new(Opcode::LoadLocalVarInt, get_offset(1)),
            Inst::new(Opcode::MoveInt, NO_ARG),
            Inst::new(Opcode::LoadLocalVarInt, get_offset(0)),
            Inst::new(Opcode::MoveInt, NO_ARG),
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(
                Opcode::CallNative,
                get_prelude_function("print").unwrap().buildin_id
            ),
            Inst::new(Opcode::PopFrame, NO_ARG)
        ]
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
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadInt, 0),
            Inst::new(Opcode::StoreGlobalInt, 0),
            Inst::new(Opcode::LoadGlobalVarInt, 0),
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(Opcode::LeInt, 0),
            Inst::new(Opcode::JumpIfFalse, 26),
            Inst::new(Opcode::LoadGlobalVarInt, 0),
            Inst::new(Opcode::LoadInt, 3),
            Inst::new(Opcode::EqInt, 0),
            Inst::new(Opcode::JumpIfFalse, 11),
            Inst::new(Opcode::Jump, 21),
            Inst::new(Opcode::LoadGlobalVarInt, 0),
            Inst::new(Opcode::LoadInt, 4),
            Inst::new(Opcode::EqInt, 0),
            Inst::new(Opcode::JumpIfFalse, 16),
            Inst::new(Opcode::Jump, 26),
            Inst::new(Opcode::LoadString, 0),
            Inst::new(Opcode::LoadGlobalVarInt, 0),
            Inst::new(Opcode::MoveInt, 0),
            Inst::new(Opcode::LoadInt, 1),
            Inst::new(Opcode::CallNative, 1),
            Inst::new(Opcode::LoadGlobalVarInt, 0),
            Inst::new(Opcode::LoadInt, 1),
            Inst::new(Opcode::AddInt, 0),
            Inst::new(Opcode::StoreGlobalInt, 0),
            Inst::new(Opcode::Jump, 2),
            Inst::new(Opcode::LoadInt, 0),
            Inst::new(Opcode::StoreGlobalInt, 8),
            Inst::new(Opcode::LoadGlobalVarInt, 8),
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(Opcode::LtInt, 0),
            Inst::new(Opcode::JumpIfFalse, 52),
            Inst::new(Opcode::LoadGlobalVarInt, 8),
            Inst::new(Opcode::LoadInt, 1),
            Inst::new(Opcode::AddInt, 0),
            Inst::new(Opcode::StoreGlobalInt, 8),
            Inst::new(Opcode::LoadGlobalVarInt, 8),
            Inst::new(Opcode::LoadInt, 3),
            Inst::new(Opcode::EqInt, 0),
            Inst::new(Opcode::JumpIfFalse, 41),
            Inst::new(Opcode::Jump, 28),
            Inst::new(Opcode::LoadString, 0),
            Inst::new(Opcode::LoadGlobalVarInt, 8),
            Inst::new(Opcode::MoveInt, 0),
            Inst::new(Opcode::LoadInt, 1),
            Inst::new(Opcode::CallNative, 1),
            Inst::new(Opcode::LoadGlobalVarInt, 8),
            Inst::new(Opcode::LoadInt, 4),
            Inst::new(Opcode::EqInt, 0),
            Inst::new(Opcode::JumpIfFalse, 51),
            Inst::new(Opcode::Jump, 52),
            Inst::new(Opcode::Jump, 28),
        ]
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
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadString, 1),
            Inst::new(Opcode::LoadFloat, 0),
            Inst::new(
                Opcode::CallNative,
                get_stdlib().sub_modules["math"].functions["sin"].buildin_id
            ),
            Inst::new(Opcode::MoveFloat, 0),
            Inst::new(Opcode::LoadInt, 1),
            Inst::new(Opcode::CallNative, 0),
        ]
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
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(Opcode::StoreGlobalInt, 0),
            Inst::new(Opcode::LoadGlobalVarInt, 0),
            Inst::new(Opcode::LoadInt, 1),
            Inst::new(Opcode::EqIntWithoutPop, 0),
            Inst::new(Opcode::JumpIfTrue, 7),
            Inst::new(Opcode::Jump, 13),
            Inst::new(Opcode::LoadString, 0),
            Inst::new(Opcode::LoadGlobalVarInt, 0),
            Inst::new(Opcode::MoveInt, 0),
            Inst::new(Opcode::LoadInt, 1),
            Inst::new(Opcode::CallNative, 1),
            Inst::new(Opcode::Jump, 32),
            Inst::new(Opcode::LoadInt, 3),
            Inst::new(Opcode::EqIntWithoutPop, 0),
            Inst::new(Opcode::JumpIfTrue, 20),
            Inst::new(Opcode::LoadInt, 2),
            Inst::new(Opcode::EqIntWithoutPop, 0),
            Inst::new(Opcode::JumpIfTrue, 20),
            Inst::new(Opcode::Jump, 26),
            Inst::new(Opcode::LoadString, 0),
            Inst::new(Opcode::LoadGlobalVarInt, 0),
            Inst::new(Opcode::MoveInt, 0),
            Inst::new(Opcode::LoadInt, 1),
            Inst::new(Opcode::CallNative, 1),
            Inst::new(Opcode::Jump, 32),
            Inst::new(Opcode::Jump, 28),
            Inst::new(Opcode::Jump, 32),
            Inst::new(Opcode::LoadString, 1),
            Inst::new(Opcode::LoadInt, 0),
            Inst::new(Opcode::CallNative, 1),
            Inst::new(Opcode::Jump, 32),
            Inst::new(Opcode::LoadString, 2),
            Inst::new(Opcode::LoadInt, 0),
            Inst::new(Opcode::CallNative, 1),
        ]
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
    assert_eq!(
        t.staticdata.inst,
        vec![
            Inst::new(Opcode::LoadString, 0),
            Inst::new(Opcode::StoreGlobalStr, 0),
            Inst::new(Opcode::LoadGlobalVarStr, 0),
            Inst::new(Opcode::LoadString, 0),
            Inst::new(Opcode::EqStrWithoutPop, 0),
            Inst::new(Opcode::JumpIfTrue, 10),
            Inst::new(Opcode::LoadString, 1),
            Inst::new(Opcode::EqStrWithoutPop, 0),
            Inst::new(Opcode::JumpIfTrue, 10),
            Inst::new(Opcode::Jump, 16),
            Inst::new(Opcode::LoadString, 2),
            Inst::new(Opcode::LoadGlobalVarStr, 0),
            Inst::new(Opcode::MoveStr, 0),
            Inst::new(Opcode::LoadInt, 1),
            Inst::new(Opcode::CallNative, 1),
            Inst::new(Opcode::Jump, 22),
            Inst::new(Opcode::Jump, 18),
            Inst::new(Opcode::Jump, 22),
            Inst::new(Opcode::LoadString, 3),
            Inst::new(Opcode::LoadInt, 0),
            Inst::new(Opcode::CallNative, 1),
            Inst::new(Opcode::Jump, 22),
        ]
    )
}
