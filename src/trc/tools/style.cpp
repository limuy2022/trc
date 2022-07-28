#include <tools.h>
#include <Compiler/token.h>
#include <Compiler/pri_compiler.hpp>
#include <string>
#include <base/utils/filesys.h>
#include <cstdio>

namespace trc::tools {
namespace tools_in {
    void __style_file(const char* filepath) {
        std::string filedata;
        utils::readcode(filedata, filepath);
        // 覆盖掉该文件的所有内容
        FILE* file = fopen(filepath, "w");
        // 调用token解析器进行解析
        compiler::compiler_public_data shared_data{compiler::compiler_error("__main__"), &compiler::nooptimize_option};
        // 清空文件并通过token_lex解析过滤掉所有个人风格然后再全部重新按照规则排列，是为了让代码风格更加统一
        compiler::token_lex lexer(filedata, shared_data);
        // 指向现在解析的token
        compiler::token* now_write;
        // 记录token解析开始时和结束时位置
        const char*token_begin, *token_end;
        // 用于控制缩进的代码
        int tab_size = 0;
        for(;;) {
            // 此处需要截取出具体的字符串，因为token_lex返回的token大部分都不会带字符串信息
            // 所以必须记录开始和结尾的字符串指针位置相减得到具体字符串
            token_begin = lexer.get_charptr();
            now_write = lexer.get_token();
            token_end = lexer.get_charptr();
            if(now_write->tick == compiler::token_ticks::END_OF_TOKENS) {
                break;
            }
            compiler::token_ticks tick = now_write->tick;
            if(compiler::is_cal_token(tick)||compiler::is_condit_token(tick)) {
                // 运算符左右加空格
                fputc(' ', file);
                utils::write_to_file(file, token_begin, token_end-token_begin);
                fputc(' ', file);
            } else if(tick == compiler::token_ticks::COMMA) {
                // 逗号后空一格
                utils::write_to_file(file, token_begin, token_end-token_begin);
                fputc(' ', file);
            } else {
                // 未匹配到需要格式化的代码直接按照原样写入文件
                utils::write_to_file(file, token_begin, token_end-token_begin);
            }
        }
        fclose(file);
    }
}

namespace tools_out {
    void style() {
        for (int i = 2; i < argc; ++i) {
            tools_in::__style_file(argv[i]);
        }
    }
}
}
