#include <string>
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


static const float kBlurredFlowSigma            = 8.0f;
static const int kPyrMinImageSize               = 24;
static const int kPyrMaxLevels                  = 1000;
  static const float kGradEpsilon                 = 0.001f; // for finite differences
  static const float kUpdateAlphaThreshold        = 0.9f;   // pixels with alpha below this aren't updated by proposals
  static const int kMedianBlurSize                = 5;      // medianBlur max size is 5 pixels for CV_32FC2
  static const int kPreBlurKernelWidth            = 5;
  static const float kPreBlurSigma                = 0.25f;  // amount to blur images before pyramids
  static const int kFinalFlowBlurKernelWidth      = 3;
  static const float kFinalFlowBlurSigma          = 1.0f;   // blur that is applied to flow at the end after upscaling
  static const int kGradientBlurKernelWidth       = 3;
  static const float kGradientBlurSigma           = 0.5f;   // amount to blur image gradients
  static const int kBlurredFlowKernelWidth        = 15;     // for regularization/smoothing/diffusion
  static const float pyrScaleFactor                  = 0.9f;
  static const float smoothnessCoef                  = 0.001f;
  static const float verticalRegularizationCoef      = 0.01f;
  static const float horizontalRegularizationCoef    = 0.01f;
  static const float gradientStepSize                = 0.5f;
  static const float downscaleFactor                 = 0.5f;
  static const float directionalRegularizationCoef   = 0.0f;
  static const float MaxPercentage=0.5f;
  static const bool usePrevFlowTemporalRegularization=false;
  static const bool UseDirectionalRegularization=true;
  enum DirectionHint { UNKNOWN, RIGHT, DOWN, LEFT, UP };

template <typename T>
  inline T clamp(const T& x, const T& a, const T& b) {
  	return x < a ? a : x > b ? b : x;
  }
  float computePatchError(
  	const Mat& i0, const Mat& alpha0, int i0x, int i0y,
  	const Mat& i1, const Mat& alpha1, int i1x, int i1y);
  Mat visualizeFlowAsGreyDisparity(const Mat& flow);
  static  int computeSearchDistance() ;
  void adjustInitialFlow(
  	const Mat& I0,
  	const Mat& I1,
  	const Mat& alpha0,
  	const Mat& alpha1,
  	Mat& flow,
  	const DirectionHint hint);
  Rect computeSearchBox(DirectionHint hint);
  float computeIntensityRatio(
  	const Mat& lhs, const Mat& lhsAlpha,
  	const Mat& rhs, const Mat& rhsAlpha);
  void computeOpticalFlow(
  	const Mat& rgba0byte,
  	const Mat& rgba1byte,
  	const Mat& prevFlow,
  	const Mat& prevI0BGRA,
  	const Mat& prevI1BGRA,
  	Mat& flow,
  	DirectionHint hint);
  vector<Mat> buildPyramid(const Mat& src) ;
  void patchMatchPropagationAndSearch(
  	const Mat& I0,
  	const Mat& I1,
  	const Mat& alpha0,
  	const Mat& alpha1,
  	Mat& flow,
  	DirectionHint hint);
  void proposeFlowUpdate(
  	const Mat& alpha0,
  	const Mat& alpha1,
  	const Mat& I0,
  	const Mat& I1,
  	const Mat& I0x,
  	const Mat& I0y,
  	const Mat& I1x,
  	const Mat& I1y,
  	Mat& flow,
  	const Mat& blurredFlow,
  	float& currErr,
  	const int updateX, const int updateY,
  	const Point2f& proposedFlow) ;
  void lowAlphaFlowDiffusion(const Mat& alpha0, const Mat& alpha1, Mat& flow);

  Mat visualizeFlowAsGreyDisparity(const Mat& flow);
  float errorFunction(
  	const Mat& I0,
  	const Mat& I1,
  	const Mat& alpha0,
  	const Mat& alpha1,
  	const Mat& I0x,
  	const Mat& I0y,
  	const Mat& I1x,
  	const Mat& I1y,
  	const int x,
  	const int y,
  	const Mat& flow,
  	const Mat& blurredFlow,
  	const Point2f& flowDir);
  Point2f errorGradient(
  	const Mat& I0,
  	const Mat& I1,
  	const Mat& alpha0,
  	const Mat& alpha1,
  	const Mat& I0x,
  	const Mat& I0y,
  	const Mat& I1x,
  	const Mat& I1y,
  	const int x,
  	const int y,
  	const Mat& flow,
  	const Mat& blurredFlow,
  	const float currErr);
  static  float getPixBilinear32FExtend(const Mat& img, float x, float y);
  void generateNovelView(
  	const double shiftFromL,
  	Mat& outNovelViewMerged,
  	Mat& outNovelViewFromL,
  	Mat& outNovelViewFromR,
  	const Mat& imageL,
  	const Mat& imageR,
  	const Mat& flowRtoL,
  	const Mat& flowLtoR);
  Mat generateNovelViewSimpleCvRemap(
  	const Mat& srcImage,
  	const Mat& flow,
  	const double t);
  Mat combineNovelViews(
  	const Mat& imageL,
  	const float blendL,
  	const Mat& imageR,
  	const float blendR,
  	const Mat& flowLtoR,
  	const Mat& flowRtoL);
  template <typename V, typename T>
inline V lerp(const V x0, const V x1, const T alpha) {
  return x0 * (T(1) - alpha) + x1 * alpha;
}

template <typename T>
inline T lerp(const T x0, const T x1, const T alpha) {
  return x0 * (T(1) - alpha) + x1 * alpha;
}
Mat OpticalViewStitch(const Mat img1,const Mat img2);
Mat flattenLayersDeghostPreferBase(
    const Mat& bottomLayer,
    const Mat& topLayer);
