#include "fluid_simulation.h"
#include "visualizer.h"
#include <iostream>
#include <opencv2/opencv.hpp>

cv::Point mousePos(-1, -1);

void mouseCallback(int event, int x, int y, int, void*) {
    if (event == cv::EVENT_MOUSEMOVE) {
        mousePos = cv::Point(x, y);
    }
}

int main() {
    // Initialize colors
    const std::vector<cv::Scalar> colors = { cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 255) };

    // Create a window
    cv::namedWindow("Fluid Simulation", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("Fluid Simulation", mouseCallback);

    FluidSimulation fluidSim(512, 512);

    while (true) {
        if (mousePos.x >= 0 && mousePos.y >= 0) {

            while (true) {
                if (mousePos.x >= 0 && mousePos.y >= 0) {
                    // Convert mouse position to percentage
                    double mouseXPercent = static_cast<double>(mousePos.x) / 512 * 100;
                    double mouseYPercent = static_cast<double>(mousePos.y) / 512 * 100;

                    //std::cout << "Mouse position: (" << mouseXPercent << "%, " << mouseYPercent << "%)" << std::endl;
                    fluidSim.updateFluid(mousePos.x, mousePos.y, colors);
                }

                std::string base64Image = fluidSim.getBase64PNG();
                visualizeBase64Image(base64Image);

                if (cv::waitKey(30) >= 0) break;
            }
            fluidSim.updateFluid(mousePos.x, mousePos.y, colors);
        }

        std::string base64Image = fluidSim.getBase64PNG();
        visualizeBase64Image(base64Image);

        if (cv::waitKey(30) >= 0) break;
    }

    cv::destroyAllWindows();
    return 0;
}
