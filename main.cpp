#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include "psnr_ssim.hpp"
#include <iostream>
#include <cstdarg>
#include "bgsc.hpp"

using namespace cv;
using namespace std;

void help(){
    cout << "Usage:" << endl
         << "./bgsc -s video.avi thr1 thr2" << endl
         << "\t\t for seporate video on FG BG and N layers" << endl
         << "./bgsc -r BG_layer.avi FG_layer.avi" << endl
         << "\t\t for reconstruct video from FG and BG layers" << endl
         << "./bgsc -psnr video1.avi video2.avi [video_mask.avi]" << endl
         << "\t\t check PSNR between two video [The use of mask is supported]" << endl
         << "--------------------------------------------------------------------------" << endl
         << endl;
}

int main(int argc, char *argv[]) {

    if (argc < 3) {
        help();
        cerr << "Incorret input list" << endl;
        return 1;
    }
    std::string argMode = argv[1];
    if (argMode == "-psnr") {
        if (argc < 4) {
            help();
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
        double psnr = 0;
        if (argc > 4) {
            VideoCapture mask(argv[4]);
            psnr = psnr_between_videos(video1, video2, mask);
        } else {
            psnr = psnr_between_videos(video1, video2);
        }

        cout << "PSNR = " << psnr << endl;
    }
    if (argMode == "-s") {
        if (argc != 6) {
            help();
            cerr << "Incorret input args!" << endl;
            return EXIT_FAILURE;
        }
        processVideo(argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
    }
    if (argMode == "-r") {
        if (argc != 5) {
            help();
            cerr << "Incorret input args!" << endl;
            return EXIT_FAILURE;
        }
        processVideoRecon(argv[2], argv[3], argv[4]);
    }
    destroyAllWindows();
    return EXIT_SUCCESS;
}
