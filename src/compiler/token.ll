%{
#include "compiler.hpp"
#include "parser.tab.hpp"

// A number symbol corresponding to the value in S.
  yy::parser::symbol_type
  make_NUMBER (const std::string &s, const yy::parser::location_type& loc);
%}

/* %option noyywrap nounput noinput batch debug */
%option noyywrap batch debug

string  \"[^\n"]+\"

blank [ \t\r]

alpha   [A-Za-z]
ids     alpha|_
dig     [0-9]
name    ({ids}|{dig}|\$)({ids}|{dig}|[_.\-/$])*
num1    [-+]?{dig}+\.?([eE][-+]?{dig}+)?
num2    [-+]?{dig}*\.{dig}+([eE][-+]?{dig}+)?
number  {num1}|{num2}

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns (yyleng);
%}

%%
%{
  // A handy shortcut to the location held by the driver.
  yy::location& loc = drv.location;
  // Code run each time yylex is called.
  loc.step ();
%}
{blank}+  loc.step ();

"/*"    {
        int c;
        while((c = yyinput()) != 0) {
            if(c == '\n') {
                loc.lines (yyleng);
                loc.step ();
            } else if(c == '*') {
                if((c = yyinput()) == '/') {
                    break;
                } else
                    unput(c);
                }
            }
        }

{number}  

\n+        loc.lines (yyleng); loc.step (); 

{name}    

{string}  

%%

namesapce trc::compiler {
    void compiler::scan_begin(const std::string&path) {
        yy_flex_debug = trace_scanning;
        yyin = open_file(file);
    }

    void compiler::scan_begin(FILE* file) {
        yy_flex_debug = trace_scanning;
        yyin = file;
    }

    void compiler::scan_end () {
        fclose(yyin);
    }   
}
