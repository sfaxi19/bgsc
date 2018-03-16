//
// Created by sfaxi19 on 14.03.18.
//
#include <iostream>
#include <cstdlib>
#include <opencv/cv.hpp>
#include <cstdarg>
#include "bgsc.hpp"
#include "psnr_ssim.hpp"

using namespace std;
using namespace cv;

double psnr_rebuilt = 0;
double psnr_fullrebuilt = 0;

Mat ShowManyImages(string title, int nArgs, ...) {
    int size;
    int i;
    int m, n;
    int x, y;

// w - Maximum number of images in a row
// h - Maximum number of images in a column
    int w, h;

// scale - How much we have to resize the image
    float scale;
    int max;

// If the number of arguments is lesser than 0 or greater than 12
// return without displaying
    if (nArgs <= 0) {
        printf("Number of arguments too small....\n");
        return Mat();
    } else if (nArgs > 14) {
        printf("Number of arguments too large, can only handle maximally 12 images at a time ...\n");
        return Mat();
    }
// Determine the size of the image,
// and the number of rows/cols
// from number of arguments
    else if (nArgs == 1) {
        w = h = 1;
        size = 500;
    } else if (nArgs == 2) {
        w = 2;
        h = 1;
        size = 500;
    } else if (nArgs == 3 || nArgs == 4) {
        w = 2;
        h = 2;
        size = 400;
    } else if (nArgs == 5 || nArgs == 6) {
        w = 3;
        h = 2;
        size = 300;
    } else if (nArgs == 7 || nArgs == 8) {
        w = 4;
        h = 2;
        size = 200;
    } else {
        w = 4;
        h = 3;
        size = 150;
    }

// Create a new 3 channel image
    Mat DispImage = Mat::zeros(Size(100 + size * w, 60 + size * h), CV_8UC3);

// Used to get the arguments passed
    va_list args;
    va_start(args, nArgs);

// Loop for nArgs number of arguments
    for (i = 0, m = 20, n = 20; i < nArgs; i++, m += (20 + size)) {
        // Get the Pointer to the IplImage
        Mat img = va_arg(args, Mat);

        // Check whether it is NULL or not
        // If it is NULL, release the image, and return
        if (img.empty()) {
            printf("Invalid arguments");
            return Mat();
        }

        // Find the width and height of the image
        x = img.cols;
        y = img.rows;

        // Find whether height or width is greater in order to resize the image
        max = (x > y) ? x : y;

        // Find the scaling factor to resize the image
        scale = (float) ((float) max / size);

        // Used to Align the images
        if (i % w == 0 && m != 20) {
            m = 20;
            n += 20 + size;
        }

        // Set the image ROI to display the current image
        // Resize the input image and copy the it to the Single Big Image
        Rect ROI(m, n, (int) (x / scale), (int) (y / scale));

        Mat temp;
        resize(img, temp, Size(ROI.width, ROI.height));
        temp.copyTo(DispImage(ROI));
    }
    putText(DispImage, "PSNR_BG_FG: " + std::to_string(psnr_rebuilt), cvPoint(10, 20),
            FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 0, 200), 1, CV_AA);
    putText(DispImage, "PSNR_full : " + std::to_string(psnr_fullrebuilt), cvPoint(10, 40),
            FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 200, 100), 1, CV_AA);
// Create a new window, and show the Single Big Image
    namedWindow(title, 1);
    imshow(title, DispImage);
    //waitKey();

// End the number of arguments
    va_end(args);
    return DispImage;
}

Mat differenceMask(Mat frame1, Mat frame2, int thr) {
    if (frame1.channels() != frame2.channels()) {
        cerr << "DifferenceMask frame1.channels() != frame2.channels() ";
        exit(EXIT_FAILURE);
    }
    if (frame1.size() != frame2.size()) {
        cerr << "DifferenceMask frame1.size() != frame2.size() ";
        exit(EXIT_FAILURE);
    }
    Mat mask(frame1.rows, frame1.cols, CV_8UC1);
    for (int i = 0; i < frame1.rows; i++) {
        for (int j = 0; j < frame1.cols; j++) {
            Vec3b v1 = frame1.at<Vec3b>(i, j);
            Vec3b v2 = frame2.at<Vec3b>(i, j);
            int value1 = abs(v1.val[0] - v2.val[0]);
            int value2 = abs(v1.val[1] - v2.val[1]);
            int value3 = abs(v1.val[2] - v2.val[2]);
            if ((value1 > thr) || (value2 > thr) || (value3 > thr)) {
                mask.at<uchar>(i, j) = 255;
            } else {
                mask.at<uchar>(i, j) = 0;
            }
        }
    }
    return mask;
}

