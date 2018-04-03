LOG_PATH=psnr_graph/$1/log
VIDEO_PATH=psnr_graph/$1/videos
#echo $LOG_PATH
#echo $VIDEO_PATH
mkdir -p $LOG_PATH
mkdir -p $VIDEO_PATH
ffmpeg -i $1/in%d.bmp -vcodec libx264 -y -b:v $2k -pix_fmt yuv420p $VIDEO_PATH/out$2k.mkv
../bgsc -psnr check_psnr.avi $VIDEO_PATH/out$2k.mkv | grep "PSNR = " > $LOG_PATH/$2k.txt
