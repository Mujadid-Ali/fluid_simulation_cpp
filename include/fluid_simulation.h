#ifndef FLUID_SIMULATION_H
#define FLUID_SIMULATION_H
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

class FluidSimulation {
public:
    FluidSimulation(int width = 512, int height = 512, float viscosity = 0.1, float forceStrength = 50.0);
    void updateFluid(int mouseX, int mouseY, const std::vector<cv::Scalar>& colors);
    std::string getBase64PNG();

private:
    int width, height;
    float viscosity, forceStrength;
    cv::Mat canvas;
    cv::Mat velocityX, velocityY, density, pressure;
    void diffuse(float dt);
    void advect(float dt);
    void applyForce(int mouseX, int mouseY, const std::vector<cv::Scalar>& colors);
    void render(cv::Mat& canvas, const std::vector<cv::Scalar>& colors);
    std::string base64Encode(const std::vector<uchar>& data);
};

#endif // FLUID_SIMULATION_H