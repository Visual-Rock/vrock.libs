#include <iomanip>
#include <iostream>
#include <vrock/security.hpp>
#include <vrock/utils/SpanHelpers.hpp>

using namespace vrock::security;
using namespace vrock::utils;

int main( )
{
    std::vector<uint8_t> data = { 'T', 'e', 's', 't' };
    auto hash = md5( data );
    std::cout << "hash: " << to_hex_string( hash ) << std::endl
              << "ref:  0cbc6611f5540bd0809a388dc95a615b" << std::endl;
}