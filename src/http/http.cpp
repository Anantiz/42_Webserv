#include "http.hpp"
#include <map>

/*
if you try to read/recv or write/send in any file descriptor
without using poll() (or equivalent), your grade will be 0
*/

const std::string   &Http::get_status_string(int status)
{
    static std::map<int, std::string> status_strings;
    static bool initialized = false;

    if (!initialized)
    {
        status_strings[400] = "400 Bad Request";
        status_strings[401] = "401 Unauthorized";
        status_strings[402] = "402 Payment Required";
        status_strings[403] = "403 Forbidden";
        status_strings[404] = "404 Not Found";
        status_strings[405] = "405 Method Not Allowed";
        status_strings[406] = "406 Not Acceptable";
        status_strings[407] = "407 Proxy Authentication Required";
        status_strings[408] = "408 Request Timeout";
        status_strings[409] = "409 Conflict";
        status_strings[410] = "410 Gone";
        status_strings[411] = "411 Length Required";
        status_strings[412] = "412 Precondition Failed";
        status_strings[413] = "413 Payload Too Large";
        status_strings[414] = "414 URI Too Long";
        status_strings[415] = "415 Unsupported Media Type";
        status_strings[416] = "416 Range Not Satisfiable";
        status_strings[417] = "417 Expectation Failed";
        status_strings[418] = "418 I'm a teapot";
        status_strings[421] = "421 Misdirected Request";
        status_strings[422] = "422 Unprocessable Entity";
        status_strings[423] = "423 Locked";
        status_strings[424] = "424 Failed Dependency";
        status_strings[425] = "425 Too Early";
        status_strings[426] = "426 Upgrade Required";
        status_strings[428] = "428 Precondition Required";
        status_strings[429] = "429 Too Many Requests";
        status_strings[431] = "431 Request Header Fields Too Large";
        status_strings[451] = "451 Unavailable For Legal Reasons";
        status_strings[500] = "500 Internal Server Error";
        status_strings[501] = "501 Not Implemented";
        status_strings[502] = "502 Bad Gateway";
        status_strings[503] = "503 Service Unavailable";
        status_strings[504] = "504 Gateway Timeout";
        status_strings[505] = "505 HTTP Version Not Supported";
        status_strings[506] = "506 Variant Also Negotiates";
        status_strings[507] = "507 Insufficient Storage";
        status_strings[508] = "508 Loop Detected";
        status_strings[510] = "510 Not Extended";
        status_strings[511] = "511 Network Authentication Required";
        initialized = true;
    }
    std::map<int, std::string>::iterator it = status_strings.find(status);
    if (it != status_strings.end())
        return it->second;
    else
        return status_strings[500];
}