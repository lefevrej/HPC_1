/*
 * Fichier source pour le projet d'unité
 *  INF-4101C
 *---------------------------------------------------------------------------------------------------
 * Pour compiler : g++ `pkg-config --cflags opencv4` projet_base.cpp `pkg-config
 *--libs opencv4` -o
 *projet
 *---------------------------------------------------------------------------------------------------
 * auteur : Eva Dokladalova 09/2015
 * modification : Eva Dokladalova 10/2017
 */

/*
 * Libraries stantards
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <thread>

/*
 * Libraries OpenCV "obligatoires"
 *
 */
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

/*
 * Définition des "namespace" pour évite cv::, std::, ou autre
 *
 */
using namespace std;
using namespace cv;
using std::cout;

/*
 * Some usefull defines
 * (comment if not used)
 */
#define PROFILE
#define VAR_KERNEL
#define N_ITER 100

#ifdef PROFILE
#include <time.h>
#include <sys/time.h>
#endif

void median_naif(Mat& frame, Mat& out, int k){
    int elt, med;
    for (int i = 0; i < frame.cols; i++) {
        for (int j = 0; j < frame.rows; j++) {
            auto hist = map<char, int>();
            int n_elt = 0;
            for (int ik = i - (k / 2 + 1); ik < i + k / 2; ik++) {
            	for (int jk = j - 1; jk < j + 2; jk++) {
                    if (ik >= 0 && ik < frame.cols && jk >= 0 && jk < frame.rows) {
                        hist[frame.at<char>(jk, ik)]++;
                        n_elt++;
                    }
                }
            }

            elt = 0;
            med = 0;
            for (auto& it : hist) {
                if (elt > n_elt / 2)
                    break;
                elt += it.second;
                med = it.first;
            }
            out.at<char>(j, i) = med;
        }
    }
}

uchar median(int* hist, int n_elt){
	int elt = 0;
	uchar med = 0;
	while(elt < n_elt/2){
		elt += hist[med];
		med++;
	}
	return med;
}

void median_cst(Mat frame, Mat out,int k) {
	int k_r = (k>>1);
	int column_hist[out.cols][255];
	int* hist= (int*)malloc(256*sizeof(int));

	for(int i=k_r+1; i<out.rows; i++){
		memset(hist,0,256*sizeof(int));
		
		for(int i_k=0; i_k<k;i_k++)
			for(int j_k=0; j_k<k;j_k++){				
				column_hist[j_k][frame.at<uchar>(i-k_r-1,j_k)]--;
				column_hist[j_k][frame.at<uchar>(i+k_r,j_k)]++;
				hist[frame.at<uchar>(i-k_r+i_k,j_k)]++;		
			}
		
		for(int j=k ; j < out.cols; j++){
			column_hist[j][frame.at<uchar>(i-k_r-1,j)]--;
			column_hist[j][frame.at<uchar>(i+k_r,j)]++;
			for(int r=0; r<k;++r){	
				hist[frame.at<uchar>(i-k_r+r,j-k)]--;	
				hist[frame.at<uchar>(i-k_r+r,j)]++;				
						
			}		
			out.at<uchar>(i, j-k_r-1) = median(hist,((k*k)>>1));
		}
	}
	free(hist);
}

void median_task(Mat frame, Mat out,int k, int start, int end) {
	int k_r = (k>>1);
	int column_hist[out.cols][255];
	int* hist= (int*)malloc(256*sizeof(int));

	for(int i=k_r+1+start; i<end; i++){
		memset(hist,0,256*sizeof(int));
		
		for(int i_k=0; i_k<k;i_k++)
			for(int j_k=0; j_k<k;j_k++){				
				column_hist[j_k][frame.at<uchar>(i-k_r-1,j_k)]--;
				column_hist[j_k][frame.at<uchar>(i+k_r,j_k)]++;
				hist[frame.at<uchar>(i-k_r+i_k,j_k)]++;		
			}
		
		for(int j=k ; j < out.cols; j++){
			column_hist[j][frame.at<uchar>(i-k_r-1,j)]--;
			column_hist[j][frame.at<uchar>(i+k_r,j)]++;
			for(int r=0; r<k;++r){	
				hist[frame.at<uchar>(i-k_r+r,j-k)]--;	
				hist[frame.at<uchar>(i-k_r+r,j)]++;				
						
			}		
			out.at<uchar>(i, j-k_r-1) = median(hist,((k*k)>>1));
		}
	}
	free(hist);
}

void median_thread(Mat &frame, Mat &out,int k) {
  int n = thread::hardware_concurrency();
  thread* threads[n];
  int band_width = frame.rows / n;

  threads[0] = new thread(median_task, frame, out,k, 1, band_width);
  for (int i = 1; i < n - 1; i++) {
    threads[i] = new thread(median_task, frame, out,k, i * band_width - 1-k, (i + 1) * band_width);
  }
  threads[n - 1] = new thread(median_task, frame, out,k, (n - 1) * band_width - 1-k, frame.rows);

  for (thread * t: threads)
    t -> join();
}

