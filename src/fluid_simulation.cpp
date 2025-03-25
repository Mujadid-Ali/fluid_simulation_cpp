#include "fluid_simulation.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

FluidSimulation::FluidSimulation(int width, int height, float viscosity, float forceStrength) : width(width), height(height), viscosity(viscosity), forceStrength(forceStrength) {
    canvas = cv::Mat::zeros(height, width, CV_8UC3);
    velocityX = cv::Mat::zeros(height, width, CV_32F);
    velocityY = cv::Mat::zeros(height, width, CV_32F);
    density = cv::Mat(height, width, CV_32FC3, cv::Scalar(1.0, 1.0, 1.0)); // Ensure proper initialization with 3 channels
    pressure = cv::Mat::zeros(height, width, CV_32F);
}

void FluidSimulation::updateFluid(int mouseX, int mouseY, const std::vector<cv::Scalar>& colors) {
    float dt = 1.0f / 60.0f;  // assuming 60 FPS

    // Apply external force from the mouse position
    applyForce(mouseX, mouseY, colors);

    // Diffuse the velocity and density fields
    diffuse(dt);

    // Advect the velocity and density based on their respective fields
    advect(dt);

    // Render the current state of the simulation
    render(canvas, colors);
}

void FluidSimulation::diffuse(float dt) {
    cv::Mat velocityX_copy, velocityY_copy;
    velocityX.copyTo(velocityX_copy);
    velocityY.copyTo(velocityY_copy);

    // Simple viscosity diffusion
    velocityX = velocityX_copy * (1 - viscosity * dt);
    velocityY = velocityY_copy * (1 - viscosity * dt);
}

void FluidSimulation::advect(float dt) {
    cv::Mat newDensity = density.clone();

    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            float prevX = x - velocityX.at<float>(y, x) * dt;
            float prevY = y - velocityY.at<float>(y, x) * dt;

            prevX = std::max(0.f, std::min(width - 1.f, prevX));
            prevY = std::max(0.f, std::min(height - 1.f, prevY));

            newDensity.at<cv::Vec3f>(y, x) = density.at<cv::Vec3f>(cvRound(prevY), cvRound(prevX));
        }
    }

    cv::GaussianBlur(newDensity, density, cv::Size(5, 5), 1.5); // Smoother blending
}

void FluidSimulation::applyForce(int mouseX, int mouseY, const std::vector<cv::Scalar>& colors) {
    int radius = 50;  // Spread area for smoother effect
    float speedFactor = 2.5;  // Faster blending

    // Smooth the force application using a more realistic model
    for (int y = mouseY - radius; y < mouseY + radius; ++y) {
        for (int x = mouseX - radius; x < mouseX + radius; ++x) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                float dx = x - mouseX;
                float dy = y - mouseY;
                float distanceSquared = dx * dx + dy * dy;

                if (distanceSquared < radius * radius) {
                    float falloff = exp(-distanceSquared / (radius * radius));  // Gaussian falloff

                    // Apply a higher force to simulate a more liquid-like effect
                    falloff *= speedFactor;

                    velocityX.at<float>(y, x) += forceStrength * falloff * dx / radius;
                    velocityY.at<float>(y, x) += forceStrength * falloff * dy / radius;

                    // Blend colors between Pink and Purple
                    cv::Vec3f pink(255.0 / 255.0, 105.0 / 255.0, 180.0 / 255.0);  // Pink
                    cv::Vec3f purple(147.0 / 255.0, 112.0 / 255.0, 219.0 / 255.0); // Purple

                    float mixFactor = (dx + radius) / (2 * radius);  // Smooth blending
                    cv::Vec3f mixedColor = (1 - mixFactor) * pink + mixFactor * purple;

                    // Alpha blending to make the transition smooth
                    cv::Vec3f existingDensity = density.at<cv::Vec3f>(y, x);
                    cv::Vec3f newColor = (1 - falloff) * existingDensity + falloff * mixedColor;

                    // Store the color in the density field
                    density.at<cv::Vec3f>(y, x) = newColor;
                }
            }
        }
    }
}

void FluidSimulation::render(cv::Mat& canvas, const std::vector<cv::Scalar>& colors) {
    if (density.empty()) {
        std::cerr << "Error: Density matrix is empty!" << std::endl;
        return;
    }
    
    if (density.type() != CV_32FC3) {
        std::cerr << "Error: Incorrect density matrix type!" << std::endl;
        return;
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            cv::Vec3f colorMix = density.at<cv::Vec3f>(y, x);

            // Directly use input colors
            float r = colorMix[0] * colors[0][0];
            float g = colorMix[1] * colors[1][1];
            float b = colorMix[2] * colors[2][2];

            // Convert float to uchar (0-255)
            canvas.at<cv::Vec3b>(y, x) = cv::Vec3b(
                static_cast<uchar>(b),
                static_cast<uchar>(g),
                static_cast<uchar>(r)
            );
        }
    }
}


std::string FluidSimulation::getBase64PNG() {
    std::vector<uchar> buf;
    cv::imencode(".png", canvas, buf);
    return base64Encode(buf);
}

std::string FluidSimulation::base64Encode(const std::vector<uchar>& data) {
    static const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::stringstream ss;
    size_t i = 0;
    for (; i + 2 < data.size(); i += 3) {
        ss << base64_chars[(data[i] >> 2) & 0x3F]
           << base64_chars[((data[i] & 0x3) << 4) | ((data[i + 1] >> 4) & 0xF)]
           << base64_chars[((data[i + 1] & 0xF) << 2) | ((data[i + 2] >> 6) & 0x3)]
           << base64_chars[data[i + 2] & 0x3F];
    }
    if (i < data.size()) {
        ss << base64_chars[(data[i] >> 2) & 0x3F];
        if (i + 1 < data.size()) {
            ss << base64_chars[((data[i] & 0x3) << 4) | ((data[i + 1] >> 4) & 0xF)]
               << base64_chars[(data[i + 1] & 0xF) << 2] << "=";
        } else {
            ss << base64_chars[(data[i] & 0x3) << 4] << "==";
        }
    }
    return ss.str();
}
