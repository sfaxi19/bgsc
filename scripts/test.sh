python x264_process.py \
--in-file ../cmake-build-release/results/$1/in%d.bmp \
--out-dir x264_people/$1 \
--psnr-file ../cmake-build-release/results/origin/in%d.bmp \
--mask-file ../cmake-build-release/results/mask/in%d.bmp \
--qp-min 10 \
--qp-step 5
