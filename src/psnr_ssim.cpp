//
// Created by sfaxi19 on 11.03.18.
//

#include <opencv2/core/hal/interface.h>
#include <opencv/cv.h>
#include <opencv/cv.hpp>
#include <iostream>
#include "psnr_ssim.hpp"

using namespace cv;
using namespace std;

double psnr_between_videos(VideoCapture &video1, VideoCapture &video2, int channel) {
    Mat vid1;
    Mat vid2;
    uint64_t psnr = 0;
    int psnr_cnt = 0;
    int keyboard = 0;
    int frame_idx = 0;
    while ((char) keyboard != 'q' && (char) keyboard != 27) {

        if (!(video1.read(vid1) && video2.read(vid2))) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            break;
        }
        double psnr_cur = getPSNR(vid1, vid2, channel);
        if (psnr_cur > 100) psnr_cur = 100;
        std::cout << "PSNR[" << frame_idx++ << "] = " << psnr_cur << endl;
        psnr += round(psnr_cur);

        psnr_cnt++;
    }
    return (double) psnr / psnr_cnt;
}

double psnr_between_videos(VideoCapture &video1, VideoCapture &video2, VideoCapture &video3, int channel) {
    Mat vid1;
    Mat vid2;
    Mat vid_mask;
    uint64_t psnr = 0;
    int psnr_cnt = 0;
    int keyboard = 0;
    int frame_idx = 0;
    while ((char) keyboard != 'q' && (char) keyboard != 27) {

        if (!(video1.read(vid1) && video2.read(vid2) && video3.read(vid_mask))) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            break;
        }
        double psnr_cur = getPSNR(vid1, vid2, vid_mask, channel);
        if (psnr_cur > 100) psnr_cur = 100;
        std::cout << "PSNR[" << frame_idx++ << "] = " << psnr_cur << endl;
        psnr += round(psnr_cur);
        psnr_cnt++;
    }
    return (double) psnr / psnr_cnt;
}

double psnr_between_videos(const char *filepath1, const char *filepath2, const char *filepath3, int channel) {
    VideoCapture file1(filepath1);
    VideoCapture file2(filepath2);
    if (filepath3 == nullptr) {
        return psnr_between_videos(file1, file2, channel);
    } else {
        VideoCapture file3(filepath3);
        return psnr_between_videos(file1, file2, file3, channel);
    }
}

double mssim_between_videos(VideoCapture &video1, VideoCapture &video2) {
    Mat vid1;
    Mat vid2;
    double mssim = 0;
    int mssim_cnt = 0;
    int keyboard;
    while ((char) keyboard != 'q' && (char) keyboard != 27) {

        if (!(video1.read(vid1) && video1.read(vid1))) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            break;
        }
        //mssim += getMSSIM(vid1, vid2);
        mssim_cnt++;
    }
    return mssim / mssim_cnt;
}

double getPSNR(const Mat I1, const Mat I2, int channel) {
    Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |I1 - I2|^2
    Scalar s = sum(s1);        // sum elements per channel
    double sse = 0;
    int channels = 0;
    if (channel < 0) {
        sse = s.val[0] + s.val[1] + s.val[2]; // sum channels
        channels = 3;
    } else {
        sse = s.val[channel];
        channels = 1;
    }

    double mse = sse / (double) (channels * I1.total());
    double psnr = 10.0 * log10((255 * 255) / mse);
    return psnr;
}

double getPSNR(const Mat I1, const Mat I2, const Mat mask, int channel) {
    threshold(mask, mask, 128, 255, CV_8U);
    Mat s1;
    size_t num = 0;
    Scalar s;

    absdiff(I1, I2, s1);       // |I1 - I2|
    Mat s2;
    s2 = 0;
    Scalar num_s = sum(mask);

    /*int v1sum = 0;
    int v2sum = 0;
    int v3sum = 0;
    for (int i = 0; i < mask.rows; i++) {
        for (int j = 0; j < mask.cols; j++) {
            Vec3b v1 = mask.at<Vec3b>(i, j);
            if(v1.val[0] > 0) v1sum++;
            if(v1.val[1] > 0) v2sum++;
            if(v1.val[2] > 0) v3sum++;
        }
    }
    printf("v1: %d\n",v1sum);
    printf("v2: %d\n",v2sum);
    printf("v3: %d\n",v3sum);
    printf("num1: %f\n",num_s.val[0]/255);
    printf("num2: %f\n",num_s.val[1]);
    printf("num3: %f\n",num_s.val[2]);
    printf("mask: %dx%d\n\n",mask.cols, mask.rows);
    */
    s1.copyTo(s2, mask);
    s2.convertTo(s2, CV_32F);  // cannot make a square on 8 bits
    //s1.copyTo(s1, mask);
    s2 = s2.mul(s2);
    static int cnt = 0;

    //if(cnt++ > 3) exit(1);
    //std::cout << num_s.val[0] / 255 << endl;

    num = static_cast<size_t>(num_s.val[0] / 255);
    if (num == 0) return 0;
    s = sum(s2);

    double sse = 0;
    int channels = 0;
    if (channel < 0) {
        sse = s.val[0] + s.val[1] + s.val[2]; // sum channels
        channels = 3;
    } else {
        sse = s.val[channel];
        channels = 1;
    }

    double mse = sse / (double) (channels * num);
    double psnr = 10.0 * log10((255 * 255) / mse);

    return psnr;
}

Scalar getMSSIM(const Mat &i1, const Mat &i2) {
    const double C1 = 6.5025, C2 = 58.5225;
    /***************************** INITS **********************************/
    int d = CV_32F;

    Mat I1, I2;
    i1.convertTo(I1, d);            // cannot calculate on one byte large values
    i2.convertTo(I2, d);

    Mat I2_2 = I2.mul(I2);        // I2^2
    Mat I1_2 = I1.mul(I1);        // I1^2
    Mat I1_I2 = I1.mul(I2);        // I1 * I2

    /*************************** END INITS **********************************/

    Mat mu1, mu2;                   // PRELIMINARY COMPUTING
    GaussianBlur(I1, mu1, Size(11, 11), 1.5);
    GaussianBlur(I2, mu2, Size(11, 11), 1.5);

    Mat mu1_2 = mu1.mul(mu1);
    Mat mu2_2 = mu2.mul(mu2);
    Mat mu1_mu2 = mu1.mul(mu2);

    Mat sigma1_2, sigma2_2, sigma12;

    GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
    sigma1_2 -= mu1_2;

    GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
    sigma2_2 -= mu2_2;

    GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
    sigma12 -= mu1_mu2;

    ///////////////////////////////// FORMULA ////////////////////////////////
    Mat t1, t2, t3;

    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);                 // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

    t1 = mu1_2 + mu2_2 + C1;
    t2 = sigma1_2 + sigma2_2 + C2;
    t1 = t1.mul(t2);                 // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

    Mat ssim_map;
    divide(t3, t1, ssim_map);        // ssim_map =  t3./t1;

    Scalar mssim = mean(ssim_map);   // mssim = average of ssim map
    return mssim;
}