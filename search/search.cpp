#include "search.h"

using namespace jshl;

int main(int argc, char const *argv[])
{
    SearchArgs arguements = search_parse_args(argc, argv);
    search_file(arguements);
    return 0;
}
