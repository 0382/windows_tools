#include "color.h"

int main(){
    std::cout << color::foreground_purple;
    std::cout << "this is purple text.\n";
    std::cout << (color::background_red | color::foreground_black);
    std::cout << "this is red text.\n";
    std::cout << color::resetcolor;
    std::cout << "this is normal text.";
    return 0;
}