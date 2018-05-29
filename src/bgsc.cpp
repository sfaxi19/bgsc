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
#ifdef NDEBUG
#define DEBUG if(false)
#else
#define DEBUG if(true)
#endif

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
        cerr << "AdditionMat frame1.channels() != frame2.channels() ";
        exit(EXIT_FAILURE);
    }
    if (frame1.size() != frame2.size()) {
        cerr << "AdditionMat frame1.size() != frame2.size() ";
        exit(EXIT_FAILURE);
    }
    Mat sum(frame1.size(), frame1.type());
    for (int i = 0; i < frame1.rows; i++) {
        for (int j = 0; j < frame1.cols; j++) {
            Vec3b v1 = frame1.at<Vec3b>(i, j);
            Vec3b v2 = frame2.at<Vec3b>(i, j);
            /*if ((v2.val[0] < 90) || (v2.val[0] > 200)) {
                printf("val: %d\n", v2.val[0]);
            }*/
            uchar value0 = clip(v1.val[0] +
                                ((v2.val[0] % 2 == 0) ? v2.val[0] / 2 : (v2.val[0] + 1) / -2), 0, 255);
            uchar value1 = clip(v1.val[1] +
                                ((v2.val[1] % 2 == 0) ? v2.val[1] / 2 : (v2.val[1] + 1) / -2), 0, 255);
            uchar value2 = clip(v1.val[2] +
                                ((v2.val[2] % 2 == 0) ? v2.val[2] / 2 : (v2.val[2] + 1) / -2), 0, 255);
            sum.at<Vec3b>(i, j)[0] = value0;
            sum.at<Vec3b>(i, j)[1] = value1;
            sum.at<Vec3b>(i, j)[2] = value2;
        }
    }
    return sum;
}

