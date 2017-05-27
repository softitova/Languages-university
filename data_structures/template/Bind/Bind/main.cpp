#include <iostream>
#include "bind.h"
int main()
{
    std::cout << (bind(&sum, 0, -5, 5))() << std::endl;
    std::cout << (bind(&sum, bind(&sum, _2, 10, _1), _1, _1))(6, -15) << std::endl;
    return 0;
}

