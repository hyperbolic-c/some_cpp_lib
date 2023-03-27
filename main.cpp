#include "string_view.h"
#include <typeinfo>

int main()
{
    const char *data = "helloWorld 12345678";
    david::string_view sv1{data};
    david::string_view sv2{data, 5};
    david::string_view sv3{data + 3, 3};
    david::string_view sv4{data + 5};
    std::string str{"helloWorld"};
    // std::string view_str{sv1};
    david::string_view sv5{str};
    //std::string string = static_cast<std::string>(sv5);
    // auto add = sv5.begin();
    
    // std::cout << &add << std::endl;
    std::cout << &sv5 << std::endl;
    std::cout << typeid((&sv5)).name() << std::endl;
    std::cout << typeid(sv5.data()).name() << std::endl;
    std::cout << typeid(str).name() << std::endl;
    std::cout << typeid(std::string(sv5.data())).name() << std::endl;
    // std::cout << typeid(sv5.data_).name() << std::endl;
    // std::cout << data << std::endl;
    // std::cout << sv1 << std::endl;
    // std::cout << sv2 << std::endl;
    // std::cout << sv3 << std::endl;
    // std::cout << sv4 << std::endl;
    std::cout << sv5 << std::endl;
    // std::cout << string << std::endl;
    return 0;
}

// #include "string_view1.h"

// int main()
// {
//     const char *data = "helloWorld 12345678";
//     fstd::string_view sv1{data};
//     fstd::string_view sv2{data, 5};
//     fstd::string_view sv3{data + 3, 3};
//     fstd::string_view sv4{data + 5};

//     std::string str{"hello world"};
//     fstd::string_view sv5{str};
//     std::string string = static_cast<std::string>(sv5);

//     std::cout << data << std::endl;
//     std::cout << sv1 << std::endl;
//     std::cout << sv2 << std::endl;
//     std::cout << sv3 << std::endl;
//     std::cout << sv4 << std::endl;
//     std::cout << sv5 << std::endl;
//     std::cout << string << std::endl;   

//     return 0;
// }

