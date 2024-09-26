#include "location.hpp"

// Utility function to generate a unique filename based on the original one
std::string generateUniqueFilename(const std::string& originalFilename) {
    std::string newFilename = originalFilename;
    std::string::size_type dotPos = newFilename.find_last_of('.');

    int randomDigit = rand() % 10;

    // Get current timestamp
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", ltm);

    // Insert timestamp + random digit before the file extension (dot)
    if (dotPos != std::string::npos) {
        newFilename.insert(dotPos, "_" + std::string(timestamp) + utils::anything_to_str(randomDigit));
    } else {
        // If no dot, just append the timestamp and random digit at the end
        newFilename += "_" + std::string(timestamp) + utils::anything_to_str(randomDigit);
    }
    logs::SdevLog("Generated unique filename: " + newFilename);
    return newFilename;
}

// Function to write the body of a part to a file
void writeFileData(const std::string& filename, const std::string& body) {
    logs::SdevLog("\033[92mWriting\033[0m file data to: " + filename);
    std::ofstream outfile(filename.c_str(), std::ios::out | std::ios::binary);
    if (!outfile) {
        logs::SdevLog("Coulnd't use file: " + filename);
        throw Http::HttpException(500);
    }
    outfile.write(body.c_str(), body.size());
    if (!outfile) {
        logs::SdevLog("Coulnd't use file: " + filename);
        throw Http::HttpException(500);
    }
    outfile.close();
}

// Main function to handle multipart form-data
void handleMultipart(Client &client, const std::string & _upload_dir)
{
    logs::SdevLog("Multi-part size: " + utils::anything_to_str(client.request_boundary.size()));
    int part_count = 0;
    for (std::size_t i = 0; i < client.request_boundary.size(); ++i) {
        const std::map<std::string, std::string>    &headers = client.request_boundary[i].mainHeader;
        const std::string                           &body = client.request_boundary[i].body;

        logs::SdevLog("Part " + utils::anything_to_str(part_count++) + " body-size= " \
        + utils::anything_to_str(body.size()) + " Headers count=" + utils::anything_to_str(headers.size()));
        // Log headers:

        for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end();it++ ) {
            logs::SdevLog("Headers: " + it->first + "=" + it->second);
        }

        // Check if Content-Disposition header exists
        std::map<std::string, std::string>::const_iterator it = headers.find("Content-Disposition");
        if (it == headers.end()) {
            logs::SdevLog("Bad Request, missing Content-Disposition");
            throw Http::HttpException(400);
        }

        std::string contentDisposition = it->second;
        std::string::size_type filenamePos = contentDisposition.find("filename=");
        std::string uniqueFilename;

        if (filenamePos != std::string::npos) {
            // File data
            // Extract filename from Content-Disposition
            std::string::size_type start = contentDisposition.find('"', filenamePos) + 1;
            std::string::size_type end = contentDisposition.find('"', start);
            if (start == std::string::npos || end == std::string::npos) {
                logs::SdevLog("Bad Request, malformed filename");
                throw Http::HttpException(400);
            }
            uniqueFilename = generateUniqueFilename(contentDisposition.substr(start, end - start));
        } else {
            uniqueFilename = generateUniqueFilename("UnamedForm") + ".txt";

            // Write the form data to a file
        }
        writeFileData(_upload_dir + uniqueFilename, body);
    }
}