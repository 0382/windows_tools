#include "color.h"

int main(){
    auto origin_color = color::get_current_color();
    std::cout << color::Color("red;blue") << "Hello world!" << std::endl;
    std::cout << color::Color("lyellow") << "this is light yellow text" << std::endl;
    std::cout << origin_color;
    return 0;
}