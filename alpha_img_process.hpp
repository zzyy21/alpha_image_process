/* ****************************************************************
 * FilePath     : /alpha_img_process.hpp
 * Project Name : Alpha Image Process with OpenCV
 * Author       : zzyy21
 * Create Time  : 2020-07-06 20:31:17
 * Modifed by   : zzyy21
 * Last Modify  : 2020-07-07 20:27:44
 * Description  : Processing images with Alpha channel
 * Revision     : 
 * **************************************************************** */


#ifndef ALPHA_IMG_PROCESS_HPP_
#define ALPHA_IMG_PROCESS_HPP_

#include <vector>

#include <opencv2/opencv.hpp>

cv::Mat appandLayer(const cv::Mat &dst, const cv::Mat &src,
                    int left = 0, int top = 0);

cv::Mat padImg(const cv::Mat& img, int width, int height, int left, int top);

cv::Mat alphaComposite4on4(const cv::Mat& dst, const cv::Mat& src);

cv::Mat alphaComposite4on3(const cv::Mat& dst, const cv::Mat& src);

cv::Mat img4to3NoAlpha(const cv::Mat& img);

cv::Mat img4to3Display(const cv::Mat& img, int backgroundType = 1);



// append a layer with alpha channel to a specific position on the background
// @param 1 - dst: background layer (3 or 4 channels)
// @param 2 - src: upper layer (4 channels)
// @param 3 - left: column position of the upper left corner of append layer on background
// @param 4 - top: row position of the upper left corner of append layer on background
cv::Mat appandLayer(const cv::Mat &dst, const cv::Mat &src, int left, int top) {
    cv::Mat srcPadded = padImg(src, dst.cols, dst.rows, left, top);
    cv::Mat out;
    if (src.channels() == 4) {
        out = alphaComposite4on4(dst, srcPadded);
    } 
    else if (src.channels() == 3) {
        out = alphaComposite4on3(dst, srcPadded);
    }
    return out;
}

// get a (width x height) image, and the original
// image appears at (left, top) of the target image.
// padded section will be (0, 0, 0, 0) in BGRA.
// @param 1 - img: original image
// @param 2 - width: width of target image
// @param 3 - height: height of target image
// @param 4 - left: column position of the upper left corner of the original image in the target
// @param 5 - top: row position of the upper left corner of the original image in the target
cv::Mat padImg(const cv::Mat& img, int width, int height, int left, int top) {
    cv::Mat imgPadded;
    cv::Scalar padPixel(0, 0, 0, 0);
    int padTop = top;
    int padBottom = height - img.rows - top;
    int padLeft = left;
    int padRight = width - img.cols - left;
    cv::copyMakeBorder(img, imgPadded, padTop, padBottom, padLeft, padRight, cv::BORDER_CONSTANT, padPixel);
    return imgPadded;
}

// alpha composite two 4 channel images in same size.
// @param 1 - dst: background layer (4 channels)
// @param 2 - src: upper layer (4 channels)
cv::Mat alphaComposite4on4(const cv::Mat& dst, const cv::Mat& src) {
    cv::Mat out = dst.clone();

    std::vector<cv::Mat> srcChannels;
    std::vector<cv::Mat> outChannels;
    split(src, srcChannels);
    split(out, outChannels);


    // ******** alpha compositing ********
    // src: upper layer
    // dst: background layer
    // out_A = src_A + dst_A(1 - src_A)
    // out_RGB = (src_RGBsrc_A + dst_RGBdst_A(1 - src_A)) / out_A
    // out_A = 0 --> out_RGB = 0
    // ***********************************

    // out_A <-- dst_A(1 - src_A)
    outChannels[3] = outChannels[3].mul(255.0 - srcChannels[3], 1 / 255.0);
    for (int i = 0; i < 3; i++) {
        // out_RGB <--- dst_RGBdst_A(1 - src_A)
        outChannels[i] = outChannels[i].mul(outChannels[3], 1 / 255.0);
        // out_RGB <--- src_RGBsrc_A + dst_RGBdst_A(1 - src_A)
        outChannels[i] += srcChannels[i].mul(srcChannels[3], 1 / 255.0);
    }
    //out_A <--- src_A + dst_A(1 - src_A)
    outChannels[3] += srcChannels[3];
    for (int i = 0; i < 3; i++) {
        //out_RGB <--- (src_RGBsrc_A + dst_RGBdst_A(1 - src_A)) / out_A
        cv::divide(outChannels[i], outChannels[3], outChannels[i], 255.0);
    }

    cv::merge(outChannels, out);
    return out;
}

// alpha composite two 4 channel images in same size.
// @param 1 - dst: background layer (3 channels)
// @param 2 - src: upper layer (4 channels)
cv::Mat alphaComposite4on3(const cv::Mat& dst, const cv::Mat& src) {
    cv::Mat out = dst.clone();

    std::vector<cv::Mat> srcChannels;
    std::vector<cv::Mat> outChannels;
    split(src, srcChannels);
    split(out, outChannels);

    for (int i = 0; i < 3; i++) {
        // out_RGB <--- dst_RGB(1 - src_A)
        outChannels[i] = outChannels[i].mul(255.0 - srcChannels[3], 1 / 255.0);
        // out_RGB <--- src_RGBsrc_A + dst_RGB(1 - src_A)
        outChannels[i] += srcChannels[i].mul(srcChannels[3], 1 / 255.0);
    }

    cv::merge(outChannels, out);
    return out;
}

// convert 4 channel images (BGRA) to 3 channel images (BGR)
// directly discard the alpha channel information
// provided for convenience instead of using cv::cvtColor
// @param 1 - img: 4 channel image
cv::Mat img4to3NoAlpha(const cv::Mat& img) {
    cv::Mat out;
    cv::cvtColor(img, out, cv::COLOR_BGRA2BGR);
    return out;
}

// convert 4 channel images (BGRA) to 3 channel images (BGR)
// returns the BGR image that image with alpha channel display on a background
// @param 1 - img: 4 channel image
// @param 2 - backgroundType. (0: Black; 1: White)
cv::Mat img4to3Display(const cv::Mat& img, int backgroundType) {
    //int gray = backgroundType ? 255 : 0;
    // using alphaComposite4on3
    //cv::Mat back(img.rows, img.cols, CV_8UC3, cv::Scalar(gray, gray, gray));
    //cv::Mat out = alphaComposite4on3(back, img);

    std::vector<cv::Mat> imgChannels;
    split(img, imgChannels);

    for (int i = 0; i < 3; i++) {
        // out_RGB <--- src_RGBsrc_A + dst_RGB(1 - src_A)
        imgChannels[i] = imgChannels[i].mul(imgChannels[3], 1 / 255.0) + backgroundType * (255 - imgChannels[3]);
    }
    imgChannels.pop_back();

    cv::Mat out;
    cv::merge(imgChannels, out);
    return out;
}

#endif
