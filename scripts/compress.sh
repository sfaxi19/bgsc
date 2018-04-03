rm compress/*
#ffmpeg -i origin.mkv -c:v libx264 -preset veryslow compress/origin.mkv
#ffmpeg -i bg.mkv -c:v libx264 -preset veryslow compress/bg.mkv
#ffmpeg -i motion.mkv -c:v libx264 -preset veryslow compress/motion.mkv
#ffmpeg -i rebuilt.mkv -c:v libx264 -preset veryslow compress/rebuilt.mkv

ffmpeg -i rebuilt/in%d.bmp -vcodec libx264 -preset slow -pix_fmt yuv420p compress/rebuilt.mkv
ffmpeg -i bgu/in%d.bmp -vcodec libx264 -preset slow -pix_fmt yuv420p compress/bgu.mkv
ffmpeg -i origin/in%d.bmp -vcodec libx264 -preset slow -pix_fmt yuv420p compress/origin.mkv

ffmpeg -i rebuilt/in%d.bmp -vcodec libx264 -preset slow -pix_fmt yuv420p -intra compress/rebuilt_intra.mkv
ffmpeg -i bgu/in%d.bmp -vcodec libx264 -preset slow -pix_fmt yuv420p -intra compress/bgu_intra.mkv
ffmpeg -i origin/in%d.bmp -vcodec libx264 -preset slow -pix_fmt yuv420p -intra compress/origin_intra.mkv
