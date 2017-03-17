#include "pano_util.h"
double value[3];
Mat correct_imgs_bottom(Mat img1,Mat side_img)
{
	Rect r(0,0,img1.cols,img1.rows/3);
	Rect r2(0,side_img.rows/2,side_img.cols,side_img.rows/2);
	Mat side;
	if(side_img.type()==CV_8UC4)
		cvtColor(side_img,side,CV_BGRA2BGR);
	else
		side=side_img;
	Point2f offset=get_offset(img1(r),side_img(r2));
	 // get_brightness(side_img(Rect(0,side_img.rows/2-img1.rows/6,img1.cols,img1.rows/6)),img1(Rect(0,img1.rows*5/6,img1.cols,img1.rows/6)));
	 // img1=ajust_brightness(img1);
	cout<<"bottom_offset:"<<offset.x<<endl;
	if(offset.x>0)
		return remap_pano(img1,3,offset.x);
	else
		return remap_pano(img1,2,abs(offset.x));
}
Mat correct_imgs_top(Mat img1,Mat side_img)
{
	Rect r(0,0,img1.cols/10,img1.rows);
	Rect r2(0,0,side_img.cols,side_img.rows/2);
	Mat side;
	if(side_img.type()==CV_8UC4)
		cvtColor(side_img,side,CV_BGRA2BGR);
	else
		side=side_img;
	Point2f offset=get_offset(img1(r),side(r2));
	// get_brightness(side(Rect(0,side_img.rows/2,img1.cols,img1.rows/6)),img1(Rect(0,0,img1.cols,img1.rows/6)));
	// img1=ajust_brightness(img1);
	cout<<"top_offset:"<<offset.x<<endl;
	if(offset.x>0)
		return remap_pano(img1,2,offset.x);
	else
		return remap_pano(img1,3,abs(offset.x));
}


Mat remap_pano(Mat img,int flag,int offset)//roundToRect:0;y_flip:1;offset_left:2;offset_right:3;x_flip:4
{
	Mat src, dst; 
	Mat map_x, map_y;
	float center_x;
	float center_y;
	float L;
	float H;
	float k;
   /// Load the image
	k=1.12;
	src = img;
	L=src.cols;
	H=src.rows;
	center_y=src.rows/2.0f;
	center_x=src.cols/2.0f;
  /// Create dst, map_x and map_y with the same size as src:
	dst.create( src.size(), src.type() );
	map_x.create( src.size(), CV_32FC1 );
	map_y.create( src.size(), CV_32FC1 );
	for( int j = 0; j < src.rows; j++ )
		{ for( int i = 0; i < src.cols; i++ )
			{
				switch(flag)
				{
					case 0:
					if (1)
					{
						map_x.at<float>(j,i) = cosf(2.0f*M_PI/L*i+M_PI/k)*j+center_x-13;
						map_y.at<float>(j,i) = sinf(2.0f*M_PI/L*i+M_PI/k)*j+center_y+5;
					}

					break;
					case 1:
					map_x.at<float>(j,i) = i ;
					map_y.at<float>(j,i) = src.rows - j ;
					break;
					case 2:
					if(i<offset)
					{
						map_x.at<float>(j,i) = i+src.cols-offset ;
						map_y.at<float>(j,i) = j ;
					}
					else
					{
						map_x.at<float>(j,i) = i-offset ;
						map_y.at<float>(j,i) = j ;

					}
					break;
					case 3:
					if(i<(src.cols-offset))
					{
						map_x.at<float>(j,i) = i+offset ;
						map_y.at<float>(j,i) = j ;
					}
					else
					{
						map_x.at<float>(j,i) = i-src.cols+offset ;
						map_y.at<float>(j,i) = j ;

					}
					break;
					case 4:
					map_x.at<float>(j,i) = src.cols-i ;
					map_y.at<float>(j,i) =  j ;
					break;

         } // end of switch
     }
 }
    /// Update map_x & map_y. Then apply remap
 remap( src, dst, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0, 0) );
    //Rect r(0,0,L,H*3/8); 
 return dst;
}

Mat delete_repeat(Mat img)
{	Rect r1(0,0,img.cols/2,img.rows);
	Rect r2(img.cols/2,0,img.cols/2,img.rows);
		//cout<<"vector problem"<<endl;

		//cout<<"vector problem solved"<<endl;
	Mat pano;
	pano= stitch(img(r2),img(r1));
	return pano;

}

