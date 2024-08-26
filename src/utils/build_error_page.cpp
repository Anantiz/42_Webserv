#include "utils.hpp"

const std::string utils::build_error_page(int error)
{
    // Don't talkk to me about this, it works
    static const std::string str_1 = " <!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n    <title>Error ";
    static const std::string str_3 = "</title>\n<style>\n        body {\n            font-family: Arial, sans-serif;\n            background-color: #f8f8f8;\n            color: #333;\n            display: flex;\n            justify-content: center;\n            align-items: center;\n            height: 100vh;\n            margin: 0;\n        }\n        .container {\n            text-align: center;\n        }\n        h1 {\n            font-size: 6em;\n            margin: 0;\n        }\n        p {\n            font-size: 1.5em;\n        }\n    </style>\n</head>\n<body>\n    <div class=\"container\">\n        <h1>";
    static const std::string str_5 = "</h1>\n        \n    </div>\n</body>\n</html>";
    static const std::string titles[] = {
        "400 - Bad Request",
        "401 - Unauthorized",
        "402 - Payment Required",
        "403 - Forbidden",
        "404 - Not Found",
        "405 - Method Not Allowed",
        "406 - Not Acceptable",
        "407 - Proxy Authentication Required",
        "408 - Request Timeout",
        "409 - Conflict",
        "410 - Gone",
        "411 - Length Required",
        "412 - Precondition Failed",
        "413 - Payload Too Large",
        "414 - URI Too Long",
        "415 - Unsupported Media Type",
        "416 - Range Not Satisfiable",
        "417 - Expectation Failed",
        "418 - I'm a teapot",
        "", // 19
        "", // 20
        "421 - Misdirected Request",
        "422 - Unprocessable Entity",
        "423 - Locked",
        "424 - Failed Dependency",
        "425 - Too Early",
        "426 - Upgrade Required",
        "428 - Precondition Required",
        "429 - Too Many Requests",
        "", // 30
        "431 - Request Header Fields Too Large",
        "451 - Unavailable For Legal Reasons",
        "500 - Internal Server Error",
        "501 - Not Implemented",
        "502 - Bad Gateway",
        "503 - Service Unavailable",
        "504 - Gateway Timeout",
        "505 - HTTP Version Not Supported",
        "506 - Variant Also Negotiates",
        "507 - Insufficient Storage",
        "508 - Loop Detected",
        "",
        "510 - Not Extended",
        "511 - Network Authentication Required"
    };
    // Enginering marvel
    int str_2_index = 0;
    if (error > 511 || error < 400)
        error = 500;
    if (error >= 500)
        str_2_index = error - 500 + (44 - 12);
    else if (error == 451)
        str_2_index = 32;
    else if (error >= 400)
        str_2_index = error - 400;
    else
        str_2_index = 44 - 12; // error 500

    const std::string str_2 = titles[str_2_index];
    return str_1 + str_2 + str_3 + str_2 + str_5;
}