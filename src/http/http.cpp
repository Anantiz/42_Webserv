#include "http.hpp"
#include <map>

/*
Returns the stattus string for the given status code
as : "HTTP/1.1 XXX message\r\n"
*/
const std::string   &Http::get_status_string(int status)
{
    static std::map<int, std::string> status_strings;
    static bool initialized = false;

    if (!initialized)
    {
        status_strings[200] = "HTTP/1.1 200 OK\r\n";
        status_strings[201] = "HTTP/1.1 201 Created\r\n";
        status_strings[202] = "HTTP/1.1 202 Accepted\r\n";
        status_strings[203] = "HTTP/1.1 203 Non-Authoritative Information\r\n";
        status_strings[204] = "HTTP/1.1 204 No Content\r\n";
        status_strings[205] = "HTTP/1.1 205 Reset Content\r\n";
        status_strings[206] = "HTTP/1.1 206 Partial Content\r\n";
        status_strings[300] = "HTTP/1.1 300 Multiple Choices\r\n";
        status_strings[301] = "HTTP/1.1 301 Moved Permanently\r\n";
        status_strings[302] = "HTTP/1.1 302 Found\r\n";
        status_strings[303] = "HTTP/1.1 303 See Other\r\n";
        status_strings[304] = "HTTP/1.1 304 Not Modified\r\n";
        status_strings[305] = "HTTP/1.1 305 Use Proxy\r\n";
        status_strings[307] = "HTTP/1.1 307 Temporary Redirect\r\n";
        status_strings[308] = "HTTP/1.1 308 Permanent Redirect\r\n";
        status_strings[400] = "HTTP/1.1 400 Bad Request\r\n";
        status_strings[401] = "HTTP/1.1 401 Unauthorized\r\n";
        status_strings[402] = "HTTP/1.1 402 Payment Required\r\n";
        status_strings[403] = "HTTP/1.1 403 Forbidden\r\n";
        status_strings[404] = "HTTP/1.1 404 Not Found\r\n";
        status_strings[405] = "HTTP/1.1 405 Method Not Allowed\r\n";
        status_strings[406] = "HTTP/1.1 406 Not Acceptable\r\n";
        status_strings[407] = "HTTP/1.1 407 Proxy Authentication Required\r\n";
        status_strings[408] = "HTTP/1.1 408 Request Timeout\r\n";
        status_strings[409] = "HTTP/1.1 409 Conflict\r\n";
        status_strings[410] = "HTTP/1.1 410 Gone\r\n";
        status_strings[411] = "HTTP/1.1 411 Length Required\r\n";
        status_strings[412] = "HTTP/1.1 412 Precondition Failed\r\n";
        status_strings[413] = "HTTP/1.1 413 Payload Too Large\r\n";
        status_strings[414] = "HTTP/1.1 414 URI Too Long\r\n";
        status_strings[415] = "HTTP/1.1 415 Unsupported Media Type\r\n";
        status_strings[416] = "HTTP/1.1 416 Range Not Satisfiable\r\n";
        status_strings[417] = "HTTP/1.1 417 Expectation Failed\r\n";
        status_strings[418] = "HTTP/1.1 418 I'm a teapot\r\n";
        status_strings[421] = "HTTP/1.1 421 Misdirected Request\r\n";
        status_strings[422] = "HTTP/1.1 422 Unprocessable Entity\r\n";
        status_strings[423] = "HTTP/1.1 423 Locked\r\n";
        status_strings[424] = "HTTP/1.1 424 Failed Dependency\r\n";
        status_strings[425] = "HTTP/1.1 425 Too Early\r\n";
        status_strings[426] = "HTTP/1.1 426 Upgrade Required\r\n";
        status_strings[428] = "HTTP/1.1 428 Precondition Required\r\n";
        status_strings[429] = "HTTP/1.1 429 Too Many Requests\r\n";
        status_strings[431] = "HTTP/1.1 431 Request Header Fields Too Large\r\n";
        status_strings[451] = "HTTP/1.1 451 Unavailable For Legal Reasons\r\n";
        status_strings[500] = "HTTP/1.1 500 Internal Server Error\r\n";
        status_strings[501] = "HTTP/1.1 501 Not Implemented\r\n";
        status_strings[502] = "HTTP/1.1 502 Bad Gateway\r\n";
        status_strings[503] = "HTTP/1.1 503 Service Unavailable\r\n";
        status_strings[504] = "HTTP/1.1 504 Gateway Timeout\r\n";
        status_strings[505] = "HTTP/1.1 505 HTTP Version Not Supported\r\n";
        status_strings[506] = "HTTP/1.1 506 Variant Also Negotiates\r\n";
        status_strings[507] = "HTTP/1.1 507 Insufficient Storage\r\n";
        status_strings[508] = "HTTP/1.1 508 Loop Detected\r\n";
        status_strings[510] = "HTTP/1.1 510 Not Extended\r\n";
        status_strings[511] = "HTTP/1.1 511 Network Authentication Required\r\n";
        initialized = true;
    }
    std::map<int, std::string>::iterator it = status_strings.find(status);
    if (it != status_strings.end())
        return it->second;
    else
        return status_strings[500];
}