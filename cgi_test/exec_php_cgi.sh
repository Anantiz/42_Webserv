#!/bin/sh

export GATEWAY_INTERFACE=CGI/1.1
export REDIRECT_STATUS=200
export SCRIPT_FILENAME=test.php
export SCRIPT_NAME=test.php
export SCRIPT_URI="/test.php"
export SERVER_NAME="bear.com"
export SERVER_PROTOCOL="HTTP/1.1"
export PATH_INFO=$PWD
export REQUEST_METHOD=POST
export QUERY_STRING="t=1"
# export CONTENT_TYPE="text/plain"
export CONTENT_TYPE="application/x-www-form-urlencoded"
# export CONTENT_TYPE="application/html"
# export CONTENT_LENGTH=`stat -f%z $SCRIPT_FILENAME`
export CONTENT_LENGTH=3
export REMOTE_HOST="127.0.0.1"

echo "t=1" | php-cgi