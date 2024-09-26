#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <stdexcept>

namespace Http {
    class HttpException : public std::runtime_error {
    public:
        explicit HttpException(int errorCode)
            : std::runtime_error("HTTP Error"), errorCode_(errorCode) {}
        int getErrorCode() const { return errorCode_; }

    private:
        int errorCode_;
    };
}

struct Boundary
{
    std::string startDelimiter;
    std::string endDelimiter;

    // Vector of parts: headers (as a map) + body
    std::vector<std::pair<std::map<std::string, std::string>, std::string> > headBody;
};

// Utility function to generate a unique filename based on the original one
std::string generateUniqueFilename(const std::string& originalFilename) {
    std::string newFilename = originalFilename;
    std::string::size_type dotPos = newFilename.find_last_of('.');

    // Get current timestamp
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", ltm);

    // Generate a random digit
    int randomDigit = rand() % 10;

    // Insert timestamp + random digit before the file extension (dot)
    if (dotPos != std::string::npos) {
        newFilename.insert(dotPos, "_" + std::string(timestamp) + std::to_string(randomDigit));
    } else {
        // If no dot, just append the timestamp and random digit at the end
        newFilename += "_" + std::string(timestamp) + std::to_string(randomDigit);
    }

    std::cout << "Filename: " << newFilename << std::endl;
    return newFilename;
}

// Function to write the body of a part to a file
void writeFileData(const std::string& filename, const std::string& body) {
    std::ofstream outfile(filename.c_str(), std::ios::out | std::ios::binary);
    if (!outfile) {
        std::cerr << "Internal Server Error no outfile" << std::endl;
        throw Http::HttpException(500);
    }

    outfile.write(body.c_str(), body.size());
    if (!outfile) {
        std::cerr << "Internal Server Error while writing" << std::endl;
        throw Http::HttpException(500);
    }

    outfile.close();
}

// Main function to handle multipart form-data
void handleMultipart(const Boundary& boundary) {
    for (std::size_t i = 0; i < boundary.headBody.size(); ++i) {
        const std::map<std::string, std::string>& headers = boundary.headBody[i].first;
        const std::string& body = boundary.headBody[i].second;

        // Check if Content-Disposition header exists
        std::map<std::string, std::string>::const_iterator it = headers.find("Content-Disposition");
        if (it == headers.end()) {
            std::cerr << "Bad Request, missing Content-Disposition" << std::endl;
            throw Http::HttpException(400);
        }

        std::string contentDisposition = it->second;
        std::string::size_type filenamePos = contentDisposition.find("filename=");

        if (filenamePos != std::string::npos) {
            // File data
            // Extract filename from Content-Disposition
            std::string::size_type start = contentDisposition.find('"', filenamePos) + 1;
            std::string::size_type end = contentDisposition.find('"', start);
            if (start == std::string::npos || end  == std::string::npos) {
                std::cerr << "Bad Request, malformed filename" << std::endl;
                throw Http::HttpException(400);
            }
            std::string filename = contentDisposition.substr(start, end - start);

            // Generate a unique filename to avoid conflicts
            std::string uniqueFilename = generateUniqueFilename(filename);

            // Write the body of the file to disk
            writeFileData(uniqueFilename, body);
        } else {
            // Form data (no filename)
            // std::map<std::string, std::string>::const_iterator nameIt = headers.find("name");
            // if (nameIt == headers.end()) {
                // std::cerr << "Bad Request, missing name for form data" << std::endl;
                // throw Http::HttpException(400);
            // }

            // Generate a file name based on the form field name
            std::string formFieldName = "new_form";
            std::string formFilename = formFieldName + ".txt";

            // Write the form data to a file
            writeFileData(formFilename, body);
        }
    }
}

int main() {
    // Example usage
    Boundary boundary;
    boundary.startDelimiter = "--boundary123";
    boundary.endDelimiter = "--boundary123--";

    // Example part (file data)
    std::map<std::string, std::string> headers1;
    headers1["Content-Disposition"] = "form-data; name=\"file1\"; filename=\"example.txt\"";
    headers1["Content-Type"] = "text/plain";
    std::string body1 = "This is the content of the file.";

    // Example part (form data)
    std::map<std::string, std::string> headers2;
    headers2["Content-Disposition"] = "form-data; name=\"field1\"";
    std::string body2 = "This is form data.";

    boundary.headBody.push_back(std::make_pair(headers1, body1));
    boundary.headBody.push_back(std::make_pair(headers2, body2));

    try {
        handleMultipart(boundary);
    } catch (const Http::HttpException& e) {
        std::cerr << "HTTP Exception: " << e.getErrorCode() << std::endl;
    }

    return 0;
}
