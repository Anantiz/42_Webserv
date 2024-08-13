Syntax rule:
    The first word of a *line-block is a directive
    (*line-block => Sub string separated by semicolons)
    A non-escaped semicolon closes a directive
    All words following the directive-name and the closure will be arguments
    Paths are required to be absolute.

    Comments: Any unescaped '#' will be considered the start of comment, the comment ends at the first semicolon or newline.

Directive list:

Fields: [Authorized_scope][Optional/Mandatory][Directive_name][args...]
                        [? Description]

TO_DO    {global} {Required} server {...}
                        ? Blocks containing a server configuration.

TO_DO    {server} {Required} listen <port> <port> ...
                        ? The ports the server listens to, at least one is required.

TO_DO    {server} {Required} location <uri/path> { ... }
                        ? A sub-block, it exectutes whatevers is inside if the uri matches the path. The matching is done with the longest common prefix. At least one path is required.

TO_DO    {server} {Optional} server_name <Name> <Name> ...
                        ? When multiple server listen to the same port, the server names are used to select the good one through the http "Host: ..." header.

TO_DO    {server} {Optional} error_page <error_code> <path/to/local/file>
                        ? Replace the default error page with the given one.

TO_DO    {server} {Optional} max_body_size <size>
                        ? The maximum body size of a client request in bytes. -1 for unlimited. Default is 4089.

TO_DO    {location} {Required} root <local/path/>
                        ? The root directory of the location. The uri will be appended to this path to find the file. Must be a valid directory.

TO_DO    {location} {Optional} index <file> <file> ...
                        ? The files that will be used as index if the uri is a directory. Default is [index.php, index.html].

TO_DO    {location} {Optional} directory_listing on/off
                        ? Enable or disable the directory listing. Default is off.

TO_DO    {location} {Optional} cgi_pass <path/to/local/cgi> <extension>
                        ? Exectutes the given cgi script with the given extension. The script must be executable. Multiple cgi_pass can be used with each different scripts and extension.

TO_DO    {location} {Optional} allow_methods <method> <method> ...
                        ? The allowed methods for the location. Default is [GET, POST]. Only GET, POST and DELETE are supported.

TO_DO    {location} {Optional} return <status> <uri>
                        ? Defines an HTTP redirection with the given status code and target URI.

TO_DO    {location} {Optional} upload_dir <path/to/upload/directory>
                        ? Specifies the directory where uploaded files should be saved. default is /tmp. Files larger than max_body_size are rejected.

TO_DO    {global, server} {Optional} log_error <path/to/local/file>
                        ? The path to the error log file. Default is stderr. For Warning and Error logs.

TO_DO    {global, server} {Optional} log_access <path/to/local/file>
                        ? Logs any access to the server. Default is /dev/null.

TO_DO    {global, server} {Optional} log_misc <path/to/local/file>
                        ? Logs any other events. Default is /dev/null.

1 - Global Configuration (global)

    - log_error <path> (Optional): Spécifie le fichier où les erreurs (warnings et erreurs) doivent être enregistrées. Par défaut, c'est stderr.

    - log_access <path> (Optional): Enregistre tous les accès au serveur. Par défaut, c'est /dev/null, ce qui signifie que les accès ne sont pas enregistrés.

    - log_misc <path> (Optional): Enregistre tous les autres événements (hors erreurs et accès). Par défaut, c'est /dev/null.

2 - Server Block Configuration (server)

    - listen <port> <port> ... (Required): Spécifie les ports sur lesquels le serveur doit écouter. Au moins un port doit être spécifié.

    - location <uri/path> { ... } (Required): Définit un bloc location qui correspond à une URI ou un chemin spécifique. Le serveur utilisera ce bloc pour traiter les requêtes dont l'URI correspond au chemin spécifié.

    - server_name <Name> <Name> ... (Optional): Définit des noms de serveur (virtual hosts). Utile lorsque plusieurs serveurs écoutent sur le même port pour distinguer quel serveur doit gérer la requête en fonction du nom d'hôte dans l'en-tête HTTP "Host".

    - error_page <error_code> <path> (Optional): Spécifie une page d'erreur personnalisée pour un code d'erreur spécifique (ex: 404, 500). Remplace la page d'erreur par défaut.

    - max_body_size <size> (Optional): Limite la taille du corps des requêtes HTTP. Par défaut, c'est 4089 octets, et -1 signifie illimité.

3- Location Block Configuration (location)

    - root <local/path> (Required): Spécifie le répertoire racine pour cette location. Le chemin URI de la requête est combiné avec ce répertoire pour trouver le fichier.

    - index <file> <file> ... (Optional): Définit les fichiers d'index qui seront utilisés si l'URI est un répertoire. Par défaut, c'est [index.php, index.html].

    - directory_listing on/off (Optional): Active ou désactive l'affichage de la liste des fichiers dans un répertoire si l'URI correspond à un répertoire sans fichier d'index.

    - cgi_pass <path> <extension> (Optional): Associe un script CGI avec une extension spécifique de fichier (ex: .php). Le script est exécuté lorsque le fichier avec l'extension correspondante est demandé.

    - allow_methods <method> <method> ... (Optional): Spécifie les méthodes HTTP autorisées pour cette location. Par défaut, GET et POST sont autorisés.

    - return <status> <uri> (Optional): Définit une redirection HTTP pour cette location, avec le statut et l'URI cible spécifiés.

    - upload_dir <path> (Optional): Spécifie le répertoire où les fichiers téléchargés doivent être sauvegardés. Par défaut, c'est /tmp. Les fichiers dépassant la max_body_size sont rejetés.