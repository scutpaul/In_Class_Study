# -*- coding: utf-8 -*-
import os
import cv2

#按照指定图像大小调整尺寸
def resize_image(image, height, width):
     top, bottom, left, right = (0, 0, 0, 0)

     #获取图像尺寸
     h, w, _ = image.shape

     #对于长宽不相等的图片，找到最长的一边
     longest_edge = max(h, w)

     #计算短边需要增加多上像素宽度使其与长边等长
     if h < longest_edge:
         dh = longest_edge - h
         top = dh // 2
         bottom = dh - top
     elif w < longest_edge:
         dw = longest_edge - w
         left = dw // 2
         right = dw - left
     else:
         pass

     #RGB颜色
     BLACK = [255, 255, 255]

     #给图像增加边界，是图片长、宽等长，cv2.BORDER_CONSTANT指定边界颜色由value指定
     constant = cv2.copyMakeBorder(image, top , bottom, left, right, cv2.BORDER_CONSTANT, value = BLACK)

     #调整图像大小并返回
     return cv2.resize(constant, (height, width))

def resizeImg(path_dir,shape):
    image = cv2.imread(path_dir)
    image = resize_image(image, shape, shape)
    fpath, fname = os.path.split(path_dir)
    cv2.imwrite('test/reshape/' + fname, image)
    return 'test/reshape/' + fname