/*
 *
 *--------------- MAIN FUNCTION ---------------
 *
 */
int main(int argc, char** argv) {
struct timeval start, end;
  //----------------------------------------------
  // Video acquisition - opening
  //----------------------------------------------
  VideoCapture cap(
      0); // le numéro 0 indique le point d'accès à la caméra 0 => /dev/video0
  if (!cap.isOpened()) {
    cout << "Errore";
    return -1;
  }

  //----------------------------------------------
  // Déclaration des variables - imagesize
  // Mat - structure contenant l'image 2D niveau de gris
  // Mat3b - structure contenant l'image 2D en couleur (trois cannaux)
  //
  Mat3b frame;    // couleur
  Mat frame_gray; // niveau de gris
  Mat median;

  // variable contenant les paramètres des images ou d'éxécution

  unsigned char key = '0';

#define PROFILE

#ifdef PROFILE
// profiling / instrumentation libraries
#include <time.h>
#include <sys/time.h>
#endif

  //----------------------------------------------------
  // Création des fenêtres pour affichage des résultats
  // vous pouvez ne pas les utiliser ou ajouter selon ces exemple
  //
  // namedWindow("Video input", WINDOW_AUTOSIZE);
  namedWindow("Video gray levels", WINDOW_AUTOSIZE);
  namedWindow("Video median", WINDOW_AUTOSIZE);
  // placement arbitraire des  fenêtre sur écran
  // sinon les fenêtres sont superposée l'une sur l'autre
  // moveWindow("Video input", 10, 30);
  moveWindow("Video gray levels", 800, 30);
  moveWindow("Video median", 800, 500);

  // --------------------------------------------------
  // boucle infinie pour traiter la séquence vidéo
  //
  
  int n_frames = 0, n = 17;
  double e, s;
  double mean_median[4][n/2];
  string labels[4] = {"OpenCV", "Naive", "Constant time", "Thread"};
  cap.read(frame);
  while (key != 'q' && n_frames < 24) {
    //
    // acquisition d'une trame video - librairie OpenCV
    cap.read(frame);
    // conversion en niveau de gris - librairie OpenCV
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	median = frame_gray.clone();
	struct timeval start, end;
    int k = 1;

	for (k; k < n; k += 2) {
	  gettimeofday(&start, NULL);
	  for(int i=0;i<10;i++) medianBlur(frame_gray, median, k);
	  gettimeofday(&end, NULL);
	  double e = ((double)end.tv_sec * 1000000.0 + (double)end.tv_usec);
	  double s = ((double)start.tv_sec * 1000000.0 + (double)start.tv_usec);
	  mean_median[0][k / 2] = (mean_median[0][k / 2] * n_frames + (e - s) / 10) / (n_frames+1);
	}
    
    /*k = 1;   
    for (k; k < n; k += 2) {
      gettimeofday(&start, NULL);
      median_naif(frame_gray, median, k);
      gettimeofday(&end, NULL);
      double e = ((double)end.tv_sec * 1000000.0 + (double)end.tv_usec);
      double s = ((double)start.tv_sec * 1000000.0 + (double)start.tv_usec);
      mean_median[1][k / 2] = (mean_median[1][k / 2] * n_frames + (e - s) ) / (n_frames+1);
    }*/
    
    k = 1;
    for (k; k < n; k += 2) {
      gettimeofday(&start, NULL);
      for(int i=0;i<10;i++) median_cst(frame_gray, median, k);
      //medianBlur(frame_gray, median, k);
      gettimeofday(&end, NULL);
      double e = ((double)end.tv_sec * 1000000.0 + (double)end.tv_usec);
      double s = ((double)start.tv_sec * 1000000.0 + (double)start.tv_usec);
      mean_median[2][k / 2] = (mean_median[2][k / 2] * n_frames + (e - s) / 10) / (n_frames+1);
    }
    
	 for (k; k < n; k += 2) {
	  gettimeofday(&start, NULL);
	  for(int i=0;i<10;i++) median_thread(frame_gray, median, k);
	  gettimeofday(&end, NULL);
	  double e = ((double)end.tv_sec * 1000000.0 + (double)end.tv_usec);
	  double s = ((double)start.tv_sec * 1000000.0 + (double)start.tv_usec);
	  mean_median[3][k / 2] = (mean_median[3][k / 2] * n_frames + (e - s) / 10) / (n_frames+1);
	 }

    // -------------------------------------------------
    // visualisation
    // taille d'image réduite pour meuilleure disposition sur écran

    resize(frame_gray, frame_gray, Size(), 0.5, 0.5);
    resize(median, median, Size(), 1, 1);
    imshow("Video gray levels", frame_gray);
    imshow("Video median", median);
    key = waitKey(5);
  	n_frames++;
  }
  
    
  for(int i=0; i<5;i++){
  	cout << '\n'	 << labels[i] << ":" << endl;
  	for(int j=0;j<n/2;j++)
  		cout << "\tk=" << (j*2)+1 << " -> " << mean_median[i][j] << endl;
  	}

}



