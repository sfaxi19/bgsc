//
// Created by sfaxi19 on 14.03.18.
//
#include <iostream>
#include <cstdlib>
#include <opencv/cv.hpp>
#include <cstdarg>
#include <iomanip>
#include "bgsc.hpp"
#include "psnr_ssim.hpp"

using namespace std;
using namespace cv;

Mat ShowManyImages(string title, int nArgs, ...) {
    //int nArgs = nImgs * 2;
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
    } else if (nArgs == 1) {
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
    Mat DispImage = Mat::zeros(Size(100 + size * w, 60 + size * h), CV_8UC3);

    va_list args;
    va_start(args, nArgs);

// Loop for nArgs number of arguments
    for (i = 0, m = 20, n = 20; i < nArgs; i++, m += (20 + size)) {
        // Get the Pointer to the IplImage
        string img_title = va_arg(args, string);
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
        Rect ROI(m, n + 10, (int) (x / scale), (int) (y / scale));

        Mat temp;
        resize(img, temp, Size(ROI.width, ROI.height));
        temp.copyTo(DispImage(ROI));
        putText(DispImage, img_title, cvPoint(m, n),
                FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 0, 200), 1, CV_AA);
    }
    /*
    putText(DispImage, "PSNR_BG_FG: " + std::to_string(psnr_rebuilt), cvPoint(10, 20),
            FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 0, 200), 1, CV_AA);
    putText(DispImage, "PSNR_full : " + std::to_string(psnr_fullrebuilt), cvPoint(10, 40),
            FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255, 200, 100), 1, CV_AA);
    */
    // Create a new window, and show the Single Big Image
    //namedWindow(title, 1);
    //imshow(title, DispImage);
    //waitKey();

// End the number of arguments
    va_end(args);
    return DispImage;
}

Mat additionMat(Mat &frame1, Mat &frame2) {
    if (frame1.channels() != frame2.channels()) {
        cerr << "AdditionMask frame1.channels() != frame2.channels() ";
        exit(EXIT_FAILURE);
    }
    if (frame1.size() != frame2.size()) {
        cerr << "AdditionMask frame1.size() != frame2.size() ";
        exit(EXIT_FAILURE);
    }
    Mat sum(frame1.size(), frame1.type());
    for (int i = 0; i < frame1.rows; i++) {
        for (int j = 0; j < frame1.cols; j++) {
            Vec3b v1 = frame1.at<Vec3b>(i, j);
            Vec3b v2 = frame2.at<Vec3b>(i, j);
            uchar value0 = v1.val[0] + (((int) v2.val[0]) - 128);
            uchar value1 = v1.val[1] + (((int) v2.val[1]) - 128);
            uchar value2 = v1.val[2] + (((int) v2.val[2]) - 128);
            sum.at<Vec3b>(i, j)[0] = value0;
            sum.at<Vec3b>(i, j)[1] = value1;
            sum.at<Vec3b>(i, j)[2] = value2;
        }
    }
    return sum;
}

Mat differenceMat(Mat &frame1, Mat &frame2, int thr = 0) {
    if (frame1.channels() != frame2.channels()) {
        cerr << "DifferenceMask frame1.channels() != frame2.channels() ";
        exit(EXIT_FAILURE);
    }
    if (frame1.size() != frame2.size()) {
        cerr << "DifferenceMask frame1.size() != frame2.size() ";
        exit(EXIT_FAILURE);
    }
    Mat diff(frame1.size(), frame1.type());
    for (int i = 0; i < frame1.rows; i++) {
        for (int j = 0; j < frame1.cols; j++) {
            Vec3b v1 = frame1.at<Vec3b>(i, j);
            Vec3b v2 = frame2.at<Vec3b>(i, j);
            int value0 = v1.val[0] - v2.val[0];
            int value1 = v1.val[1] - v2.val[1];
            int value2 = v1.val[2] - v2.val[2];
            if ((abs(value0) > thr) || (abs(value1) > thr) || (abs(value2) > thr)) {
                diff.at<Vec3b>(i, j)[0] = static_cast<uchar>(128 + value0);
                diff.at<Vec3b>(i, j)[1] = static_cast<uchar>(128 + value1);
                diff.at<Vec3b>(i, j)[2] = static_cast<uchar>(128 + value2);
            } else {
                diff.at<Vec3b>(i, j)[0] = static_cast<uchar>(128);
                diff.at<Vec3b>(i, j)[1] = static_cast<uchar>(128);
                diff.at<Vec3b>(i, j)[2] = static_cast<uchar>(128);
            }
        }
    }
    return diff;
}

