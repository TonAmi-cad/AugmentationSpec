#pragma once

#ifndef AUGMENTATION_H
#define AUGMENTATION_H

#include <opencv2/opencv.hpp>

// Augmentation parameters
const float SCALE_LOW = 0.3f;
const float SCALE_HIGH = 0.9f;
const float ANGLE_LOW = -0.5f;
const float ANGLE_HIGH = 0.5f;
const float CONTRAST_LOW = 0.5f; // 0.7
const float CONTRAST_HIGH = 0.9f;
const float BRIGHTNESS_LOW = 0.3f; // 0.7
const float BRIGHTNESS_HIGH = 0.85f;
const int GAUSSIAN_NOISE_STDDEV = 25;
const float GAMMA_LOW = 1.2f;
const float GAMMA_HIGH = 1.7f; 

float getRandomFloat(float low, float high);
void addGaussianNoise(cv::Mat& image, double mean, double stddev);
void adjustBrightnessContrast(cv::Mat& image, float alpha, float beta);
cv::Mat augmentImage(const cv::Mat& src);

#endif // AUGMENTATION_H