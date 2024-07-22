Syntax rule:
    The first word of a *line-block is a directive
    (*line-block => Sub string separated by semicolons)
    A non-escaped semicolon closes a directive
    All words following the directive-name and the closure will be arguments
    Paths are required to be absolute.

Directive list:

Fields:    [Authorized_scope][Optional/Mandatory][Directive_name][args...]
                        [? Description]

    {global} {Required} server {...}
                        ? Blocks containing a server configuration.

    {server} {Required} listen <port> <port> ...
                        ? The ports the server listens to, at least one is required.

    {server} {Required} location <uri/path> { ... }
                        ? A sub-block, it exectutes whatevers is inside if the uri matches the path. The matching is done with the longest common prefix. At least one path is required.

    {server} {Optional} server_name <Name> <Name> ...
                        ? When multiple server listen to the same port, the server names are used to select the good one through the http "Host: ..." header.

    {server} {Optional} error_page <error_code> <path/to/local/file>
                        ? Replace the default error page with the given one.

    {server} {Optional} max_body_size <size>
                        ? The maximum body size of a client request in bytes. -1 for unlimited. Default is 1MB.

    {location} {Required} root <local/path/>
                        ? The root directory of the location. The uri will be appended to this path to find the file. Must be a valid directory.

    {location} {Optional} index <file> <file> ...
                        ? The files that will be used as index if the uri is a directory. Default is [index.php, index.html].

    {location} {Optional} directory_listing on/off
                        ? Enable or disable the directory listing. Default is off.

    {location} {Optional} cgi_pass <path/to/local/cgi> <extension>
                        ? Exectutes the given cgi script with the given extension. The script must be executable. Multiple cgi_pass can be used with each different scripts and extension.

    {location} {Optional} allow_methods <method> <method> ...
                        ? The allowed methods for the location. Default is [GET, POST]. Only GET, POST and DELETE are supported.

    {location} {Optional} return <status> <uri>
                        ? Defines an HTTP redirection with the given status code and target URI.

    {location} {Optional} upload_dir <path/to/upload/directory>
                        ? Specifies the directory where uploaded files should be saved. default is /tmp. Files larger than 4MB are rejected.

    {global, server} {Optional} log_error <path/to/local/file>
                        ? The path to the error log file. Default is stderr. For Warning and Error logs.

    {global, server} {Optional} log_access <path/to/local/file>
                        ? Logs any access to the server. Default is /dev/null.

    {global, server} {Optional} log_misc <path/to/local/file>
                        ? Logs any other events. Default is /dev/null.

