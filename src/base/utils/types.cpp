#include "base/utils/types.h"

using namespace std;

namespace trc
{
    namespace utils
    {
        int len(int in)
        {
            int out = 0;
            do
            {
                in /= 10;
                ++out;
            } while (in != 0);
            return out;
        }
    }
}