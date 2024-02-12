#pragma once

#include "vrock/pdf/parser/ContentStreamParser.hpp"

using namespace vrock::pdf;

inline std::pair<std::shared_ptr<ResourceDictionary>, std::shared_ptr<PDFContext>> create_res_dict( )
{
    auto ctx = std::make_shared<PDFContext>( std::make_shared<PDFObjectParser>( ) );
    auto dict = std::make_shared<PDFDictionary>( ctx );
    auto res = std::make_shared<ResourceDictionary>( dict, ctx );
    return { res, ctx };
}

inline std::shared_ptr<Font> create_font(std::shared_ptr<PDFContext> ctx)
{
    auto dict = std::make_shared<PDFDictionary>( ctx );
    return std::make_shared<Font>( dict );
}