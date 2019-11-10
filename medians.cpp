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
  double mean_median[2][n/2];
  string labels[2] = {"Opencv", "Naive"};
  cap.read(frame);
  while (key != 'q' && n_frames < 24) {
    //
    // acquisition d'une trame video - librairie OpenCV
    cap.read(frame);
    // conversion en niveau de gris - librairie OpenCV
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	median = frame_gray.clone();

    int k = 1;
    /*struct timeval start, end;
    for (k; k < n; k += 2) {
      gettimeofday(&start, NULL);
      for(int i=0;i<50;i++) ;medianBlur(frame_gray, median, k);
      gettimeofday(&end, NULL);
      double e = ((double)end.tv_sec * 1000000.0 + (double)end.tv_usec);
      double s = ((double)start.tv_sec * 1000000.0 + (double)start.tv_usec);
      mean_median[0][k / 2] = (mean_median[0][k / 2] * n_frames + (e - s) / 50) / (n_frames+1);
    }*/
    
    k = 1;
    for (k; k < n; k += 2) {
      gettimeofday(&start, NULL);
      median_naif(frame_gray, median, k);
      gettimeofday(&end, NULL);
      double e = ((double)end.tv_sec * 1000000.0 + (double)end.tv_usec);
      double s = ((double)start.tv_sec * 1000000.0 + (double)start.tv_usec);
      mean_median[1][k / 2] = (mean_median[1][k / 2] * n_frames + (e - s)) / (n_frames+1);
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
  
    
  for(int i=1; i<2;i++){
  	cout << labels[i] << ":" << endl;
  	for(int j=0;j<n/2;j++)
  		cout << "\tk=" << j << " -> " << mean_median[i][j] << endl;
  	}

}