Mat remove_pole(Mat img1,Mat img2)
{	flip(img1,img1,-1);
	flip(img2,img2,-1);
	Rect s1(img1.cols*2/5,0,img1.cols*3/5,img1.rows);
	Rect s2(img2.cols*2/5,0,img2.cols*3/5,img2.rows);  
	vector<Mat> imgs;
	imgs.push_back(img1(s1));
	imgs.push_back(img2(s2));
	cout<<"Please wait..."<<endl;          
	Mat pano;
	pano=stitch(img1(s1),img2(s2));
	return pano;
}
Mat delete_black(Mat img)
{
	Mat img1=img;
	int black_left_end;
	int black_right_end;
	int height=img1.rows;
	int width=img1.cols;
	for(int x=0;x<width;++x)
		for(int y=height/2;y<height;++y)
			if(img1.at<Vec3b>(y,x)[0]!=0.0f)
			{ 
				black_left_end=x;
				x=width;
				y=height;
			}
					//cout<<black_left_end<<endl;
			for(int x=width;x>0;--x)
				for(int y=height/2;y>0;--y)
					if(img1.at<Vec3b>(y,x)[0]!=0.0f)
					{ 
						black_right_end=x;
						x=0;
						y=0;
					}
							//cout<<black_right_end<<endl	;
	// Mat dec=Mat(height,black_right_end-black_left_end,img1.depth);
					Rect 	r(black_left_end,0,black_right_end-black_left_end, height);
	// img1(r).convertTo(dec,dec.type(),1,0); 
					return img(r);
				}


Mat resize_top_bottom(Mat img,int width)//this have some problem
{	
	IplImage *src ;  
	IplImage *side; 
	IplImage *desc; 
	IplImage imgTmp = img;
	src = cvCloneImage(&imgTmp); 
	CvSize sz;
	sz.width=width;
	sz.height=(float)width/4.0f;
	desc=cvCreateImage(sz,src->depth,src->nChannels); 
	cvResize(src,desc,CV_INTER_CUBIC);
	Mat M(desc, true);
	return M;
}

