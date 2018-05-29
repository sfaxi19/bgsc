OUT_DIR=$1
rm *.mkv
rm *.yuv
rm *.svc
rm *.264
H264AVCEncoderLibTestStatic -pf main.cfg -lqp 0 20 -rqp 0 20 -lqp 1  20 -rqp 1 20
H264AVCEncoderLibTestStatic -pf single.cfg -rqp 0 20

BitStreamExtractorStatic test.264 base.svc -sl 4
BitStreamExtractorStatic test.264 enh.svc -sl 9 
BitStreamExtractorStatic single.264 single.svc -sl 4

H264AVCDecoderLibTestStatic base.svc base.yuv
H264AVCDecoderLibTestStatic enh.svc enh.yuv
H264AVCDecoderLibTestStatic single.svc single.yuv

ffmpeg -f rawvideo -vcodec rawvideo -s 768x576 -r 15 -pix_fmt yuv420p -i base.yuv -c:v libx264 -preset ultrafast -qp 0 base.mkv
ffmpeg -f rawvideo -vcodec rawvideo -s 768x576 -r 15 -pix_fmt yuv420p -i enh.yuv -c:v libx264 -preset ultrafast -qp 0 enh.mkv
ffmpeg -f rawvideo -vcodec rawvideo -s 768x576 -r 15 -pix_fmt yuv420p -i single.yuv -c:v libx264 -preset ultrafast -qp 0 single.mkv
