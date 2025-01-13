#pragma once

#include <exception>
#include <string>
#include <spdlog/spdlog.h>

namespace arangocpp {

/// Default logger for arango-cpp library
extern std::shared_ptr<spdlog::logger> arango_logger;

/// @brief General exception structure into project.
class arango_exception: public std::exception
{

public:

    /// Returns the header string.
    const std::string& header() const
    {
        return excp_header;
    }

    /// Returns the explanatory string.
    const char* what() const noexcept override
    {
        return excp_message.c_str();
    }

    /// The id of the exception.
    const int id;

    ///  @brief Create any error exception.
    ///
    ///  @param atitle    the title of the exception
    ///  @param aid       the id of the exception
    ///  @param what_arg  the explanatory string
    ///  @return arango_exception object.
    explicit arango_exception( const std::string& atitle, int aid, std::string&& amessage ):
        id(aid), excp_header(getheader(atitle, aid)), excp_message( std::forward<std::string>(amessage) )
    {}

    static std::string getheader(const std::string& atitle, int aid)
    {
        return "arango:" + atitle + ":" + std::to_string(aid);
    }

private:

    /// An exception header.
    std::string excp_header;
    /// An exception messages.
    std::string excp_message;

};


/// Throw  jarango_exception.
[[ noreturn ]] inline void ARANGO_THROW( const std::string& title, int id, std::string&& message )
{
    arango_logger->error("{} {}", arango_exception::getheader(title, id), message);
    throw arango_exception{title, id, std::forward<std::string>(message)};
}

/// Throw by condition jsonio_exception.
inline void ARANGO_THROW_IF(bool error, const std::string& title, int id, std::string&& message )
{
    if(error) {
        arango_logger->error("{} {}", arango_exception::getheader(title, id), message);
        throw arango_exception{title, id, std::forward<std::string>(message)};
    }
}

} // namespace arangocpp
