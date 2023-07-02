import vrock.utils.ByteArray;

#include <iostream>

int main()
{
    vrock::utils::ByteArray data("alksd");
    std::cout << data.to_string() << std::endl;
    return 0;
}