void processVideo(char *videoFilename, int thr1, int thr2) {
    Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
    Ptr<BackgroundSubtractor> pMOG2_noize; //MOG2 Background subtractor
    pMOG2 = createBackgroundSubtractorMOG2(500, 40, false); //MOG2 approach
    //pMOG2_noize = createBackgroundSubtractorMOG2(500, 1, false);
    int keyboard; //input from keyboard
    VideoCapture capture(videoFilename);
    if (!capture.isOpened()) {
        //error in opening the video input
        cerr << "Unable to open video file: " << videoFilename << endl;
        exit(EXIT_FAILURE);
    }
    Mat frame; //current frame
    Mat fgMaskMOG2; //fg mask fg mask generated by MOG2 method
    //Mat fgMaskMOG_noize;
    system("mkdir -p results");
    const char *originFilepath = "results/origin.mkv";
    const char *bgFilepath = "results/bg.mkv";
    const char *motionFilepath = "results/motion.mkv";
    const char *rebuiltFilepath = "results/rebuilt.mkv";
    const char *fullRebuiltFilepath = "results/full_rebuilt.mkv";

    while ((char) keyboard != 'q' && (char) keyboard != 27) {
        //read the current frame
        if (!capture.read(frame)) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            break;
            //exit(EXIT_FAILURE);
        }
        static Mat bg(frame.size(), frame.type());
        //static Mat bg_noize(frame.size(), frame.type());

        pMOG2->apply(frame, fgMaskMOG2);
        //pMOG2_noize->apply(frame, fgMaskMOG_noize);
        pMOG2->getBackgroundImage(bg);

        //Mat out(frame.size(), frame.type());
        //Mat cleanMask(fgMaskMOG2.size(), fgMaskMOG2.type());
        int an0 = 0;
        int an1 = 1;
        int an2 = 4;
        static Mat element0 = getStructuringElement(MORPH_OPEN, Size(an0 * 2 + 1, an0 * 2 + 1), Point(an0, an0));
        static Mat element1 = getStructuringElement(MORPH_OPEN, Size(an1 * 2 + 1, an1 * 2 + 1), Point(an1, an1));
        static Mat element2 = getStructuringElement(MORPH_OPEN, Size(an2 * 2 + 1, an2 * 2 + 1), Point(an2, an2));
        //Mat img2 = frame;
        //Mat img1 = bg;
        //Mat diff;
        //Mat gray;
        static VideoWriter originVid(originFilepath, CV_FOURCC('M', 'J', 'P', 'G'), 30, frame.size(), true);
        static VideoWriter bgVid(bgFilepath, CV_FOURCC('M', 'J', 'P', 'G'), 10, frame.size(), true);
        static VideoWriter motionVid(motionFilepath, CV_FOURCC('M', 'J', 'P', 'G'), 30, frame.size(), true);
        static VideoWriter rebuiltVid(rebuiltFilepath, CV_FOURCC('M', 'J', 'P', 'G'), 30, frame.size(), true);
        static VideoWriter fullRebuiltVid(fullRebuiltFilepath, CV_FOURCC('M', 'J', 'P', 'G'), 30, frame.size(), true);

        //absdiff(img1, img2, diff);
        //imshow("diff", diff);

        //threshold(diff, diff, 50, 255, THRESH_BINARY);
        //Mat cannyTmp1;
        // cvtColor(diff, gray, CV_RGB2GRAY);
        //Canny(gray, cannyTmp1, 100, 200, 3);
        //vector<vector<Point> > contours;
        //findContours(cannyTmp1, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);
        // you could also reuse img1 here
        //Mat mask2 = Mat::zeros(frame.size(), CV_8UC1);

        // CV_FILLED fills the connected components found
        //drawContours(mask2, contours, -1, Scalar(255), CV_FILLED);
        //erode(mask2, mask2, element1);
        //dilate(mask2, mask2, element2);

        //imshow("diff", diff);
        //imshow("mask2", mask2);
        Mat maskFG = differenceMask(bg, frame, thr1);
        erode(maskFG, maskFG, element1);
        //maskClean = maskClean + cannyTmp1;
        dilate(maskFG, maskFG, element1);
        dilate(maskFG, maskFG, element1);
        Mat maskNoize = differenceMask(bg, frame, thr2);
        imshow("FG", maskFG);

        //Mat maskClean;
        //threshold(fgMaskMOG2, maskClean, 200, 255, THRESH_BINARY);
        //dilate(maskClean, maskClean, element0);
        //erode(maskClean, maskClean, element1);
        //erode(maskClean, maskClean, element1);
        //erode(maskClean, maskClean, element1);
        //maskClean = maskClean + cannyTmp1;
        //dilate(maskClean, maskClean, element1);
        //dilate(maskClean, maskClean, element2);

        //imshow("cannydiff", cannyTmp1);
        //imshow("mask", mask2);

        Mat frameFG;
        frame.copyTo(frameFG, maskFG);

        Mat frameBG;
        frame.copyTo(frameBG, maskNoize - maskFG);

        //fgMaskMOG2.copyTo(bg_frame, not_mask);
        //bitwise_and(img2, img1, res, mask1);

        Mat rebuilt(frame.size(), frame.type());
        bg.copyTo(rebuilt);
        frameFG.copyTo(rebuilt, maskFG);

        Mat full_rebuilt(frame.size(), frame.type());
        rebuilt.copyTo(full_rebuilt);
        Mat bitwise;
        bitwise_xor(maskNoize, maskFG, bitwise);
        frame.copyTo(full_rebuilt, bitwise);

        // write frames to files
        originVid.write(frame);
        bgVid.write(frameBG);
        motionVid.write(frameFG);
        rebuiltVid.write(rebuilt);
        fullRebuiltVid.write(full_rebuilt);

        psnr_rebuilt = getPSNR(frame, rebuilt);
        psnr_fullrebuilt = getPSNR(frame, full_rebuilt);
        //imshow("Rebuilt", rebuilt);
        ShowManyImages("Result", 6, frame, rebuilt, full_rebuilt, bg, frameBG, frameFG);
        //imshow("bitwise", bitwise);
        keyboard = waitKey(30);
    }
    if (keyboard != 'q') {
        double psnr_rebuilt = psnr_between_videos(originFilepath, rebuiltFilepath);
        double psnr_fullrebuilt = psnr_between_videos(originFilepath, fullRebuiltFilepath);
        printf("PSNR[BG' + FG']c      = %f\n", psnr_rebuilt);
        printf("PSNR[BG' + FG' + N'] = %f\n", psnr_fullrebuilt);
    }
    //delete capture object
    capture.release();
}


void processVideoRecon(char *basefile, char *enhancefile) {
    int keyboard = 0;
    VideoCapture baseVid(basefile);
    if (!baseVid.isOpened()) {
        //error in opening the video input
        cerr << "Unable to open video file: " << basefile << endl;
        exit(EXIT_FAILURE);
    }
    VideoCapture enhVid(enhancefile);
    if (!enhVid.isOpened()) {
        //error in opening the video input
        cerr << "Unable to open video file: " << enhancefile << endl;
        exit(EXIT_FAILURE);
    }
    Mat base_frame;
    Mat enh_frame;
    Mat mask;
    while ((char) keyboard != 'q' && (char) keyboard != 27) {
        if (!(baseVid.read(base_frame) && enhVid.read(enh_frame))) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            break;
        }
        Mat gray(base_frame.rows, base_frame.cols, CV_8UC1);
        threshold(base_frame, mask, 2, 255, THRESH_BINARY);
        imshow("mask", mask);
        imshow("Base", base_frame);
        //printf("frame\n");
        keyboard = waitKey(30);
    }
}