Mat warp_feather(Mat src,int flag)//top:0;side:1;bottom:2
{
	float warp_size=(float)src.rows/6.0f;
	float feather_size=4.0f;
	int side_width=src.cols;
	int side_heigh=src.rows;
	float side_start=(float)side_heigh/2.0f-(float)side_width/12.0f;
	float side_end=(float)side_heigh/2.0f+(float)side_width/12.0f;
	float bottom_end=4.0f;
	int start =src.rows-4-feather_size/2.0f;
	if(src.type() != CV_8UC4)
		cvtColor(src,src,CV_BGR2BGRA);
	switch(flag)
	{
		case 0:
		for(int y=start;y<src.rows;++y)
			for(int x=0;x<src.cols;++x)
			{
				if(y<start+feather_size)
				{
					float alpha=1.0f-((float)(y-start))/feather_size;
					src.at<Vec4b>(y, x)[3] = (unsigned char)255.0f*alpha;
				}
				else
					src.at<Vec4b>(y,x)[3] =(unsigned char) 0;
			}
		//imwrite(argv[2],src);
			break;

			case 1:
			for(int y=0;y<src.rows;++y)
				for(int x=0;x<src.cols;++x)
				{
					if(y<(side_start-feather_size/2))
						src.at<Vec4b>(y,x)[3] =(unsigned char) 0;
					else if(y<(side_start+feather_size/2))
					{
						float alpha=1.0f-(float)(side_start+feather_size/2-y)/feather_size;
						src.at<Vec4b>(y,x)[3] =(unsigned char)255.0f*alpha;
					}
					else if(y>(side_end-feather_size/2)&&y<(side_end+feather_size/2))
					{
						float alpha=1.0f-(float)(y-side_end+feather_size/2)/feather_size;
						src.at<Vec4b>(y,x)[3] =(unsigned char)255.0f*alpha;
					}
					else if(y>side_end+feather_size/2)
						src.at<Vec4b>(y,x)[3] =(unsigned char) 0;
				}

		//imwrite(argv[2],src);
				break;
				case 2:
				for(int y=0;y<src.rows;++y)
					for(int x=0;x<src.cols;++x)
					{
						if(y<(bottom_end-feather_size/2))
							src.at<Vec4b>(y,x)[3] =(unsigned char) 0;
						else if(y<(bottom_end+feather_size/2))
						{
							float alpha=1.0f-(float)(bottom_end+feather_size/2-y)/feather_size;
							src.at<Vec4b>(y,x)[3] =(unsigned char)255.0f*alpha;
						}
					}
		//imwrite(argv[2],src);
					break;	

				}
				return src;
			}
			Mat merge_imgs(Mat img_top,Mat img_side,Mat img_bottom)
			{	float feather_size=50.0f;
				Mat src1=img_top;
	Mat src2=img_side;//read image with alpha channel
	Mat src3=img_bottom;
	int width=src1.cols;
	int height=src1.cols/2;
	Mat dec=Mat(height,width,CV_8UC4);
	int side_offset=height/2-src2.rows/2-50;
	int bottom_offset=height-src3.rows;
	for(int y=0;y<height;++y)
		for(int x=0;x<width;++x)
		{	
			if(y<((float)src1.rows/2.0f+(float)src1.cols/12.0f-150-feather_size)&&y>0)
			{
				dec.at<Vec4b>(y,x)[0]=src1.at<Vec4b>(y,x)[0];
				dec.at<Vec4b>(y,x)[1]=src1.at<Vec4b>(y,x)[1];
				dec.at<Vec4b>(y,x)[2]=src1.at<Vec4b>(y,x)[2];
				dec.at<Vec4b>(y,x)[3]=(unsigned char)255.0f;
			}
			else if(y<((float)src1.rows/2.0f+(float)src1.cols/12.0f-150+feather_size))
			{
				float alpha=(float)src1.at<Vec4b>(y,x)[3]/255.0f;
				dec.at<Vec4b>(y,x)[0]=src1.at<Vec4b>(y,x)[0]*alpha+src2.at<Vec4b>(y-side_offset,x)[0]*(1.0f-alpha);
				dec.at<Vec4b>(y,x)[1]=src1.at<Vec4b>(y,x)[1]*alpha+src2.at<Vec4b>(y-side_offset,x)[1]*(1.0f-alpha);
				dec.at<Vec4b>(y,x)[2]=src1.at<Vec4b>(y,x)[2]*alpha+src2.at<Vec4b>(y-side_offset,x)[2]*(1.0f-alpha);
				dec.at<Vec4b>(y,x)[3]=(unsigned char)255.0f;
			}
			else if(y<bottom_offset)
			{	
				dec.at<Vec4b>(y,x)[0]=src2.at<Vec4b>(y-side_offset,x)[0];
				dec.at<Vec4b>(y,x)[1]=src2.at<Vec4b>(y-side_offset,x)[1];
				dec.at<Vec4b>(y,x)[2]=src2.at<Vec4b>(y-side_offset,x)[2];
				dec.at<Vec4b>(y,x)[3]=src2.at<Vec4b>(y-side_offset,x)[3];
			}
			else if(y<=height)
				{	float alpha_b=(float)src3.at<Vec4b>(y-bottom_offset,x)[3]/255.0f;
					dec.at<Vec4b>(y,x)[0]=src3.at<Vec4b>(y-bottom_offset,x)[0]*alpha_b+src2.at<Vec4b>(y-side_offset,x)[0]*(1.0f-alpha_b);
					dec.at<Vec4b>(y,x)[1]=src3.at<Vec4b>(y-bottom_offset,x)[1]*alpha_b+src2.at<Vec4b>(y-side_offset,x)[1]*(1.0f-alpha_b);
					dec.at<Vec4b>(y,x)[2]=src3.at<Vec4b>(y-bottom_offset,x)[2]*alpha_b+src2.at<Vec4b>(y-side_offset,x)[2]*(1.0f-alpha_b);
					dec.at<Vec4b>(y,x)[3]=(unsigned char)255.0f;
				}
			}

			return dec;
		}
		int stitch_side(string dst_path)
		{
			stringstream s_stream;
			string dstFile=dst_path; 
			Mat imgs[15];    
			for(int i=1;i<15;++i)     
				{  s_stream<<"/home/yang/pano_pic/isp_out/cam"<<i<<".png";
			cout<<s_stream.str()<<endl;
			imgs[i]=imread(s_stream.str());
			s_stream.str("");          
			if (imgs[i].empty())          
			{              
				cout<<"Can't read image '"<<i<<"'\n";              
				system("pause");              
				return -1;           
			}             
		}
Point2f offset_images[14];//for storage the entire image offset
for(int i=1;i<14;i++)
	{   int offset_tmp;
		imgs[i+1]=adjust_side(imgs[i],imgs[i+1],&offset_tmp);
		offset_images[i].x=offset_images[i-1].x+offset_tmp;
	}
	Mat pano=imgs[1];
//Mat pano2=imgs[14];
	for(int i=1;i<14;i++)
	{
		pano=merge_images(pano,imgs[i+1],offset_images[i]);
	}

	imwrite(dstFile, pano); 
	cout<<"finished"<<endl;    
	return 0;
}

