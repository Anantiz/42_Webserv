import os

# List of standard HTTP error codes and their messages
http_errors = {
    400: "Bad Request",
    401: "Unauthorized",
    402: "Payment Required",
    403: "Forbidden",
    404: "Not Found",
    405: "Method Not Allowed",
    406: "Not Acceptable",
    407: "Proxy Authentication Required",
    408: "Request Timeout",
    409: "Conflict",
    410: "Gone",
    411: "Length Required",
    412: "Precondition Failed",
    413: "Payload Too Large",
    414: "URI Too Long",
    415: "Unsupported Media Type",
    416: "Range Not Satisfiable",
    417: "Expectation Failed",
    418: "I'm a teapot",
    421: "Misdirected Request",
    422: "Unprocessable Entity",
    423: "Locked",
    424: "Failed Dependency",
    425: "Too Early",
    426: "Upgrade Required",
    428: "Precondition Required",
    429: "Too Many Requests",
    431: "Request Header Fields Too Large",
    451: "Unavailable For Legal Reasons",
    500: "Internal Server Error",
    501: "Not Implemented",
    502: "Bad Gateway",
    503: "Service Unavailable",
    504: "Gateway Timeout",
    505: "HTTP Version Not Supported",
    506: "Variant Also Negotiates",
    507: "Insufficient Storage",
    508: "Loop Detected",
    510: "Not Extended",
    511: "Network Authentication Required"
}

# Directory to save error pages
output_dir = "./"
os.makedirs(output_dir, exist_ok=True)

# HTML template for error pages
html_template = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Error {code} - {message}</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            background-color: #f8f8f8;
            color: #333;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }}
        .container {{
            text-align: center;
        }}
        h1 {{
            font-size: 6em;
            margin: 0;
        }}
        p {{
            font-size: 1.5em;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>{code}</h1>
        <p>{message}</p>
    </div>
</body>
</html>
"""

# Create an HTML file for each error code
for code, message in http_errors.items():
    filename = os.path.join(output_dir, f"{code}.html")
    with open(filename, "w") as file:
        file.write(html_template.format(code=code, message=message))

print(f"HTML error pages created in '{output_dir}' directory.")
