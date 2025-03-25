#include "visualizer.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

void visualizeBase64Image(const std::string& encodedString) {
    static const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::vector<uchar> decodedData;

    // Check if input length is valid
    if (encodedString.empty() || encodedString.size() % 4 != 0) {
        std::cerr << "Invalid Base64 encoded string length!" << std::endl;
        return;
    }

    // Create a lookup table for decoding
    std::unordered_map<char, int> base64_index;
    for (int i = 0; i < 64; ++i) {
        base64_index[base64_chars[i]] = i;
    }

    size_t i = 0;
    while (i < encodedString.size()) {
        uint32_t buffer = 0;
        int bitsCollected = 0;
        int paddingCount = 0;

        for (int j = 0; j < 4; ++j) {
            char c = encodedString[i++];
            if (c == '=') {
                buffer <<= 6;
                paddingCount++;
            }
            else if (base64_index.find(c) != base64_index.end()) {
                buffer = (buffer << 6) | base64_index[c];
            }
            else {
                std::cerr << "Invalid Base64 character: " << c << std::endl;
                return;
            }
        }

        decodedData.push_back((buffer >> 16) & 0xFF);
        if (paddingCount < 2) decodedData.push_back((buffer >> 8) & 0xFF);
        if (paddingCount < 1) decodedData.push_back(buffer & 0xFF);
    }

    // Create image from decoded data
    cv::Mat img = cv::imdecode(decodedData, cv::IMREAD_COLOR);

    if (!img.empty()) {
        // Show the image in an OpenCV window
        cv::imshow("Fluid Simulation", img);
    }
    else {
        std::cerr << "Failed to decode or display the image!" << std::endl;
    }
}
