/* ****************************************************************
 * FilePath     : /test/mergetest.cpp
 * Project Name : Alpha Image Process with OpenCV
 * Author       : zzyy21
 * Create Time  : 2020-07-06 20:31:17
 * Modifed by   : zzyy21
 * Last Modify  : 2020-07-07 20:35:28
 * Description  : function test
 * Revision     : 
 * **************************************************************** */

#include "../alpha_img_process.hpp"

#include <iostream>

#include <opencv2/opencv.hpp>

int main(int argc, char const *argv[]) {
    cv::Mat img1 = cv::imread("2.png", -1);
    cv::Mat img2 = cv::imread("4.png", -1);
    cv::Mat img1bgr = cv::imread("2.png");
    cv::Mat alphaimg = cv::imread("FF00007F.png", -1);

    cv::Mat paddedImg2 = padImg(img2, 1920, 1080, 653, 413);
    cv::Mat output = appandLayer(img1, img2, 653, 413);

    cv::imwrite("output/appandLayer.png", output);
    cv::imwrite("output/padImg.png", paddedImg2);
    cv::imwrite("output/alphaComposite4on4.png", alphaComposite4on4(img1, paddedImg2));
    cv::imwrite("output/alphaComposite4on3.png", alphaComposite4on3(img1bgr, paddedImg2));
    cv::imwrite("output/img4to3NoAlpha.png", img4to3NoAlpha(alphaimg));
    cv::imwrite("output/img4to3Display_white.png", img4to3Display(alphaimg));
    cv::imwrite("output/img4to3Display_black.png", img4to3Display(alphaimg, 0));

    return 0;
}

