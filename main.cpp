#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include "psnr_ssim.hpp"
#include <iostream>
#include <cstdarg>
#include "bgsc.hpp"

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {
    cout << "Usage:" << endl
         << "./bgsc -s video.avi thr1 thr2" << endl
         << "\t\t for seporate video on FG BG and N layers" << endl
         << "./bgsc -r BG_layer.avi FG_layer.avi" << endl
         << "\t\t for reconstruct video from FG and BG layers" << endl
         << "./bgsc -psnr video1.avi video2.avi" << endl
         << "\t\t check PSNR between two video" << endl
         << "--------------------------------------------------------------------------" << endl
         << endl;
    if (argc < 3) {
        cerr << "Incorret input list" << endl;
        return EXIT_FAILURE;
    }
    std::string argMode = argv[1];
    if (argMode == "-psnr") {
        if (argc != 4) {
            cerr << "Incorret input args!" << endl;
            return EXIT_FAILURE;
        }
        VideoCapture video1(argv[2]);
        if (!video1.isOpened()) {
            cerr << "Unable to open video file: " << argv[2] << endl;
            exit(EXIT_FAILURE);
        }
        VideoCapture video2(argv[3]);
        if (!video2.isOpened()) {
            cerr << "Unable to open video file: " << argv[3] << endl;
            exit(EXIT_FAILURE);
        }
        double psnr = psnr_between_videos(video1, video2);
        cout << "PSNR = " << psnr << endl;
    }
    if (argMode == "-s") {
        if (argc != 5) {
            cerr << "Incorret input args!" << endl;
            return EXIT_FAILURE;
        }
        processVideo(argv[2], atoi(argv[3]), atoi(argv[4]));
    }
    if (argMode == "-r") {
        if (argc != 4) {
            cerr << "Incorret input args!" << endl;
            return EXIT_FAILURE;
        }
        processVideoRecon(argv[2], argv[3]);
    }
    destroyAllWindows();
    return EXIT_SUCCESS;
}
