// encoding: utf-8
// language: c++
// author:   0.382

#ifndef JSHL_COLOR_H
#define JSHL_COLOR_H

#include <iostream>
#include <windows.h>

namespace color{

// 一些用于控制的数
static const WORD _foreground_attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;  // 所有的前景色标志位
static const WORD _background_attributes = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;  // 所有的背景色标志位
static const WORD _special_flag = WORD(0x2000);                                                                         // 在官方文档中，没有这一位的说明，用这个来set，结果是false

// 获得控制台默认的颜色设置
WORD get_default_color(){
    HANDLE hd = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO current_screen_buffer_info;
    GetConsoleScreenBufferInfo(hd, &current_screen_buffer_info);
    return current_screen_buffer_info.wAttributes & (_foreground_attributes | _background_attributes);
}

static const WORD _default_color = get_default_color();       // 一开始就获得默认的颜色设置
static       WORD _current_color = _default_color;            // 记录当前的颜色设置 

// 表示颜色的结构体
struct Color{
    Color(const WORD &c) : color(c) {}
    WORD color;
};

// 颜色常数
// 黑色，由于黑色为全部是0，所以这里特别定义一下
static const Color foreground_black = Color(_special_flag | _foreground_attributes);
static const Color background_black = Color(_special_flag | _background_attributes);
static const Color all_black        = Color(WORD(0));                                 // 对于为0的情况，会特别处理

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
// 回复默认设置
static const Color resetcolor       = Color(_special_flag);                                                  // 控制回复默认颜色的操作符

// 重载算符使得输出流可以控制颜色
std::ostream &operator<<(std::ostream &os, const Color &color){
    WORD color_to_set = color.color;
    if(color_to_set & _special_flag)
    {
        if((color_to_set & _foreground_attributes)==_foreground_attributes && (color_to_set & _background_attributes)==_background_attributes)
        {
            color_to_set = WORD(0);
        }
        else if((color_to_set & _foreground_attributes)==_foreground_attributes)
        {
            color_to_set = color_to_set & _background_attributes ? color_to_set & _background_attributes : _current_color & _background_attributes;
        }
        else if((color_to_set & _background_attributes)==_background_attributes)
        {
            color_to_set = color_to_set & _foreground_attributes ? color_to_set & _foreground_attributes : _current_color & _foreground_attributes;
        }
        else
        {
            color_to_set = _default_color;
        }
    }
    else
    {
        if((color_to_set & _foreground_attributes) && (color_to_set & _background_attributes)){}
        else if(color_to_set & _foreground_attributes)
        {
            color_to_set = color_to_set | (_current_color & _background_attributes);
        }
        else if(color_to_set & _background_attributes)
        {
            color_to_set = color_to_set | (_current_color & _foreground_attributes);
        }
        else
        {
            color_to_set = WORD(0);
        }
    }
    HANDLE color_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(color_handle, color_to_set);
    _current_color = color_to_set;
    return os;
}

// 表示颜色混合，比如蓝色加绿色等于黄色
// 其实还是为了解决我现在不确定怎么实现比较好的同时设置前景色和背景色的问题
Color operator|(const Color &ca, const Color &cb){
    return Color(ca.color | cb.color);
}

// 同样表示颜色混合算了，反正如果采用位与运算这种情况似乎也用不到
Color operator&(const Color &ca, const Color &cb){
    return Color(ca.color | cb.color);
}

} // end namespace color

#endif // JSHL_COLOR_H