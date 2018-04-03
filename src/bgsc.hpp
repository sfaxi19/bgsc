//
// Created by sfaxi19 on 14.03.18.
//

#ifndef BGSC_HPP
#define BGSC_HPP

void processVideo(char *videoFilename, int thr1, int thr2, int N = 100);

void processVideoRecon(char *basefile, char *enhancefile, char *output);

inline uchar clip(int value, int min, int max) {
    return static_cast<uchar>((value < min) ? min : ((value > max) ? max : value));
}

#endif //BGSC_HPP
