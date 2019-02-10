#ifndef JSHL_WINDOWS_SEARCH_H
#define JSHL_WINDOWS_SEARCH_H

#include <iostream>
#include <windows.h>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <regex>

namespace jshl{

// 帮助文档
static const char* usagetext = "usage: search [options] pattern [directory]";
static const char* helptext = \
"    search files in command line (only for windows)\n\n\
arguements:\n\
  pattern               pattern for search\n\
  directory             set search directory if given\n\n\
options:\n\
  -h, --help            show this help message and exit\n\
  -r                    if this arguement exists, use regex to search";

// 表征命令行参数情况的结构体
// 如果 "-r" 参数存在，那么就用正则表达式搜索
// 否则使用 windows 通配符搜索
// 如果搜索目录参数为空，则在当前目录下搜索
struct SearchArgs{
    bool exit = false;       // 参数错误，退出
    bool help = false;       // 有 help 参数存在
    bool ifregex = false;    // 使用正则表达式（可选）
    std::string pattern;     // 搜索模式
    std::string dir_name;    // 搜索目录名称（可选）
};

// 解析命令行参数
SearchArgs search_parse_args(int argc, char const* argv[]){
    SearchArgs arguements;
    // 如果没有命令行参数，退出
    if(argc == 1)
    {
        arguements.exit = true;
        return arguements;
    }
    std::vector<std::string> args;
    for(int i=1; i<argc; ++i) args.push_back(argv[i]);
    // help 参数
    auto pos = std::find_if(args.cbegin(), args.cend(), [](const std::string &word){return word=="-h" || word=="--help";});
    if(pos != args.cend())
    {
        arguements.help = true;
        return arguements;
    }
    // -r 参数
    pos = std::find(args.cbegin(), args.cend(), "-r");
    if(pos != args.cend())
    {
        arguements.ifregex = true;
        pos = args.erase(pos);
    }
    // 如果没有其他参数
    if(args.size() == 0)
    {
        arguements.exit = true;
        return arguements;
    }
    // 只有一个位置参数为搜索模式
    if(args.size() == 1)
    {
        arguements.pattern = args[0];
    }
    // 有两个位置参数，第二个为指定的目录
    if(args.size() == 2)
    {
        arguements.pattern = args[0];
        arguements.dir_name = args[1];
    }
    return arguements;
}

// 打印正则匹配结果
std::ostream &operator<<(std::ostream &os, const std::smatch &sm){
    os << sm.prefix();
    HANDLE hd = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hd, FOREGROUND_RED);
    os << sm.str();
    SetConsoleTextAttribute(hd, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    os << sm.suffix();
    return os;
}

// 打印模式匹配的结果
void print_search_result(const char* dir_name, const char* file_name){
    std::cout << dir_name << "\\";
    HANDLE hd = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hd, FOREGROUND_RED);
    std::cout << file_name;
    SetConsoleTextAttribute(hd, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << std::endl;
}

// 用通配符搜索
void search_file_with_pattern(const std::string &dir_name, const char *pattern){
    char *search_pattern = new char[MAX_PATH];
    WIN32_FIND_DATAA current_file;
    std::strcpy(search_pattern, dir_name.c_str());
    std::strcat(search_pattern, "\\");
    std::strcat(search_pattern, pattern);
    HANDLE search_handle = FindFirstFileA(search_pattern, &current_file);
    if(search_handle != INVALID_HANDLE_VALUE)
    {
        if(std::strcmp(current_file.cFileName, ".") != 0 && std::strcmp(current_file.cFileName, "..") != 0)
        {
            print_search_result(dir_name.c_str(), current_file.cFileName);
        }
        while(FindNextFileA(search_handle, &current_file))
        {
            if(std::strcmp(current_file.cFileName, ".") == 0 || std::strcmp(current_file.cFileName, "..") == 0) continue;
            print_search_result(dir_name.c_str(), current_file.cFileName);
        }
    }
    FindClose(search_handle);
    // 递归搜索
    std::strcpy(search_pattern, dir_name.c_str());
    std::strcat(search_pattern, "\\*");
    search_handle = FindFirstFileA(search_pattern, &current_file);
    if(search_handle != INVALID_HANDLE_VALUE)
    {
        if(std::strcmp(current_file.cFileName, ".") != 0 && std::strcmp(current_file.cFileName, "..") != 0)
        {
            if(current_file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                search_file_with_pattern(dir_name + "\\" + current_file.cFileName, pattern);
            }
        }
        while(FindNextFileA(search_handle, &current_file))
        {
            if(std::strcmp(current_file.cFileName, ".") == 0 || std::strcmp(current_file.cFileName, "..") == 0) continue;
            if(current_file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                search_file_with_pattern(dir_name + "\\" + current_file.cFileName, pattern);
            }
        }
    }
    FindClose(search_handle);
    delete [] search_pattern;
}

void search_file_with_regex(const std::string &dir_name, const std::regex &search_regex){
    WIN32_FIND_DATAA current_file;
    char *search_dir = new char[MAX_PATH];
    std::string file_name;
    std::smatch sm;
    std::strcpy(search_dir, dir_name.c_str());
    std::strcat(search_dir, "\\*");
    HANDLE search_handle = FindFirstFileA(search_dir, &current_file);
    if(search_handle != INVALID_HANDLE_VALUE)
    {
        if(std::strcmp(current_file.cFileName, ".") != 0 && std::strcmp(current_file.cFileName, "..") != 0)
        {
            file_name = std::string(current_file.cFileName);
            if(std::regex_search(file_name, sm, search_regex)) 
            {
                std::cout << dir_name << "\\" << sm << std::endl;
            }
            if(current_file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                search_file_with_regex(dir_name + "\\" + current_file.cFileName, search_regex);
            }
        }
        while(FindNextFileA(search_handle, &current_file))
        {
            if(std::strcmp(current_file.cFileName, ".") == 0 || std::strcmp(current_file.cFileName, "..") == 0) continue;
            file_name = std::string(current_file.cFileName);
            if(std::regex_search(file_name, sm, search_regex))
            {
                std::cout << dir_name << "\\" << sm << std::endl;
            }
            if(current_file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                search_file_with_regex(dir_name + "\\" + current_file.cFileName, search_regex);
            }
        }
    }
    FindClose(search_handle);
}

void search_file(SearchArgs &arguements){
    if(arguements.exit)
    {
        std::cout << usagetext << std::endl;
        return;
    }
    if(arguements.help)
    {
        std::cout << usagetext << std::endl;
        std::cout << helptext << std::endl;
        return;
    }
    char *dir_name = new char [MAX_PATH];
    SetCurrentDirectoryA(arguements.dir_name.c_str());
    GetCurrentDirectoryA(MAX_PATH, dir_name);
    arguements.dir_name = dir_name;
    if(arguements.dir_name == "C:\\") arguements.dir_name = "C:";
    if(arguements.ifregex)
    {
        // 由于 windows 下文件大小写随意，所以直接默认忽略大小写
        std::regex search_regex(arguements.pattern, std::regex_constants::syntax_option_type::icase);
        search_file_with_regex(arguements.dir_name, search_regex);
    }
    else
    {
        search_file_with_pattern(arguements.dir_name, arguements.pattern.c_str());
    }
    delete [] dir_name;
}

}

#endif // JSHL_WINDOWS_SEARCH_H