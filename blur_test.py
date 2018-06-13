import cv2
import numpy as np
#from matplotlib import pyplot as plt
img = cv2.imread('star.png')
#kernel = np.ones((5,5),np.float32)/25
dst1 = cv2.blur(img, (5,5))
dst2 = cv2.GaussianBlur(img, (5,5), 0)
#imf = np.float32(img)/255.0
#freq = cv2.dct(imf)
cv2.namedWindow('Origin', cv2.WINDOW_NORMAL)
cv2.resizeWindow('Origin', 600,600)

cv2.namedWindow('Blurred', cv2.WINDOW_NORMAL)
cv2.resizeWindow('Blurred', 600,600)

cv2.namedWindow('Gaussian', cv2.WINDOW_NORMAL)
cv2.resizeWindow('Gaussian', 600,600)

cv2.imshow("Origin", img)
cv2.imshow("Blurred", dst1)
cv2.imshow("Gaussian", dst2)
#cv2.imshow("Freq",freq)
cv2.imwrite("blured.png",dst1)
cv2.imwrite("gaussian.png",dst2)
cv2.waitKey(0)

