// C++ implementation of the approach
#include "strXor.h"



// Driver code
int main()
{
    std::string a = "11001", b = "111111";
    std::string res = "";
    getXOR(a, b, res);
    std::cout << res << std::endl;
    std::cout << a << std::endl;

    return 0;
}
