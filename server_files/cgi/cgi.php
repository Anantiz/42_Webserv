<?php
header('Content-Type: text/plain');

// Output a simple message
echo "CGI Test Script\n\n";
echo "PHP SAPI: " . php_sapi_name() . "\n";

// Check and display environment variables to ensure CGI is working
echo "Environment Variables:\n";
foreach ($_SERVER as $key => $value) {
    echo "$key: $value\n";
}

// Check input method (GET, POST, etc.)
echo "\nRequest Method: " . $_SERVER['REQUEST_METHOD'] . "\n";

// If it's a POST request, display the input data
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    echo "\nPOST Data:\n";
    echo file_get_contents('php://input') . "\n";
} else {
    echo "\nGET Data:\n";
    echo $_SERVER['QUERY_STRING'] . "\n";
}
?>
