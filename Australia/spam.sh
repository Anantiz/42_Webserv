#!/bin/bash

# Number of requests
num_requests=1000

# The URL to send the requests to
url="http://localhost:6942/template.html"

# Log file to store the results
log_file="stress_test_log.txt"

send_request() {
    # Use curl with -s (silent) and -o (output to discard) options
    # -w (write-out) to extract the HTTP status code
    http_status=$(curl -s -o /dev/null -w "%{http_code}" "$url")
    if [ "$http_status" -ne 200 ]; then
        echo "Error: $http_status" >> "$log_file"
    fi
}

# Clear the log file if it exists
echo "Sending $num_requests parallel requests to $url..." > "$log_file"

# Loop to send requests in parallel
for i in $(seq 1 $num_requests); do
    send_request &
done

# Wait for all background jobs to finish
wait

echo "Sent $num_requests parallel requests to $url. Check $log_file for details."
