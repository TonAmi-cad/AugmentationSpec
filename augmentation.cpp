#include "augmentation.h"
#include <random>
#include <opencv2/opencv.hpp>

using namespace cv;

float getRandomFloat(float low, float high) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(low, high);
    return static_cast<float>(dist(gen));
}

void addGaussianNoise(Mat& image, double mean, double stddev) {
    Mat noise(image.size(), image.type());
    randn(noise, mean, stddev);
    image += noise;
}

void adjustBrightnessContrast(Mat& image, float alpha, float beta) {
    image.convertTo(image, -1, alpha, beta);
}

void applyGammaCorrection(Mat& image, float gamma) {
    Mat lut_matrix(1, 256, CV_8UC1);
    uchar* ptr = lut_matrix.ptr();
    for (int i = 0; i < 256; i++) {
        ptr[i] = pow(i / 255.0, gamma) * 255.0;
    }
    LUT(image, lut_matrix, image);
}


Mat augmentImage(const Mat& src) {
    Mat mask, foreground;

    // del background
    inRange(src, Scalar(200, 200, 200), Scalar(255, 255, 255), mask);
    bitwise_not(mask, mask);
    src.copyTo(foreground, mask);

    // scale
    float random_scale = getRandomFloat(SCALE_LOW, SCALE_HIGH);
    Mat scaled;
    resize(foreground, scaled, Size(), random_scale, random_scale);
    resize(mask, mask, Size(), random_scale, random_scale);

    // rotation
    float random_angle = getRandomFloat(ANGLE_LOW, ANGLE_HIGH);
    Point2f center(scaled.cols / 2.0, scaled.rows / 2.0);
    Mat rotationMatrix = getRotationMatrix2D(center, random_angle, 1.0);
    Mat rotated, rotated_mask;
    warpAffine(scaled, rotated, rotationMatrix, scaled.size());
    warpAffine(mask, rotated_mask, rotationMatrix, scaled.size());

    // brightness and contrast
    float random_alpha = getRandomFloat(CONTRAST_LOW, CONTRAST_HIGH);
    float random_beta = getRandomFloat(BRIGHTNESS_LOW, BRIGHTNESS_HIGH) * 50;
    adjustBrightnessContrast(rotated, random_alpha, random_beta);

    // Gamma correction
    float random_gamma = getRandomFloat(GAMMA_LOW, GAMMA_HIGH);
    applyGammaCorrection(rotated, random_gamma);

    // White background
    Mat output = Mat(1080, 1920, CV_8UC3, Scalar(255, 255, 255));
    int max_x = output.cols - rotated.cols;
    int max_y = output.rows - rotated.rows;
    int random_x = static_cast<int>(getRandomFloat(0, max_x));
    int random_y = static_cast<int>(getRandomFloat(0, max_y));
    rotated.copyTo(output(Rect(random_x, random_y, rotated.cols, rotated.rows)), rotated_mask);

    // Noise
    Mat object_roi = output(Rect(random_x, random_y, rotated.cols, rotated.rows));
    addGaussianNoise(object_roi, 0, GAUSSIAN_NOISE_STDDEV);
    GaussianBlur(object_roi, object_roi, Size(5, 5), 0);

    return output;
}