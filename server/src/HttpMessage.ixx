module;

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>

export module vrock.server.HttpMessage;

namespace vrock::server
{
    export class HttpMessage
    {
    public:
        HttpVersion version = HttpVersion::HTTP_1_1;
        std::unordered_map<std::string, std::string> headers;
        std::string body;
    };

    export class HttpRequest : public HttpMessage
    {
    public:
        HttpMethod method;
        std::string uri;
    };

    export class HttpResponse : public HttpMessage
    {
    public:
        HttpStatusCode status_code;
    };
} // namespace vrock::server
