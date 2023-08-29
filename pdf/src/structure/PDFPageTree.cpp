module;

import vrock.pdf.PDFBaseObjects;
import vrock.utils.List;

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

module vrock.pdf.PDFPageTree;

namespace vrock::pdf
{
    PageTreeNode::PageTreeNode( std::shared_ptr<PDFDictionary> dict, std::shared_ptr<PDFContext> context,
                                PageTreeNode *parent )
        : PageBaseObject( std::move( dict ), parent, std::move( context ), false )
    {
    }
} // namespace vrock::pdf