Mat differenceMask(Mat &frame1, Mat &frame2, int thr) {
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
            //Vec3b v1 = ;
            //Vec3b v2 = frame2.at<Vec3b>(i, j);
            int value1 = abs(frame1.at<Vec3b>(i, j).val[0] - frame2.at<Vec3b>(i, j).val[0]);
            int value2 = abs(frame1.at<Vec3b>(i, j).val[1] - frame2.at<Vec3b>(i, j).val[1]);
            int value3 = abs(frame1.at<Vec3b>(i, j).val[2] - frame2.at<Vec3b>(i, j).val[2]);
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

    double psnr_rebuilt = 0;
    double psnr_fullrebuilt = 0;

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
    system("mkdir -p results/rebuilt/");
    system("mkdir -p results/bgu/");
    system("mkdir -p results/origin/");

    system("rm results/rebuilt/*");
    system("rm results/bgu/*");
    system("rm results/compress/*");
    system("rm results/origin/*");

    const char *originFilepath = "results/origin.mkv";
    const char *bgFilepath = "results/bg.mkv";
    const char *motionFilepath = "results/motion.mkv";
    const char *rebuiltFilepath = "results/rebuilt/rebuilt.mkv";
    const char *fullRebuiltFilepath = "results/full_rebuilt.mkv";
    const char *cellsFilepath = "results/cells.mkv";
    int an0 = 0;
    int an1 = 1;
    int an2 = 4;

    Mat element0 = getStructuringElement(MORPH_OPEN, Size(an0 * 2 + 1, an0 * 2 + 1), Point(an0, an0));
    Mat element1 = getStructuringElement(MORPH_OPEN, Size(an1 * 2 + 1, an1 * 2 + 1), Point(an1, an1));
    Mat element2 = getStructuringElement(MORPH_OPEN, Size(an2 * 2 + 1, an2 * 2 + 1), Point(an2, an2));

    int frameIdx = 0;
    int N = 100;

    while ((char) keyboard != 'q' && (char) keyboard != 27) {
        if (!capture.read(frame)) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            break;
        }

        static Mat frameBG(frame.size(), frame.type());

        pMOG2->apply(frame, fgMaskMOG2);
        if (frameIdx++ % N == 0) {
            pMOG2->getBackgroundImage(frameBG);
        }

        //Mat out(frame.size(), frame.type());
        //Mat cleanMask(fgMaskMOG2.size(), fgMaskMOG2.type());
        //Mat img2 = frame;
        //Mat img1 = bg;
        //Mat diff;
        //Mat gray;
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
        Mat maskFG = differenceMask(frameBG, frame, thr1);
        erode(maskFG, maskFG, element1);
//        //maskClean = maskClean + cannyTmp1;
        dilate(maskFG, maskFG, element1);
//        dilate(maskFG, maskFG, element1);
        //Mat maskNoize = differenceMask(frameBG, frame, thr2);
        //imshow("FG", maskFG);

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


        Mat rebuilt(frame.size(), frame.type());
        frameBG.copyTo(rebuilt);
        frameFG.copyTo(rebuilt, maskFG);


        Mat frameBGU = differenceMat(frame, rebuilt, thr2);
        //frame.copyTo(frameBG, maskNoize - maskFG);

        Mat full_rebuilt(frame.size(), frame.type());
        rebuilt.copyTo(full_rebuilt);
        full_rebuilt = additionMat(full_rebuilt, frameBGU);

        psnr_rebuilt = getPSNR(frame, rebuilt);
        psnr_fullrebuilt = getPSNR(frame, full_rebuilt);

        stringstream psnr_rebuilt_stream;
        psnr_rebuilt_stream << fixed << setprecision(2) << psnr_rebuilt;
        stringstream psnr_fullrebuilt_stream;
        psnr_fullrebuilt_stream << fixed << setprecision(2) << psnr_fullrebuilt;
        Mat frameCells = ShowManyImages("Result", 6,
                                        string("F"), frame,
                                        string("F'=BG+FG PSNR[F, F']: ") + psnr_rebuilt_stream.str(), rebuilt,
                                        string("F''=BG+FG+N PSNR[F, F'']: ") + psnr_fullrebuilt_stream.str(),
                                        full_rebuilt,
                                        string("BG[idx=") + to_string((int) floor(frameIdx / N)) + "]", frameBG,
                                        string("FG[thr=") + to_string(thr1) + "]", frameFG,
                                        string("N"), frameBGU);
        static double fps = capture.get(CV_CAP_PROP_FPS);
        imwrite("results/rebuilt/in" + to_string(frameIdx) + ".bmp", rebuilt);
        imwrite("results/bgu/in" + to_string(frameIdx) + ".bmp", frameBGU);
        imwrite("results/origin/in" + to_string(frameIdx) + ".bmp", frame);
        //static VideoWriter originVid(originFilepath, CV_FOURCC('D', 'I', 'B', ' '), fps, frame.size(), true);
        static VideoWriter bgVid(bgFilepath, CV_FOURCC('M', 'J', 'P', 'G'), fps, frame.size(), true);
        static VideoWriter motionVid(motionFilepath, CV_FOURCC('M', 'J', 'P', 'G'), fps, frame.size(), true);
        static VideoWriter rebuiltVid(rebuiltFilepath, CV_FOURCC('M', 'J', 'P', 'G'), fps, frame.size(), true);
        static VideoWriter fullRebuiltVid(fullRebuiltFilepath, CV_FOURCC('M', 'J', 'P', 'G'), fps, frame.size(), true);
        static VideoWriter cellsVid(cellsFilepath, CV_FOURCC('M', 'J', 'P', 'G'), fps, frameCells.size(), true);
        //originVid.write(frame);
        bgVid.write(frameBGU);
        motionVid.write(frameFG);
        rebuiltVid.write(rebuilt);
        fullRebuiltVid.write(full_rebuilt);
        cellsVid.write(frameCells);
        imshow("Results", frameCells);
        keyboard = waitKey(1);
    }
    if (keyboard != 'q') {
        system("rm results/compress/*");
        system("ffmpeg -i results/rebuilt/in%d.bmp -vcodec libx264 -preset veryslow -pix_fmt yuv420p results/compress/rebuilt.mkv");
        system("ffmpeg -i results/bgu/in%d.bmp -vcodec libx264 -preset veryslow -pix_fmt yuv420p results/compress/bgu.mkv");
        system("ffmpeg -i results/origin/in%d.bmp -vcodec libx264 -preset veryslow  -pix_fmt yuv420p results/compress/origin.mkv");
        system("ls -lh results/compress/");
        //double psnr_rebuilt = psnr_between_videos(originFilepath, rebuiltFilepath);
        //double psnr_fullrebuilt = psnr_between_videos(originFilepath, fullRebuiltFilepath);
        //printf("PSNR[BG' + FG']      = %f\n", psnr_rebuilt);
        //printf("PSNR[BG' + FG' + N'] = %f\n", psnr_fullrebuilt);
    }
    //delete capture object
    //cellsVid.release();
    capture.release();
}


void processVideoRecon(char *basefile, char *enhancefile, char *output) {
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
    Mat new_frame;
    static double fps = baseVid.get(CV_CAP_PROP_FPS);
    while ((char) keyboard != 'q' && (char) keyboard != 27) {
        if (!(baseVid.read(base_frame) && enhVid.read(enh_frame))) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            break;
        }
        static VideoWriter writer(output, CV_FOURCC('M', 'J', 'P', 'G'), fps, base_frame.size(), true);
        new_frame = additionMat(base_frame,enh_frame);
        writer.write(new_frame);
        keyboard = waitKey(1);
    }
}