Mat differenceMat(Mat &frame1, Mat &frame2, int thr = 0) {
    if (frame1.channels() != frame2.channels()) {
        cerr << "DifferenceMat frame1.channels() != frame2.channels() ";
        exit(EXIT_FAILURE);
    }
    if (frame1.size() != frame2.size()) {
        cerr << "DifferenceMat frame1.size() != frame2.size() ";
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
                diff.at<Vec3b>(i, j)[0] = clip((value0 >= 0) ? 2 * value0 : (-2 * value0 - 1), 0, 255);
                diff.at<Vec3b>(i, j)[1] = clip((value1 >= 0) ? 2 * value1 : (-2 * value1 - 1), 0, 255);
                diff.at<Vec3b>(i, j)[2] = clip((value2 >= 0) ? 2 * value2 : (-2 * value2 - 1), 0, 255);
            } else {
                diff.at<Vec3b>(i, j)[0] = 0;
                diff.at<Vec3b>(i, j)[1] = 0;
                diff.at<Vec3b>(i, j)[2] = 0;
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

void processVideo(char *videoFilename, int thr1, int thr2, int N) {
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
    system("mkdir -p results/rebuiltv2/");
    system("mkdir -p results/rebuiltv3/");
    system("mkdir -p results/bgu/");
    system("mkdir -p results/origin/");
    system("mkdir -p results/fg/");
    system("mkdir -p results/mask/");

    system("rm results/rebuilt/*");
    system("rm results/rebuiltv2/*");
    system("rm results/rebuiltv3/*");
    system("rm results/bgu/*");
    system("rm results/compress/*");
    system("rm results/origin/*");
    system("rm results/fg/*");
    system("rm results/mask/*");

    const char *originFilepath = "results/origin.mkv";
    const char *bgFilepath = "results/bg.mkv";
    const char *motionFilepath = "results/motion.mkv";
    const char *rebuiltFilepath = "results/rebuilt.mkv";
    const char *fullRebuiltFilepath = "results/full_rebuilt.mkv";
    const char *cellsFilepath = "results/cells.mkv";
    const char *maskFilepath = "results/mask.mkv";
    int an0 = 0;
    int an1 = 1;
    int an2 = 4;

    Mat element0 = getStructuringElement(MORPH_OPEN, Size(an0 * 2 + 1, an0 * 2 + 1), Point(an0, an0));
    Mat element1 = getStructuringElement(MORPH_OPEN, Size(an1 * 2 + 1, an1 * 2 + 1), Point(an1, an1));
    Mat element2 = getStructuringElement(MORPH_OPEN, Size(an2 * 2 + 1, an2 * 2 + 1), Point(an2, an2));

    int frameIdx = 0;

    while (((char) keyboard != 'q' && (char) keyboard != 27) && (frameIdx < 300)) {
        if (!capture.read(frame)) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            break;
        }

        static Mat frameBG(frame.size(), frame.type());

        pMOG2->apply(frame, fgMaskMOG2);
        if (frameIdx++ % N == 0) {
            //frameIdx++;
            pMOG2->getBackgroundImage(frameBG);
        }
        Mat maskFG = differenceMask(frameBG, frame, thr1);
        erode(maskFG, maskFG, element1);
//      maskClean = maskClean + cannyTmp1;
        dilate(maskFG, maskFG, element2);
        //dilate(maskFG, maskFG, element1);
        //Mat maskNoize = differenceMask(frameBG, frame, thr2);
        //imshow("FG", maskFG);


        //imshow("cannydiff", cannyTmp1);
        //imshow("mask", mask2);

        /***************************************
         *           Foreground frame
         ***************************************/
        Mat frameFG;
        frame.copyTo(frameFG, maskFG);

        /***************************************
         *             Blur frame
         ***************************************/
        Mat frameBlur;
        //frameBG.copyTo(frameBlur, maskFG);
        frame.copyTo(frameBlur);
        //blur(frameBlur, frameBlur, Size(20, 20));
        GaussianBlur(frameBlur, frameBlur, Size(31, 31), 0, 0);
        /***************************************
         *            Rebuilt frame v1
         ***************************************/
        Mat rebuiltv1(frame.size(), frame.type());
        frameBlur.copyTo(rebuiltv1);
        //frameBG.copyTo(rebuilt);
        frameFG.copyTo(rebuiltv1, maskFG);
        /***************************************
         *            Rebuilt frame v2
         ***************************************/
        Mat rebuiltv2(frame.size(), frame.type());
        frameBG.copyTo(rebuiltv2);
        frameFG.copyTo(rebuiltv2, maskFG);
        /***************************************
         *            Rebuilt frame v3
         ***************************************/
        Mat rebuiltv3(frame.size(), frame.type());
        frameBG.copyTo(rebuiltv3);
        GaussianBlur(rebuiltv3, rebuiltv3, Size(5, 5), 0, 0);
        frameFG.copyTo(rebuiltv3, maskFG);
        /***************************************
         *             BGU frame
         ***************************************/
        Mat frameBGU = differenceMat(frame, rebuiltv1, thr2); //additionMat(frameBG, diffMat);
        
        Mat frameCells;
        static double fps = capture.get(CAP_PROP_FPS);
        DEBUG {


            /***************************************
             *          Full rebuilt frame
             ***************************************/
            Mat full_rebuilt(frame.size(), frame.type());
            rebuiltv1.copyTo(full_rebuilt);
            Mat diffMatRebuilt = frameBGU;//differenceMat(frameBG, frameBGU);//frameBGU;
            full_rebuilt = additionMat(full_rebuilt, diffMatRebuilt);
            /***************************************
             *             Calc PSNR
             ***************************************/
            psnr_rebuilt = getPSNR(frame, rebuiltv1);//, maskFG);
            psnr_fullrebuilt = getPSNR(frame, full_rebuilt);
            stringstream psnr_rebuilt_stream;
            psnr_rebuilt_stream << fixed << setprecision(2) << psnr_rebuilt;
            stringstream psnr_fullrebuilt_stream;
            psnr_fullrebuilt_stream << fixed << setprecision(2) << psnr_fullrebuilt;

            frameCells = ShowManyImages("Result", 6,
                                        string("F"), frame,
                                        string("F'=BG+FG PSNR[F, F']: ") + psnr_rebuilt_stream.str(), rebuiltv1,
                                        string("F''=BG+FG+N PSNR[F, F'']: ") + psnr_fullrebuilt_stream.str(),
                                        full_rebuilt,
                                        string("BG[idx=") + to_string((int) floor(frameIdx / N)) + "]", frameBG,
                                        string("FG[thr=") + to_string(thr1) + "]", frameFG,
                                        string("N"), frameBGU);

            static VideoWriter originVid(originFilepath, CV_FOURCC('M', 'J', 'P', 'G'), fps, frame.size(), true);
            static VideoWriter bgVid(bgFilepath, CV_FOURCC('M', 'J', 'P', 'G'), fps, frame.size(), true);
            static VideoWriter motionVid(motionFilepath, CV_FOURCC('M', 'J', 'P', 'G'), fps, frame.size(), true);
            static VideoWriter rebuiltVid(rebuiltFilepath, CV_FOURCC('M', 'J', 'P', 'G'), fps, frame.size(), true);

            originVid.write(frame);
            bgVid.write(frameBGU);
            motionVid.write(frameFG);
            rebuiltVid.write(rebuiltv1);

            static VideoWriter cellsVid(cellsFilepath, CV_FOURCC('M', 'J', 'P', 'G'), fps, frameCells.size(), true);
            cellsVid.write(frameCells);
            imshow("Results", frameCells);
        }

        imwrite("results/rebuilt/in" + to_string(frameIdx) + ".bmp", rebuiltv1);
        imwrite("results/rebuiltv2/in" + to_string(frameIdx) + ".bmp", rebuiltv2);
        imwrite("results/rebuiltv3/in" + to_string(frameIdx) + ".bmp", rebuiltv3);
        imwrite("results/bgu/in" + to_string(frameIdx) + ".bmp", frameBGU);
        imwrite("results/origin/in" + to_string(frameIdx) + ".bmp", frame);
        //imwrite("results/fg/in" + to_string(frameIdx) + ".bmp", frameFG);

        //static VideoWriter fullRebuiltVid(fullRebuiltFilepath, CV_FOURCC('M', 'J', 'P', 'G'), fps, frame.size(), true);

        //static VideoWriter maskVid(maskFilepath, CV_FOURCC('M', 'J', 'P', 'G'), fps, maskFG.size(), true);

        //DEBUG fullRebuiltVid.write(full_rebuilt);
        Mat maskFG_RGB;
        cvtColor(maskFG, maskFG_RGB, COLOR_GRAY2RGB);
        imwrite("results/mask/in" + to_string(frameIdx) + ".bmp", maskFG_RGB);
        //maskVid.write(maskFG_RGB);
        //imshow("maskFG", maskFG);
        imshow("rebuiltv1", rebuiltv1);
        //imshow("rebuiltv2", rebuiltv2);
        keyboard = waitKey(1);
    }

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
        new_frame = additionMat(base_frame, enh_frame);
        writer.write(new_frame);
        keyboard = waitKey(1);
    }
}
