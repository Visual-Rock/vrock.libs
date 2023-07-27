module;

#include <string>
#include <utility>

export module vrock.http.Url;

namespace vrock::http
{
    export class Url
    {
    public:
        Url( ) = default;

        explicit Url( std::string u ) : url( std::move( u ) )
        {
        }

        std::string url;
    };
} // namespace vrock::http