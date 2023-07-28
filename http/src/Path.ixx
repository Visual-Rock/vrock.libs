module;

#include <regex>
#include <string>
#include <unordered_map>
#include <utility>

export module vrock.http.Path;

namespace vrock::http
{
    export class Parameters
    {
    public:
        std::unordered_map<std::string, std::string> params;
    };

    export class BaseMatcher
    {
    public:
        virtual ~BaseMatcher( ) = default;

        virtual auto match( const std::string & ) -> bool
        {
            return true;
        }

        virtual auto get_parameters( Parameters &, const std::string & ) -> void
        {
        }
    };

    export class SegmentMatcher : public BaseMatcher
    {
    public:
        explicit SegmentMatcher( std::string pattern ) : pattern( std::move( pattern ) )
        {
        }

        auto match( const std::string &str ) -> bool override
        {
            return str == pattern;
        }

        auto get_parameters( Parameters &, const std::string & ) -> void override
        {
        }

    private:
        const std::string pattern;
    };

    export class VariableMatcher : public BaseMatcher
    {
    public:
        explicit VariableMatcher( std::string name ) : name( std::move( name ) )
        {
        }

        VariableMatcher( ) = delete;

        auto match( const std::string &str ) -> bool override
        {
            return !str.empty( );
        }

        auto get_parameters( Parameters &params, const std::string &str ) -> void override
        {
            if ( !name.empty( ) )
                params.params[ name ] = str;
        }

    private:
        const std::string name;
    };

    export class RegexMatcher : public BaseMatcher
    {
    public:
        explicit RegexMatcher( std::string name, const std::string &regex ) : name( std::move( name ) ), regex( regex )
        {
        }

        RegexMatcher( ) = delete;

        auto match( const std::string &str ) -> bool override
        {
            return std::regex_match( str, regex );
        }

        auto get_parameters( Parameters &params, const std::string &str ) -> void override
        {
            if ( !name.empty( ) )
                params.params[ name ] = str;
        }

    private:
        const std::string name;
        const std::regex regex;
    };

    export auto to_matcher( const std::string &segment ) -> std::shared_ptr<BaseMatcher>
    {
        if ( segment.empty( ) )
            return std::make_shared<BaseMatcher>( );
        if ( segment[ 0 ] == '{' && segment[ segment.length( ) - 1 ] == '}' )
        {
            std::string trimmed_segment( segment.substr( 1, segment.length( ) - 2 ) );
            size_t colon_index = trimmed_segment.find( ':' );
            if ( colon_index == std::string::npos )
                return std::make_shared<VariableMatcher>( trimmed_segment );
            return std::make_shared<RegexMatcher>( trimmed_segment.substr( 0, colon_index ),
                                                   trimmed_segment.substr( colon_index + 1 ) );
        }
        return std::make_shared<SegmentMatcher>( segment );
    }

    auto split( const std::string &s, char delim ) -> std::vector<std::string>
    {
        std::vector<std::string> result;
        std::stringstream ss( s );
        std::string item;

        while ( getline( ss, item, delim ) )
        {
            result.push_back( item );
        }

        return result;
    }

    export auto split_path( const std::string &path ) -> std::vector<std::string>
    {
        auto res = split( path, '/' );
        if ( path[ path.size( ) - 1 ] == '/' )
            res.emplace_back( "" );
        return res;
    }

    export auto compile_path( const std::string &path ) -> std::vector<std::shared_ptr<BaseMatcher>>
    {
        auto segments = split_path( path );
        if ( segments.empty( ) )
            return { to_matcher( "" ) };
        std::vector<std::shared_ptr<BaseMatcher>> seg;
        seg.reserve( segments.size( ) );
        for ( const auto &segment : segments )
            seg.push_back( to_matcher( segment ) );
        return seg;
    }
} // namespace vrock::http