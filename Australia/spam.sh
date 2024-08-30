#!/bin/bash

# Number of requests
num_requests=10

# The URL to send the requests to
url="http://localhost:6942"

# Log file to store the results
log_file="stress_test_log.txt"

# Function to send a GET request and check the response
send_request() {
    status_code=$(wget -nc "$url" && rm index.html)
    if [ "$status_code" -eq 200 ]; then
        echo "Request succeeded with status code: $status_code" >> "$log_file"
    else
        echo "Request failed with status code: $status_code" >> "$log_file"
    fi
}

# Clear the log file if it exists
> "$log_file"

# Loop to send requests in parallel
for i in $(seq 1 $num_requests); do
    send_request &
done

# Wait for all background jobs to finish
wait

echo "Sent $num_requests parallel requests to $url. Check $log_file for details."
