#include <iostream>
#include "lib3dmm.h"

__declspec(dllexport)
void
hello(void)
{
    std::cout << "hello" << std::endl;
}

