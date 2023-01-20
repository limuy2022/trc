/**
 * 生成语法树并且生成字节码
 * 语法制导翻译
 */

#include <Compiler/Compiler.hpp>
#include <language/error.hpp>
#include <string>

#define LINE_NUM compiler_data.error.get_line()

namespace trc::compiler {
void Compiler::generate_line_table(line_t line) {
    // 判断是否需要生成行号表
    if (!compiler_data.option.number_table) {
        return;
    }
    compiler_data.vm.line_number_table.push_back(line);
    if (line != prev_value) {
        while(line_to_bycodeindex_table.size() != line) {
            line_to_bycodeindex_table.push_back(compiler_data.vm.byte_codes.size() - 1);
        }
        prev_value = line;
    }
}

void Compiler::add_opcode(
    byteCodeNumber opcode, TVM_space::bytecode_index_t index) {
    local->bytecode.emplace_back(opcode, index);
    generate_line_table(LINE_NUM);
}

bool Compiler::id(bool error_report) {
    // 变量名节点，生成读取变量的字节码
    auto t = token_.get_token();
    auto varname = t.data;
    // 获取变量的索引
    size_t index_argv;
    // 首先尝试在局部变量中查找
    if (local == &infoenv) {
        // 相等，说明现在处于全局作用域中
        index_argv = local->get_index_of_var(varname, error_report);
        if (index_argv == unsave) {
            goto failed;
        }
        add_opcode(byteCodeNumber::LOAD_NAME, index_argv);
    } else {
        index_argv = local->get_index_of_var(varname, false);
        if (index_argv == unsave) {
            // 如果在局部变量中没有查找到
            index_argv = infoenv.get_index_of_var(varname, error_report);
            if (index_argv == unsave) {
                goto failed;
            }
            add_opcode(byteCodeNumber::LOAD_NAME, index_argv);
        } else {
            add_opcode(byteCodeNumber::LOAD_LOCAL, index_argv);
        }
    }
    return true;
failed:
    token_.unget_token(t);
    return false;
}

Compiler::Compiler(const std::string& module_name,
    const compiler_option& option, TVM_space::TVM_static_data& vm)
    : compiler_data(module_name, option, vm)
    , infoenv(compiler_data, vm.byte_codes)
    , token_(compiler_data) {
}

void Compiler::compile(const std::string& codes) {
    token_.set_code(codes);
    local = &infoenv;
    while (get_next_token_tick() != token_ticks::END_OF_TOKENS) {
        sentence();
    }
    // 设置全局符号表
    compiler_data.vm.global_symbol_table_size = infoenv.get_name_size();
    // 压缩内存
    compiler_data.vm.compress_memory();
}

void Compiler::argv() {
    do {
        value();
        if (get_next_token_tick() != token_ticks::COMMA) {
            break;
        }
        token_.get_token();
    } while (true);
}

bool Compiler::const_value() {
    token data_token = token_.get_token();
    switch (data_token.tick) {
    case token_ticks::INT_VALUE: {
        add_opcode(byteCodeNumber::LOAD_INT, data_token.data);
        return true;
    }
    case token_ticks::FLOAT_VALUE: {
        add_opcode(byteCodeNumber::LOAD_FLOAT, data_token.data);
        return true;
    }
    case token_ticks::STRING_VALUE: {
        add_opcode(byteCodeNumber::LOAD_STRING, data_token.data);
        return true;
    }
    case token_ticks::TRUE_: {
        // 常量true转换成数字1
        // 第1项是数字1
        add_opcode(byteCodeNumber::LOAD_INT, 1);
        return true;
    }
    case token_ticks::FALSE_:
    case token_ticks::NULL_: {
        add_opcode(byteCodeNumber::LOAD_INT, 0);
        return true;
    }
    case token_ticks::LONG_INT_VALUE: {
        add_opcode(byteCodeNumber::LOAD_LONG, data_token.data);
        return true;
    }
    default: {
        token_.unget_token(data_token);
        return false;
    }
    }
}

token Compiler::clear_enter() {
    token now;
    do {
        now = token_.get_token();
    } while (now.tick == token_ticks::END_OF_LINE);
    return now;
}

bool Compiler::sentence() {
    auto now = clear_enter();
    switch (now.tick) {
    case token_ticks::IF: {
        add_block<false>();
        return true;
    }
    case token_ticks::WHILE: {
        add_block<true>();
        return true;
    }
    case token_ticks::NAME: {
        switch (get_next_token_tick()) {
        case token_ticks::ASSIGN: {
            // 变量赋值
            token_.get_token();
            item(true);
            auto index_argv = local->get_index_of_var(now.data, true);
            if (local == &infoenv) {
                // 全局环境
                add_opcode(byteCodeNumber::CHANGE_VALUE, index_argv);
            } else {
                // 局部环境
                add_opcode(byteCodeNumber::CHANGE_LOCAL, index_argv);
            }
            break;
        }
        case token_ticks::STORE: {
            // 变量定义
            token_.get_token();
            item(true);
            auto index_argv = local->add_var(now.data);
            if (local == &infoenv) {
                // 全局环境
                add_opcode(byteCodeNumber::STORE_NAME, index_argv);
            } else {
                // 局部环境
                add_opcode(byteCodeNumber::STORE_LOCAL, index_argv);
            }
            break;
        }
        case token_ticks::LEFT_SMALL_BRACE: {
            token_.get_token();
            argv();
            token_.get_token();
            break;
        }
        default: {
            compiler_data.error.send_error_module(
                error::SyntaxError, language::error::syntaxerror);
        }
        }
        return true;
    }
    case token_ticks::FUNC: {
        return true;
    }
    case token_ticks::ASSERT: {
        argv();
        return true;
    }
    case token_ticks::GOTO: {
        token t = token_.get_token();
        if (t.tick != token_ticks::INT_VALUE) {
            compiler_data.error.send_error_module(
                error::SyntaxError, language::error::syntaxerror);
        }
        add_opcode(byteCodeNumber::GOTO, t.data);
    }
    default: {
        return false;
    }
    }
}

bool Compiler::value() {
    return const_value() || id(false);
}

bool Compiler::factor() {
    if (value()) {
        return true;
    }
    return match(token_ticks::LEFT_SMALL_BRACE) && expr()
        && match(token_ticks::RIGHT_SMALL_BRACE);
}

bool Compiler::term(bool first_call) {
    if (first_call&&term(false)) {
        byteCodeNumber bytecode;
        if (match(token_ticks::MUL)) {
            bytecode = byteCodeNumber::MUL;
        } else if (match(token_ticks::DIV)) {
            bytecode = byteCodeNumber::DIV;
        } else if (match(token_ticks::POW)) {
            bytecode = byteCodeNumber::POW;
        } else if (match(token_ticks::ZDIV)) {
            bytecode = byteCodeNumber::ZDIV;
        }
        if (!factor()) {
            return false;
        }
        add_opcode(bytecode, 0);
    } else {
        return factor();
    }
}

bool Compiler::expr(bool first_call) {
    if (first_call&&expr(false)) {
        byteCodeNumber bytecode;
        if (match(token_ticks::ADD)) {
            bytecode = byteCodeNumber::ADD;
        } else if (match(token_ticks::SUB)) {
            bytecode = byteCodeNumber::SUB;
        }
        if (!term()) {
            return false;
        }
        add_opcode(bytecode, 0);
    } else {
        return term();
    }
}

bool Compiler::func_call() {
    if (!id(false)) {
        return false;
    }
    match(token_ticks::LEFT_SMALL_BRACE);
    argv();
    match(token_ticks::RIGHT_SMALL_BRACE);
}

bool Compiler::item(bool error_reoprt) {
    return expr() || func_call() || value();
}

bool Compiler::match(token_ticks tick) {
    token res = token_.get_token();
    if (res.tick != tick) {
        token_.unget_token(res);
        return false;
    }
    return true;
}

token_ticks Compiler::get_next_token_tick() {
    token next_tmp = token_.get_token();
    auto nexttick = next_tmp.tick;
    token_.unget_token(next_tmp);
    return nexttick;
}
}
