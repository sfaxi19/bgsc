if test "$#" -ne 2; then
    echo "Illegal number of parameters"
    echo "Usages:"
    echo "arg1: path to input images;"
    echo "arg2: output path."
    exit 1
fi

cd $1
python3 psnr_graph.py origin $2
python3 psnr_roi.py origin $2 check_video.avi mask.mkv

python3 psnr_graph.py rebuilt $2
python3 psnr_roi.py rebuilt $2 check_video.avi mask.mkv