void get_brightness(Mat img1,Mat img2)
{   value[0]=0;
	value[1]=0;
	value[2]=0;
	// resize(img1,img1,Size(img2.cols/4,img2.rows/4),0,0,CV_INTER_CUBIC);
	// resize(img2,img2,Size(img2.cols/4,img2.rows/4),0,0,CV_INTER_CUBIC);
	for(int i=0;i<img1.cols;++i)
		for(int j=0;j<img1.rows;++j)
			{   value[0]+=(img1.at<Vec3b>(j,i)[0]-img2.at<Vec3b>(j,i)[0]);
				value[1]+=(img1.at<Vec3b>(j,i)[1]-img2.at<Vec3b>(j,i)[1]);
				value[2]+=(img1.at<Vec3b>(j,i)[2]-img2.at<Vec3b>(j,i)[2]);

            // img1_value+=(img1.at<Vec3b>(j,i)[0]+img1.at<Vec3b>(j,i)[1]+img1.at<Vec3b>(j,i)[2]);
            // img2_value+=(img2.at<Vec3b>(j,i)[0]+img2.at<Vec3b>(j,i)[1]+img2.at<Vec3b>(j,i)[2]);
			}
     // img1_value/=(img1.cols*img1.rows*3);
     // img2_value/=(img1.cols*img1.rows*3);
			value[0]/=(img1.cols*img1.rows);
			value[1]/=(img1.cols*img1.rows);
			value[2]/=(img1.cols*img1.rows);

		}
Mat ajust_brightness(Mat img)
		{
// float values[3];
// values[0]=value;
			for(int i=0;i<img.cols;++i)
				for(int j=0;j<img.rows;++j)
				{   
					img.at<Vec3b>(j,i)[0]=saturate_cast<uchar>(img.at<Vec3b>(j,i)[0]+value[0]);
					img.at<Vec3b>(j,i)[1]=saturate_cast<uchar>(img.at<Vec3b>(j,i)[1]+value[1]);
					img.at<Vec3b>(j,i)[2]=saturate_cast<uchar>(img.at<Vec3b>(j,i)[2]+value[2]);
				}
				return img;
}
void get_brightness_foralpha(Mat img1,Mat img2)
{   value[0]=0;
	value[1]=0;
	value[2]=0;
	// resize(img1,img1,Size(img2.cols/4,img2.rows/4),0,0,CV_INTER_CUBIC);
	// resize(img2,img2,Size(img2.cols/4,img2.rows/4),0,0,CV_INTER_CUBIC);
	for(int i=0;i<img1.cols;++i)
		for(int j=0;j<img1.rows;++j)
			{   value[0]+=(img1.at<Vec4b>(j,i)[0]-img2.at<Vec4b>(j,i)[0]);
				value[1]+=(img1.at<Vec4b>(j,i)[1]-img2.at<Vec4b>(j,i)[1]);
				value[2]+=(img1.at<Vec4b>(j,i)[2]-img2.at<Vec4b>(j,i)[2]);

            // img1_value+=(img1.at<Vec3b>(j,i)[0]+img1.at<Vec3b>(j,i)[1]+img1.at<Vec3b>(j,i)[2]);
            // img2_value+=(img2.at<Vec3b>(j,i)[0]+img2.at<Vec3b>(j,i)[1]+img2.at<Vec3b>(j,i)[2]);
			}
     // img1_value/=(img1.cols*img1.rows*3);
     // img2_value/=(img1.cols*img1.rows*3);
			value[0]/=(img1.cols*img1.rows);
			value[1]/=(img1.cols*img1.rows);
			value[2]/=(img1.cols*img1.rows);

		}
