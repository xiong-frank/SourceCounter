#include <iostream>

int main(/* args */)
{
    // show something
    std::cout << R"(Hello "Frank")" << std::endl;
    std::cout << R"(open file(/* only write */): C:\\users\Frank\demo.cpp)" << std::endl;
    std::cout << R"(write: \" hi: \\" Jack)" << std::endl;
    std::cout << R"abc(reply: "(/*msg*/)" // )abc" << std::endl;
    const char* text = R"(
    Hi:
      Jack, "are you ok?"
    
    your friend: Frank // 2020-01-01
    )";
    
    return 0;
}
