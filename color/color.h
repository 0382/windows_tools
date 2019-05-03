// encoding: utf-8
// language: c++
// author:   0.382

#ifndef JSHL_COLOR_H
#define JSHL_COLOR_H

#include <iostream>
#include <windows.h>
#include <map>
#include <string>
#include <regex>
#include <exception>


namespace color{

// map from color words to windows api color code
static const std::map<std::string, int> color_dict = {
 {"black", 0},  {"blue",    1},  {"green",   2},  {"aqua",   3},
 {"red",   4},  {"purple",  5},  {"yellow",  6},  {"white",  7},
 {"gray",  8},  {"lblue",   9},  {"lgreen",  10}, {"laqua",  11},
 {"lred",  12}, {"lpurple", 13}, {"lyellow", 14}, {"bwhite", 15}
};

static const char* _color_regex_str = R"((Black|Blue|Green|Aqua|Red|Purple|Yellow|White|Gray|LBlue|LGreen|LAqua|LRed|LPurple|LYellow|BWhite)[,;:.?!<>|\\/@#$%\^\&\*-\+= \t]*(Black|Blue|Green|Aqua|Red|Purple|Yellow|White|Gray|LBlue|LGreen|LAqua|LRed|LPurple|LYellow|BWhite)*)";
static const std::regex color_regex(_color_regex_str, std::regex::icase);

static const WORD _foreground_attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;  // all foreground color bits
static const WORD _background_attributes = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;  // all background color bits

std::string lower(std::string s){
    const char step = 'a' - 'A';
    for(auto &c : s) if(c >= 'A' && c <= 'Z') c += step;
    return s;
}

// change string to windows api color code
// return -1 indicates wrong string, 
int str_to_code(const std::string &str){
    if(str.empty()) return -1;
    if(str.size()<=2)
    {
        try{
            return std::stoi(str, nullptr, 16);
        } catch (std::exception error) {
            return -1;
        }
    }
    std::smatch sm;
    std::regex_search(str, sm, color_regex);
    if(sm.empty()) return -1;
    else
    {
        const std::string color_one = lower(sm.str(1));
        const std::string color_two = lower(sm.str(2));
        if(color_two == "") return color_dict.at(color_one);
        else return color_dict.at(color_one) << 4 | color_dict.at(color_two);
    }
}

// color struct
struct Color{
    Color(const WORD &c) : color(c) {}
    Color(const std::string &s) : color(str_to_code(s)) {}
    WORD color;
};

// get console color setting
Color get_current_color(){
    HANDLE hd = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO current_screen_buffer_info;
    GetConsoleScreenBufferInfo(hd, &current_screen_buffer_info);
    return Color(current_screen_buffer_info.wAttributes & (_foreground_attributes | _background_attributes));
}

Color get_background_color(const Color &color){
    return Color(color.color & _background_attributes);
}

Color get_foreground_color(const Color &color){
    return Color(color.color % _foreground_attributes);
}

// set color
int setcolor(const Color &color){
    return SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color.color);
}

// set color use ostream
std::ostream &operator<<(std::ostream &os, const Color &color){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color.color);
    return os;
}

// 表示颜色混合，比如蓝色加绿色等于黄色
Color operator|(const Color &ca, const Color &cb){
    return Color(ca.color | cb.color);
}

// 前景色
static const Color foreground_red    = Color(FOREGROUND_RED);                                                // 红色
static const Color foreground_greed  = Color(FOREGROUND_GREEN);                                              // 绿色
static const Color foreground_blue   = Color(FOREGROUND_BLUE);                                               // 蓝色
static const Color foreground_aqua   = Color(FOREGROUND_GREEN | FOREGROUND_BLUE);                            // 浅绿色= 绿色+蓝色
static const Color foreground_purple = Color(FOREGROUND_RED | FOREGROUND_BLUE);                              // 紫色  = 红色+蓝色
static const Color foreground_yellow = Color(FOREGROUND_RED | FOREGROUND_GREEN);                             // 黄色  = 红色+绿色
static const Color foreground_wight  = Color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);           // 白色
static const Color foreground_gray   = Color(FOREGROUND_INTENSITY);                                          // 灰色
static const Color foreground_light_red    = Color(FOREGROUND_RED | FOREGROUND_INTENSITY);                   // 加 light 的都是原来颜色的淡色
static const Color foreground_light_green  = Color(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
static const Color foreground_light_blue   = Color(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
static const Color foreground_light_aqua   = Color(FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_BLUE);
static const Color foreground_light_purple = Color(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
static const Color foreground_light_yellow = Color(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
static const Color foreground_bright_wight = Color(_foreground_attributes);                                  // 亮白 
// 背景色
static const Color background_red    = Color(BACKGROUND_RED);                                                // 和前景色一样
static const Color background_greed  = Color(BACKGROUND_GREEN);
static const Color background_blue   = Color(BACKGROUND_BLUE);
static const Color background_aqua   = Color(BACKGROUND_GREEN | BACKGROUND_BLUE);
static const Color background_purple = Color(BACKGROUND_RED | BACKGROUND_BLUE);
static const Color background_yellow = Color(BACKGROUND_RED | BACKGROUND_GREEN);
static const Color background_wight  = Color(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
static const Color background_gray   = Color(BACKGROUND_INTENSITY);
static const Color background_light_red    = Color(BACKGROUND_RED | BACKGROUND_INTENSITY);
static const Color background_light_green  = Color(BACKGROUND_GREEN | BACKGROUND_INTENSITY);
static const Color background_light_blue   = Color(BACKGROUND_BLUE | BACKGROUND_INTENSITY);
static const Color background_light_aqua   = Color(BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_BLUE);
static const Color background_light_purple = Color(BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY);
static const Color background_light_yellow = Color(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY);
static const Color background_bright_wight = Color(_background_attributes);

} // end namespace color

#endif // JSHL_COLOR_H