Mat ajust_brightness_foralpha(Mat img)
		{
// float values[3];
// values[0]=value;
			for(int i=0;i<img.cols;++i)
				for(int j=0;j<img.rows;++j)
				{   
					img.at<Vec4b>(j,i)[0]=saturate_cast<uchar>(img.at<Vec4b>(j,i)[0]+value[0]);
					img.at<Vec4b>(j,i)[1]=saturate_cast<uchar>(img.at<Vec4b>(j,i)[1]+value[1]);
					img.at<Vec4b>(j,i)[2]=saturate_cast<uchar>(img.at<Vec4b>(j,i)[2]+value[2]);
				}
				return img;

			}
Mat adjust_side(Mat img1,Mat img2,int* i)
			{
				Mat src1=img1;
				Mat src2=img2;
				if (src1.type() != CV_8UC3) 
				{
					cvtColor(src1, src1, CV_BGRA2BGR);
					cvtColor(src2, src2, CV_BGRA2BGR);
				}
				Rect r(0,0,src2.cols/4,src2.rows);
				Point2f offset;
				offset=get_offset(src1,src2(r));
				*i=offset.x;
				Rect r1(offset.x,0,src1.cols-offset.x,src1.rows);
				Rect r2(0,0,src1.cols-offset.x,src1.rows);
				get_brightness(src1(r1),src2(r2));
				return ajust_brightness(src2);
			}



Mat stitch(Mat img1,Mat img2)
			{ 
				int offset_tmp;
				Point2f offset;
				adjust_side(img1,img2,&offset_tmp);
				offset.x=offset_tmp;
				return multiBandBlendAdjustment(img1,img2,offset);

			}
