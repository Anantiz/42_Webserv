#!bin/bash

echo -e "GET / HTTP/1.1\r\nHost: example.com\r\nCustom-Header: Value\r\nConnection: close\r\n\r\n" | nc example.com 80
