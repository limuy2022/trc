#include <csetjmp>

namespace trc::error::error_env {
bool quit = true;
jmp_buf error_back_place;
}