Point2f get_offset(Mat image01,Mat image02)
			{
				Mat image1, image2;
				cvtColor(image01, image1, CV_RGB2GRAY);
				cvtColor(image02, image2, CV_RGB2GRAY);

    //提取特征点    
    // SiftFeatureDetector siftDetector(800);  // 海塞矩阵阈值  
    // vector<KeyPoint> keyPoint1, keyPoint2;
    // siftDetector.detect(image1, keyPoint1);
    // siftDetector.detect(image2, keyPoint2);
    SurfFeatureDetector surfDetector(800);  // 海塞矩阵阈值  
    vector<KeyPoint> keyPoint1, keyPoint2;
    surfDetector.detect(image1, keyPoint1);
    surfDetector.detect(image2, keyPoint2);

    //特征点描述，为下边的特征点匹配做准备    
    //SiftDescriptorExtractor siftDescriptor;
    SurfDescriptorExtractor surf;
    Mat imageDesc1, imageDesc2;
    // siftDescriptor.compute(image1, keyPoint1, imageDesc1);
    // siftDescriptor.compute(image2, keyPoint2, imageDesc2);
    surf.compute(image1, keyPoint1, imageDesc1);
    surf.compute(image2, keyPoint2, imageDesc2);

    //获得匹配特征点，并提取最优配对     
    FlannBasedMatcher matcher;
    vector<DMatch> matchePoints;
    matcher.match(imageDesc1, imageDesc2, matchePoints, Mat());
    if (matchePoints.size() < 10)
    {  
    	Point2f no_fit;
    	return no_fit;
    }
    matchePoints.erase(matchePoints.begin()+150,matchePoints.end());  
    sort(matchePoints.begin(), matchePoints.end()); //特征点排序，opencv按照匹配点准确度排序    
    //获取排在前N个的最优匹配特征点  
    vector<Point2f> imagePoints1, imagePoints2;
    for (int i = 0; i<10; i++)
    {
    	imagePoints1.push_back(keyPoint1[matchePoints[i].queryIdx].pt);
    	imagePoints2.push_back(keyPoint2[matchePoints[i].trainIdx].pt);
    }

    //获取图像1到图像2的投影映射矩阵，尺寸为3*3  
    Mat homo = findHomography(imagePoints1, imagePoints2, CV_RANSAC);
    Mat adjustMat = (Mat_<double>(3, 3) << 1.0, 0, image01.cols, 0, 1.0, 0, 0, 0, 1.0);//向后偏移image01.cols矩阵
    Mat adjustHomo = adjustMat*homo;//矩阵相乘，先偏移

    //获取最强配对点（就是第一个配对点）在原始图像和矩阵变换后图像上的对应位置，用于图像拼接点的定位  
    Point2f originalLinkPoint, targetLinkPoint, basedImagePoint;
    Point2f offset;
    // for(int i=0;i<4;++i)
    // {
    // originalLinkPoint = keyPoint1[matchePoints[i].queryIdx].pt;
    // basedImagePoint = keyPoint2[matchePoints[i].trainIdx].pt;
    // offset+=abs(basedImagePoint.x-originalLinkPoint.x)*(float)(4.0f-i)/10.0f;
    // }
    originalLinkPoint = keyPoint1[matchePoints[0].queryIdx].pt;
    basedImagePoint = keyPoint2[matchePoints[0].trainIdx].pt;
    offset.x=floor(originalLinkPoint.x-basedImagePoint.x);
    offset.y=floor(originalLinkPoint.y-basedImagePoint.y);
    
    return offset;
    // cout<<originalLinkPoint.x<<endl;
    // cout<<basedImagePoint.x<<endl;
    // cout<<offset_length<<endl;
    // cout<<image01.cols<<endl;
    // Rect des(0,0,offset_length,image02.rows);
    // if(offset_length>image02.cols)
    //  return image02;
    // else
    // return image02(des);
}  
Mat merge_images(Mat img1,Mat img2,Point2f offset)
{   
	Mat src1=img1;
	Mat src2=img2;
	int offset_y=abs(offset.y);
	int width=src1.cols-offset.x;
	Mat dec(img1.rows-2*offset_y,(img1.cols+img2.cols-width),CV_8UC3);
    // Mat dec(4000,4000,img1.type())
	int side_offset=offset.x;
	for(int x=0;x<dec.cols;++x)
		for(int y=0;y<dec.rows;++y)
		{
			if(x<(offset.x))
			{  
				dec.at<Vec3b>(y,x)[0]=src1.at<Vec3b>(y+offset_y,x)[0];
				dec.at<Vec3b>(y,x)[1]=src1.at<Vec3b>(y+offset_y,x)[1];
				dec.at<Vec3b>(y,x)[2]=src1.at<Vec3b>(y+offset_y,x)[2];
			}
			else if(x<offset.x+100)
			{   
				float alpha=1.0f-((float)(x-offset.x))/100;
				dec.at<Vec3b>(y,x)[0]=src1.at<Vec3b>(y+offset_y,x)[0]*alpha+src2.at<Vec3b>(y,x-side_offset)[0]*(1.0f-alpha);
				dec.at<Vec3b>(y,x)[1]=src1.at<Vec3b>(y+offset_y,x)[1]*alpha+src2.at<Vec3b>(y,x-side_offset)[1]*(1.0f-alpha);
				dec.at<Vec3b>(y,x)[2]=src1.at<Vec3b>(y+offset_y,x)[2]*alpha+src2.at<Vec3b>(y,x-side_offset)[2]*(1.0f-alpha);
			}
			else 
			{   
				dec.at<Vec3b>(y,x)[0]=src2.at<Vec3b>(y,x-side_offset)[0];
				dec.at<Vec3b>(y,x)[1]=src2.at<Vec3b>(y,x-side_offset)[1];
				dec.at<Vec3b>(y,x)[2]=src2.at<Vec3b>(y,x-side_offset)[2];
			}

		}
		cout<<"merge_images.."<<endl;
		return dec;

	}
