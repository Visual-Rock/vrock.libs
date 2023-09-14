module;

#include "tinyfiledialog/tinyfiledialogs.h"

#include <format>
#include <future>
#include <string>
#include <vector>

module vrock.ui.FileDialog;

namespace vrock::ui
{
    std::vector<std::string> split( std::string s, const std::string &delimiter )
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length( );
        std::string token;
        std::vector<std::string> res;

        while ( ( pos_end = s.find( delimiter, pos_start ) ) != std::string::npos )
        {
            token = s.substr( pos_start, pos_end - pos_start );
            pos_start = pos_end + delim_len;
            res.push_back( token );
        }

        res.push_back( s.substr( pos_start ) );
        return res;
    }

    auto open_file( const std::string &title, const std::string &default_path, std::vector<Filter> filter_patterns )
        -> std::string
    {
        const char **filter = (const char **)std::malloc( sizeof( const char * ) * filter_patterns.size( ) );
        for ( size_t i = 0; i < filter_patterns.size( ); i++ )
            filter[ i ] =
                std::format( "{0}|{1}", filter_patterns[ i ].description, filter_patterns[ i ].filter ).c_str( );

        auto ret =
            tinyfd_openFileDialog( title.c_str( ), default_path.c_str( ), (int)filter_patterns.size( ), filter, "", 0 );

        std::free( filter );
        if ( ret == nullptr )
            return "";
        return { ret };
    }

    auto open_multiple_files( const std::string &title, const std::string &default_path,
                              std::vector<Filter> filter_patterns ) -> std::vector<std::string>
    {
        const char **filter = (const char **)std::malloc( sizeof( const char * ) * filter_patterns.size( ) );
        for ( size_t i = 0; i < filter_patterns.size( ); i++ )
            filter[ i ] =
                std::format( "{0}|{1}", filter_patterns[ i ].description, filter_patterns[ i ].filter ).c_str( );

        auto ret =
            tinyfd_openFileDialog( title.c_str( ), default_path.c_str( ), (int)filter_patterns.size( ), filter, "", 1 );

        std::free( filter );
        if ( ret == nullptr )
            return { };
        return split( std::string( ret ), "|" );
    }

    auto select_folder( const std::string &title, const std::string &default_path ) -> std::string
    {
        auto res = tinyfd_selectFolderDialog( title.c_str( ), default_path.c_str( ) );
        if ( res == nullptr )
            return "";
        return { res };
    }

    auto save_file( const std::string &title, const std::string &default_path, std::vector<Filter> filter_patterns )
        -> std::string
    {
        const char **filter = (const char **)std::malloc( sizeof( const char * ) * filter_patterns.size( ) );
        for ( size_t i = 0; i < filter_patterns.size( ); i++ )
            filter[ i ] =
                std::format( "{0}|{1}", filter_patterns[ i ].description, filter_patterns[ i ].filter ).c_str( );

        auto ret =
            tinyfd_saveFileDialog( title.c_str( ), default_path.c_str( ), (int)filter_patterns.size( ), filter, "" );

        std::free( filter );
        if ( ret == nullptr )
            return "";
        return { ret };
    }

    auto open_file_async( const std::string &title, const std::string &default_path,
                          const std::vector<Filter> &filter_patterns ) -> std::future<std::string>
    {
        return std::async( &open_file, title, default_path, filter_patterns );
    }

    auto open_multiple_files_async( const std::string &title, const std::string &default_path,
                                    const std::vector<Filter> &filter_patterns )
        -> std::future<std::vector<std::string>>
    {
        return std::async( &open_multiple_files, title, default_path, filter_patterns );
    }

    auto select_folder_async( const std::string &title, const std::string &default_path ) -> std::future<std::string>
    {
        return std::async( &select_folder, title, default_path );
    }

    auto save_file_async( const std::string &title, const std::string &default_path,
                          const std::vector<Filter> &filter_patterns ) -> std::future<std::string>
    {
        return std::async( &save_file, title, default_path, filter_patterns );
    }
} // namespace vrock::ui