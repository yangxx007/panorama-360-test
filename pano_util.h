#include "opencv2/highgui/highgui.hpp"
#include "opencv2/stitching/stitcher.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/nonfree/nonfree.hpp"      
#include "opencv2/legacy/legacy.hpp"
#include <opencv2/core/core.hpp>
#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace cv;
using namespace std;
using namespace detail;
Mat remap_pano(Mat img,int flag,int offset=0);//roundToRect:0;y_flip:1;offset_right:2;offset_left:3;x_flip:4
Mat remove_pole(Mat img1,Mat img2);
Mat delete_black(Mat img);
Mat resize_top_bottom(Mat img,int width);
Mat warp_feather(Mat img,int flag);//top:0;side:1;bottom:2
Mat merge_imgs(Mat img_top,Mat img_side,Mat img_bottom);
Mat delete_repeat(Mat img);
Mat correct_imgs_bottom(Mat img1,Mat side_img);
Mat roundToRect(Mat src);	
Mat correct_imgs_top(Mat img1,Mat side_img);
Mat stitch(Mat img1,Mat img2);
Mat merge_images(Mat img1,Mat img2,Point2f offset);
Point2f get_offset(Mat image01,Mat image02);
void get_brightness(Mat img1,Mat img2);
Mat ajust_brightness(Mat img);
void get_brightness_foralpha(Mat img1,Mat img2);
Mat ajust_brightness_foralpha(Mat img);
Mat adjust_side(Mat img1,Mat img2,int* i);
Mat multiBandBlend( Mat image1,Mat image2);
Mat multiBandBlendAdjustment(Mat image1,Mat image2,Point2f offset);
Mat multiBandMergeImages(Mat top,Mat side,Mat bottom);
void extendedMat(const Mat fisheyeSpherical,const Mat croppedSideSpherical,Mat& extendedfisheye,Mat& extendedside);
Mat multiBandMergeImages2(Mat top,Mat bottom);
Mat multiBandBlendAdjustment2(Mat image1,Mat image2);