Mat multiBandBlendAdjustment(Mat image1,Mat image2,Point2f offset)
	{
		Mat final_img(image2.rows,image2.cols+offset.x,image2.type());
		Rect r1(offset.x-100,0,image1.cols+100-offset.x,image1.rows);
		Rect r2(0,0,offset.x-100,image1.rows);
		Rect r3(offset.x-100,0,final_img.cols+100-offset.x,final_img.rows);
		image1(r2).copyTo(final_img(r2));
		multiBandBlend(image1(r1),image2).copyTo(final_img(r3));
		return final_img;

	}
	Mat multiBandBlendAdjustment2(Mat image1,Mat image2)
	{
		Mat final_img(image1.rows+image2.rows-93,image2.cols,image2.type());
		Rect r1(0,image1.rows-100,image1.cols,100);
		Rect r2(0,0,image2.cols,100);
		Rect r3(0,image1.rows-100,final_img.cols,107);
		image1(Rect(0,0,image1.cols,image1.rows-100)).copyTo(final_img(Rect(0,0,image1.cols,image1.rows-100)));
		image2(Rect(0,100,image1.cols,image2.rows-100)).copyTo(final_img(Rect(0,image1.rows+7,image1.cols,image2.rows-100)));
		cvtColor(final_img,final_img,CV_RGBA2RGB);
		multiBandMergeImages2(image1(r1),image2(r2)).copyTo(final_img(r3));
		return final_img;

	}
Mat multiBandBlend(Mat image1,Mat image2)
	{
		Mat img_mask1=Mat::zeros(image1.size(),CV_8UC1);
		Mat img_mask2=Mat::ones(image2.size(),CV_8UC1);
		Mat img_mask=Mat::ones(image2.size()*2,CV_8UC1);
		float warp_offset=image1.cols-100;
		Rect r1(0,0,100+warp_offset/2,img_mask1.rows);
		Rect r2(0,0,warp_offset/2,img_mask1.rows);
		img_mask1(r1)=Mat::ones(r1.size(),CV_8UC1);
		img_mask2(r2)=Mat::zeros(r2.size(),CV_8UC1);
		int max_BINARY_value = 256;  
		threshold(img_mask1, img_mask1,0, max_BINARY_value, THRESH_BINARY); 
		threshold(img_mask2, img_mask2,0, max_BINARY_value, THRESH_BINARY); 
		MultiBandBlender blender;
    //&blender=Blender::createDefault(Blender::MULTI_BAND);
    // vector<Point> vp;
    // vector<Size> vs;
    // vp.push_back(Point(0,0));
    // //vp.push_back(Point(offset.x,0));
    // vs.push_back(Size(image1.cols,image1.rows));
    //vs.push_back(Size(image2.cols,image2.rows));
		Rect s(0,0,100+image2.cols,image2.rows);
		blender.prepare(s);
    // cout<<"begining feeding"<<endl;
		blender.feed(image1,img_mask1,Point(0,0));
		blender.feed(image2,img_mask2,Point(100,0));
		Mat image;
		blender.blend(image,img_mask);
    // if(! image.data ) // Check for invalid input
    // {
    //     cout << "Could not open or find the image" << std::endl ;
    //     return -1;
    // }
    // int threshold_value = atoi(argv[2]);  

	// image.copyTo(img_masked, img_mask);
		return image;
 // Wait for a keystroke in the window

	}
Mat multiBandMergeImages(Mat top,Mat side,Mat bottom)
	{	
		float width=side.cols;
		cvtColor(side,side,CV_RGBA2RGB);
		side.convertTo(side,CV_8UC3);
		Mat top_mask=Mat::zeros(top.size(),CV_8UC1);
		Mat side_mask=Mat::zeros(side.size(),CV_8UC1);
		Mat bottom_mask=Mat::zeros(bottom.size(),CV_8UC1);
		Mat final_mask=Mat::ones(side.cols,side.cols,CV_8UC1);
		Rect r1(0,0,width,width/6+50);
		Rect r2(0,side.rows/2-width/12+80,width,width/6);
		Rect r3(0,bottom.rows-width/6-100,width,width/6+100);
		top_mask(r1)=Mat::ones(r1.size(),CV_8UC1);
		side_mask(r2)=Mat::ones(r2.size(),CV_8UC1);
		bottom_mask(r3)=Mat::ones(r3.size(),CV_8UC1);
		int max_BINARY_value = 256;  
		threshold(top_mask, top_mask,0, max_BINARY_value, THRESH_BINARY); 
		threshold(side_mask, side_mask,0, max_BINARY_value, THRESH_BINARY);  
		threshold(bottom_mask, bottom_mask,0, max_BINARY_value, THRESH_BINARY); 
		//cout<<"mask ready"<<endl;

		MultiBandBlender blender;
		Rect s(0,0,width,width/2+50);
		blender.prepare(s);
    // cout<<"begining feeding"<<endl;

		blender.feed(top,top_mask,Point(0,0));
		blender.feed(side,side_mask,Point(0,width/6-side.rows/2+width/12-40));
		blender.feed(bottom,bottom_mask,Point(0,width/3-bottom.rows+width/6+50));
		//cout<<"feeding images"<<endl;
		Mat image;
		blender.blend(image,final_mask);
		return image;
	}
