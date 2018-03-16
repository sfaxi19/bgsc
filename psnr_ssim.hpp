//
// Created by sfaxi19 on 11.03.18.
//

#ifndef BACKGROUND_SUBTRACTED_PSNR_SSIM_HPP
#define BACKGROUND_SUBTRACTED_PSNR_SSIM_HPP

using namespace cv;

double getPSNR(const Mat &I1, const Mat &I2);

Scalar getMSSIM(const Mat &i1, const Mat &i2);

double psnr_between_videos(VideoCapture &video1, VideoCapture &video2);

double psnr_between_videos(const char *filepath1, const char *filepath2);

#endif //BACKGROUND_SUBTRACTED_PSNR_SSIM_HPP
