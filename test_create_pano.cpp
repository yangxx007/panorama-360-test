#include "pano_util.h"
#include "testOpticalView.h"
#include <pthread.h>
#define NUM_THREADS 5
pthread_mutex_t sum_mutex;
void* opticalPrepareThread1(void* v)
{	
	std::vector<Mat>* mats=(std::vector<Mat> *)v;
	Mat final_side=(*mats)[0];
	Mat top_final=(*mats)[1];
	Mat top_extended,side_1;
	Rect r1(0,50,top_final.cols,top_final.rows);
	pthread_mutex_lock( &sum_mutex );
	extendedMat(top_final,final_side(r1),top_extended,side_1);
	imwrite("/home/yang/pano_pic/top_test.png",top_extended);
	pthread_mutex_unlock( &sum_mutex ); //释放锁，供其他线程使用  
	(*mats)[1] = OpticalViewStitch(top_extended,side_1);

}
void* opticalPrepareThread2(void* v)
{	std::vector<Mat>* mats=(std::vector<Mat> *)v;
	Mat final_side=(*mats)[0];
	Mat bottom_final=(*mats)[2];
	Mat bottom_extended,side_2;
	Rect r2(0,bottom_final.rows-50,bottom_final.cols,bottom_final.rows);
	pthread_mutex_lock( &sum_mutex );
	extendedMat(bottom_final,final_side(r2),bottom_extended,side_2);
	// extendedMat(bottom_final,final_side,bottom_extended,side_2);
	imwrite("/home/yang/pano_pic/bottom_extended.png",bottom_extended);
	pthread_mutex_unlock( &sum_mutex );
	Mat side_bottom_flip,bottom_flip;
	flip(side_2,side_bottom_flip,-1);
	flip(bottom_extended,bottom_flip,-1);
	Mat final_2 = OpticalViewStitch(bottom_flip,side_bottom_flip);
	flip(final_2,(*mats)[2],-1);
	
}
void* sidePrepareThread(void* v)
{	Mat* side = (Mat *)v;
	cout<<"dealing with side"<<endl;
	
	*side=delete_black(*side);
	// side=delete_repeat(side);
	// cvtColor(side,side,CV_RGB2RGBA);
}
void* topPrepareThread(void* v)
{
	std::vector<Mat>* mats=(std::vector<Mat> *)v;
	Mat side=(*mats)[3];
	Mat top=(*mats)[0];
	cout<<"dealing with top"<<endl;
	top=roundToRect(top);
	top=remap_pano(top,4);
	top=delete_black(top);
	top=resize_top_bottom(top,side.cols);
	top=correct_imgs_top(top,side);
	(*mats)[0]=warp_feather(top,0);

}
void* bottomPrepareThread(void* v)
{
	std::vector<Mat>* mats=(std::vector<Mat> *)v;
	Mat side=(*mats)[3];
	Mat bottom_1=(*mats)[1];
	Mat bottom_2=(*mats)[2];
	Mat bottom;
	cout<<"dealing with bottom"<<endl;
	bottom_1=roundToRect(bottom_1);
	bottom_2=roundToRect(bottom_2);
	bottom=remove_pole(bottom_1,bottom_2);
	bottom=delete_repeat(bottom);
	// imshow("bottom",bottom);
	// waitKey(1);
	bottom=remap_pano(bottom,4);//flip
	bottom=delete_black(bottom);
	bottom=resize_top_bottom(bottom,side.cols);
	bottom=correct_imgs_bottom(bottom,side);
	(*mats)[1]=warp_feather(bottom,2);

}
int main(int argc, char** argv)
{	Mat *top=new Mat;
	
	Mat *bottom_2=new Mat;
	Mat *bottom_1=new Mat;
	Mat *side =new Mat;
	*top = imread(argv[1],-1);
	*bottom_1 = imread(argv[2],-1);
	*bottom_2 = imread(argv[3],-1);
	*side =imread(argv[4],-1);//already stitched side img
	pthread_t tids[NUM_THREADS];
	pthread_create(&tids[0],NULL,sidePrepareThread,(void*)side);
	pthread_join(tids[0],NULL);
	vector<Mat>* matVectors=new std::vector<Mat>;
	matVectors->push_back(*top);
	matVectors->push_back(*bottom_1);
	matVectors->push_back(*bottom_2);
	matVectors->push_back(*side);

	
	delete top;
	delete bottom_2;
	delete bottom_1;

	


	pthread_create(&tids[1],NULL,bottomPrepareThread,(void*)matVectors);
	pthread_create(&tids[2],NULL,topPrepareThread,(void*)matVectors);
	pthread_join(tids[1],NULL);
	pthread_join(tids[2],NULL);
	
	cout<<"success!dealing with merge imgs"<<endl;
	
	Mat* side_final=new Mat;
	Mat* top_final=new Mat;
	Mat* bottom_final=new Mat;

	 *side_final=*side;
	 *top_final=(*matVectors)[0];
	 *bottom_final=(*matVectors)[1];

	 delete side;
	delete matVectors;
	Mat* final_side=new Mat;
	*final_side=Mat::zeros(Size((*side_final).cols,(*side_final).cols/2),CV_8UC4);
	Rect r_side(0,(*final_side).rows/2-(*side_final).rows/2,(*side_final).cols,(*side_final).rows);
	(*side_final).copyTo((*final_side)(r_side));


	std::vector<Mat> opticalMats;
	opticalMats.push_back(*final_side);
	opticalMats.push_back(*top_final);
	opticalMats.push_back(*bottom_final);


	delete top_final;
	delete bottom_final;
	delete side_final;
	delete final_side;


	pthread_create(&tids[3],NULL,opticalPrepareThread1,(void*)&opticalMats);
	pthread_join(tids[3],NULL);
	pthread_create(&tids[4],NULL,opticalPrepareThread2,(void*)&opticalMats);
	
	pthread_join(tids[4],NULL);
	
	Mat panorama_img= multiBandBlendAdjustment2(opticalMats[1],opticalMats[2]);
	imwrite (argv[5],panorama_img(Rect(0,0,panorama_img.cols/1.2f,panorama_img.rows)));
	cout<<"success !!!"<<endl;
	return 0;
}