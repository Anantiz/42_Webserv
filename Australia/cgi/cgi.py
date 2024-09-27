import os
import http.cookies

# Function to print headers and HTM, end="\r\n", flush=TrueL
def print_html(cookie_value=None):
    # Print content type header and set cookie if needed
    print("Content-Type: text/html", end="\r\n", flush=True)
    if cookie_value:
        print(f"Set-Cookie: user_id={cookie_value}; Path=/; HttpOnly", end="\r\n", flush=True)

    print("",end="\r\n")  # End of headers (must have a blank line here to separate headers from body, end="\r\n", flush=True)

    # HTML content begins
    print("<html><head><title>CGI Cookie Example</title></head>", flush=True)
    print("<body>", flush=True)
    if cookie_value:
        print(f"<p>Cookie 'user_id' is set to: {cookie_value}</p>", flush=True)
    else:
        print("<p>No cookie found. Setting a new cookie!</p>", flush=True)
    print("</body></html>", end="\r\n", flush=True)

# Read existing cookies
cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))
user_id = cookie.get("user_id")

# If there's no cookie, set a new one
if user_id is None:
    user_id = "12345"  # Example of a new user ID
    print_html(cookie_value=user_id)
else:
    # If cookie exists, display its value
    print_html(cookie_value=user_id.value)
exit(0)