Mat multiBandMergeImages2(Mat top,Mat bottom)
	{	
		float width=top.cols;
		
		
		Mat top_mask=Mat::zeros(top.size(),CV_8UC1);
		
		Mat bottom_mask=Mat::zeros(bottom.size(),CV_8UC1);
		Mat final_mask=Mat::ones(top.cols,top.cols,CV_8UC1);
		Rect r1(0,0,width,top.rows-45);
		Rect r2(0,45,width,bottom.rows-45);
		// Rect r3(0,bottom.rows-width/6-100,width,width/6+100);
		top_mask(r1)=Mat::ones(r1.size(),CV_8UC1);
		bottom_mask(r2)=Mat::ones(r2.size(),CV_8UC1);
		// bottom_mask(r3)=Mat::ones(r3.size(),CV_8UC1);
		int max_BINARY_value = 256;  
		threshold(top_mask, top_mask,0, max_BINARY_value, THRESH_BINARY); 
		threshold(bottom_mask, bottom_mask,0, max_BINARY_value, THRESH_BINARY); 
		cout<<"mask ready"<<endl;
		if(top.type()==CV_8UC4)
		{
			cvtColor(top,top,CV_RGBA2RGB);
		}
		if(bottom.type()==CV_8UC4)
		{
			cvtColor(bottom,bottom,CV_RGBA2RGB);
		}
		MultiBandBlender blender;
		Rect s(0,0,width,top.rows+bottom.rows-93);
		blender.prepare(s);
    // cout<<"begining feeding"<<endl;

		blender.feed(top,top_mask,Point(0,0));
		blender.feed(bottom,bottom_mask,Point(0,top.rows-93));
		cout<<"feeding images"<<endl;
		Mat image;
		blender.blend(image,final_mask);
		return image;
	}
Mat roundToRect(Mat src)
	{ 
		Mat dst;
		const float dTheta = 2.0f * M_PI / float(src.cols);
		Mat warpMat = Mat(src.rows, src.cols, CV_32FC2);
		for (int i = 0; i < src.cols; ++i) {
			const float theta = i * dTheta;
			for (int j = 0; j < src.rows; ++j) {
				const float r = j / float(src.rows);
				const float rPix = 781.0f * r;
				const float srcX = 1024 + cosf(theta) * rPix;
				const float srcY = 1024 + sinf(theta) * rPix;
				warpMat.at<Point2f>(j, i) = Point2f(srcX, srcY);
			}
		}
		remap( src, dst, warpMat, Mat(), CV_INTER_LINEAR, BORDER_CONSTANT );

		return dst;
	}
void extendedMat(const Mat fisheyeSpherical,const Mat croppedSideSpherical,Mat& extendedfisheye,Mat& extendedside)
	{
		const float kExtendFrac = 1.2f;
		const int extendedWidth = float(fisheyeSpherical.cols) * kExtendFrac;
		Mat extendedSideSpherical(Size(extendedWidth, fisheyeSpherical.rows), CV_8UC4);
		Mat extendedFisheyeSpherical(Size(extendedWidth, fisheyeSpherical.rows),  CV_8UC4);
		for (int y = 0; y < extendedSideSpherical.rows; ++y) {
			for (int x = 0; x < extendedSideSpherical.cols; ++x) {
				extendedSideSpherical.at<Vec4b>(y, x) =
				croppedSideSpherical.at<Vec4b>(y, x % fisheyeSpherical.cols);
				extendedFisheyeSpherical.at<Vec4b>(y, x) =
				fisheyeSpherical.at<Vec4b>(y, x % fisheyeSpherical.cols);
			}
		}
		extendedSideSpherical.copyTo(extendedside);	
		extendedFisheyeSpherical.copyTo(extendedfisheye);
		
	}