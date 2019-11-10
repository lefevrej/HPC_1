/*
 * Fichier source pour le projet d'unité
 *  INF-4101C
 *---------------------------------------------------------------------------------------------------
 * Pour compiler : g++ `pkg-config --cflags opencv4` projet_base.cpp `pkg-config
 *--libs opencv4` -o
 *projet
 *---------------------------------------------------------------------------------------------------
 * auteur : Eva Dokladalova 09/2015
 * modification : Josselin Lefèvre 11/2019
 */

/*
 * Libraries stantards
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <thread>

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

void sobel0(Mat & frame, Mat & grad) {
  int ddepth = CV_16S;
  int scale = 1;
  int delta = 0;
  Mat grad_x;
  Mat grad_y;
  Mat abs_grad_y;
  Mat abs_grad_x;
  // calcul du gradient- librairie OpenCV
  /// Gradient Y
  Sobel(frame, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
  /// absolute value
  convertScaleAbs(grad_x, abs_grad_x);
  /// Gradient Y
  Sobel(frame, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
  /// absolute value
  convertScaleAbs(grad_y, abs_grad_y);
  /// Total Gradient (approximate)    
  addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
}

/*
 *
 *--------------- Poiteurs déroulés en i et j ---------------
 *
 */
void sobel5(Mat frame, Mat out) {
  int x, y;
  char a,
  x00, x01, x02, x03, x04,
  x10, x11, x12, x13, x14,
  x20, x21, x22, x23, x24,
  x30, x31, x32, x33, x34,
  x40, x41, x42, x43, x44;

  uchar * r1, * r2, * r3, * r4, * r5, * rout1, * rout2, * rout3;
  r4 = frame.ptr(0);
  r5 = frame.ptr(1);
  for (int i = 0; i < frame.rows - 3; i += 3) {
    r1 = r4;
    r2 = r5;
    r3 = frame.ptr(2 + i);
    r4 = frame.ptr(3 + i);
    r5 = frame.ptr(4 + i);
    rout1 = out.ptr(1 + i);
    rout2 = out.ptr(2 + i);
    rout3 = out.ptr(3 + i);
    for (int j = 0; j < frame.cols - 2; j += 3) {
      x00 = r1[j];
      x01 = r1[j + 1];
      x02 = r1[j + 2];
      x03 = r1[j + 3];
      x04 = r1[j + 4];

      x10 = r2[j];
      x11 = r2[j + 1];
      x12 = r2[j + 2];
      x13 = r2[j + 3];
      x14 = r2[j + 4];

      x20 = r3[j];
      x21 = r3[j + 1];
      x22 = r3[j + 2];
      x23 = r3[j + 3];
      x24 = r3[j + 4];

      x30 = r4[j];
      x31 = r4[j + 1];
      x32 = r4[j + 2];
      x33 = r4[j + 3];
      x34 = r4[j + 4];

      x40 = r5[j];
      x41 = r5[j + 1];
      x42 = r5[j + 2];
      x43 = r5[j + 3];
      x44 = r5[j + 4];

      x = -x00 - (x01 << 1) - x02 + x20 + (x21 << 1) + x22;
      y = -x00 + x02 - (x10 << 1) + (x12 << 1) - x20 + x22;

      a = (abs(x) + abs(y)) >> 1;
      rout1[1 + j] = a > 255 ? 255 : a;

      x = -x10 - (x11 << 1) - x12 + x30 + (x31 << 1) + x32;
      y = -x10 + x12 - (x20 << 1) + (x22 << 1) - x30 + x32;

      a = (abs(x) + abs(y)) >> 1;
      rout2[1 + j] = a > 255 ? 255 : a;

      x = -x20 - (x21 << 1) - x22 + x40 + (x41 << 1) + x42;
      y = -x20 + x22 - (x30 << 1) + (x32 << 1) - x40 + x42;

      a = (abs(x) + abs(y)) >> 1;
      rout3[1 + j] = a > 255 ? 255 : a;

      x = -x01 - (x02 << 1) - x03 + x21 + (x22 << 1) + x23;
      y = -x01 + x03 - (x11 << 1) + (x13 << 1) - x21 + x23;

      a = (abs(x) + abs(y)) >> 1;
      rout1[2 + j] = a > 255 ? 255 : a;

      x = -x11 - (x12 << 1) - x13 + x31 + (x32 << 1) + x33;
      y = -x11 + x13 - (x21 << 1) + (x23 << 1) - x31 + x33;

      a = (abs(x) + abs(y)) >> 1;
      rout2[2 + j] = a > 255 ? 255 : a;

      x = -x21 - (x22 << 1) - x23 + x41 + (x42 << 1) + x43;
      y = -x21 + x23 - (x31 << 1) + (x33 << 1) - x41 + x43;

      a = (abs(x) + abs(y)) >> 1;
      rout3[2 + j] = a > 255 ? 255 : a;

      x = -x02 - (x03 << 1) - x04 + x22 + (x23 << 1) + x24;
      y = -x02 + x04 - (x12 << 1) + (x14 << 1) - x22 + x24;

      a = (abs(x) + abs(y)) >> 1;
      rout1[3 + j] = a > 255 ? 255 : a;

      x = -x12 - (x13 << 1) - x14 + x32 + (x33 << 1) + x34;
      y = -x12 + x14 - (x22 << 1) + (x24 << 1) - x32 + x34;

      a = (abs(x) + abs(y)) >> 1;
      rout2[3 + j] = a > 255 ? 255 : a;

      x = -x22 - (x23 << 1) - x24 + x42 + (x43 << 1) + x44;
      y = -x22 + x24 - (x32 << 1) + (x34 << 1) - x42 + x44;

      a = (abs(x) + abs(y)) >> 1;
      rout3[3 + j] = a > 255 ? 255 : a;
    }
  }
}

/*
 *
 *--------------- Poiteurs déroulés en i ---------------
 *
 */				
void sobel4(Mat & frame, Mat & out) {
  int x, y;
  char a,
  x00, x01, x02,
  x10, x11, x12,
  x20, x21, x22,
  x30, x31, x32,
  x40, x41, x42;

  uchar * r1, * r2, * r3, * r4, * r5, * rout1, * rout2, * rout3;
  r4 = frame.ptr(0);
  r5 = frame.ptr(1);
  for (int i = 0; i < frame.rows - 3; i += 3) {
    r1 = r4;
    r2 = r5;
    r3 = frame.ptr(2 + i);
    r4 = frame.ptr(3 + i);
    r5 = frame.ptr(4 + i);
    rout1 = out.ptr(1 + i);
    rout2 = out.ptr(2 + i);
    rout3 = out.ptr(3 + i);
    for (int j = 0; j < frame.cols - 2; j++) {
      x00 = r1[j];
      x01 = r1[j + 1];
      x02 = r1[j + 2];

      x10 = r2[j];
      x11 = r2[j + 1];
      x12 = r2[j + 2];

      x20 = r3[j];
      x21 = r3[j + 1];
      x22 = r3[j + 2];

      x30 = r4[j];
      x31 = r4[j + 1];
      x32 = r4[j + 2];

      x40 = r5[j];
      x41 = r5[j + 1];
      x42 = r5[j + 2];

      x = -x00 - x01 * 2 - x02 + x20 + x21 * 2 + x22;
      y = -x00 + x02 - x10 * 2 + x12 * 2 - x20 + x22;

      a = (abs(x) + abs(y)) >> 2;
      rout1[1 + j] = a > 255 ? 255 : a;

      x = -x10 - x11 * 2 - x12 + x30 + x31 * 2 + x32;
      y = -x10 + x12 - x20 * 2 + x22 * 2 - x30 + x32;

      a = (abs(x) + abs(y)) >> 2;
      rout2[1 + j] = a > 255 ? 255 : a;

      x = -x20 - x21 * 2 - x22 + x40 + x41 * 2 + x42;
      y = -x20 + x22 - x30 * 2 + x32 * 2 - x40 + x42;

      a = (abs(x) + abs(y)) >> 2;
      rout3[1 + j] = a > 255 ? 255 : a;
    }
  }
}

/*
 *
 *--------------- Naif ---------------
 *
 */					
void sobel1(Mat & f, Mat & out) {
  int a;
  for (int i = 1; i < f.rows - 1; i++) {
    for (int j = 1; j < f.cols - 1; j++) {
      a = (abs(-f.at < char > (i - 1, j - 1) + f.at < char > (i - 1, j + 1) - 2 * f.at < char > (i, j - 1) + 2 * f.at < char > (i, j + 1) - f.at < char > (i + 1, j - 1) + f.at < char > (i + 1, j + 1)) +
        abs(-f.at < char > (i - 1, j - 1) - 2 * f.at < char > (i - 1, j) - f.at < char > (i - 1, j + 1) + f.at < char > (i + 1, j - 1) + 2 * f.at < char > (i + 1, j) + f.at < char > (i + 1, j + 1) * 3)) / 4;
      out.at < char > (i, j) = a > 255 ? 255 : a;
    }
  }
}

/*
 *
 *--------------- NAIf déroulé enn i ---------------
 *
 */			
void sobel2(Mat & f, Mat & out) {
  int a;
  char
  x10, x11, x12, x20, x21, x22;
  for (int i = 1; i < f.rows - 1; i += 2) {
    for (int j = 1; j < f.cols - 1; j++) {
      x10 = f.at < char > (i, j - 1);
      x11 = f.at < char > (i, j);
      x12 = f.at < char > (i, j + 1);
      x20 = f.at < char > (i + 1, j - 1);
      x21 = f.at < char > (i + 1, j);
      x22 = f.at < char > (i + 1, j + 1);
      //x->y
      a = (abs(-f.at < char > (i - 1, j - 1) + f.at < char > (i - 1, j + 1) - 2 * x10 + 2 * x12 - x20 + x22) +
        abs(-f.at < char > (i - 1, j - 1) - 2 * f.at < char > (i - 1, j) - f.at < char > (i - 1, j + 1) + x20 + 2 * x21 + x22)) / 2;
      out.at < char > (i, j) = a > 255 ? 255 : a;

      a = (abs(-x10 + x12 - 2 * x20 + 2 * x22 - f.at < char > (i + 2, j - 1) + f.at < char > (i + 2, j + 1)) +
        abs(-f.at < char > (i, j - 1) - 2 * f.at < char > (i, j) - f.at < char > (i, j + 1) + f.at < char > (i + 2, j - 1) + 2 * f.at < char > (i + 2, j) + f.at < char > (i + 2, j + 1))) / 2;
      out.at < char > (i + 1, j) = a > 255 ? 255 : a;
    }
  }
}

/*
 *
 *--------------- Poiteurs  ---------------
 *
 */					
void sobel3(Mat & frame, Mat & out) {
  int x, y;
  char a,
  x00, x01, x02,
  x10, x11, x12,
  x20, x21, x22;

  uchar * r1, * r2, * r3, * rout1;
  r2 = frame.ptr(0);
  r3 = frame.ptr(1);
  for (int i = 0; i < frame.rows - 2; i++) {
    r1 = r2;
    r2 = r3;
    r3 = frame.ptr(2 + i);
    rout1 = out.ptr(1 + i);
    for (int j = 0; j < frame.cols - 2; j++) {
      x00 = r1[j];
      x01 = r1[j + 1];
      x02 = r1[j + 2];

      x10 = r2[j];
      x11 = r2[j + 1];
      x12 = r2[j + 2];

      x20 = r3[j];
      x21 = r3[j + 1];
      x22 = r3[j + 2];

      x = -x00 - (x01 << 1) - x02 + x20 + (x21 << 1) + x22;
      y = -x00 + x02 - (x10 << 1) + (x12 << 1) - x20 + x22;

      a = (abs(x) + abs(y)) >> 1;
      rout1[1 + j] = a > 255 ? 255 : a;
    }
  }
}

/*
 *
 *--------------- Task déroulé en i et j ---------------
 *
 */
void sobel_task_uu(Mat frame, Mat out, int start, int end) {
  int x, y;
  int a,
  x00, x01, x02, x03, x04,
  x10, x11, x12, x13, x14,
  x20, x21, x22, x23, x24,
  x30, x31, x32, x33, x34,
  x40, x41, x42, x43, x44;

  uchar * r1, * r2, * r3, * r4, * r5, * rout1, * rout2, * rout3;
  r4 = frame.ptr(start);
  r5 = frame.ptr(start + 1);
  for (int i = start; i < end - 1; i += 3) {
    r1 = r4;
    r2 = r5;
    r3 = frame.ptr(2 + i);
    r4 = frame.ptr(3 + i);
    r5 = frame.ptr(4 + i);
    rout1 = out.ptr(1 + i);
    rout2 = out.ptr(2 + i);
    rout3 = out.ptr(3 + i);
    for (int j = 0; j < frame.cols - 2; j += 3) {
      x00 = r1[j];
      x01 = r1[j + 1];
      x02 = r1[j + 2];
      x03 = r1[j + 3];
      x04 = r1[j + 4];

      x10 = r2[j];
      x11 = r2[j + 1];
      x12 = r2[j + 2];
      x13 = r2[j + 3];
      x14 = r2[j + 4];

      x20 = r3[j];
      x21 = r3[j + 1];
      x22 = r3[j + 2];
      x23 = r3[j + 3];
      x24 = r3[j + 4];

      x30 = r4[j];
      x31 = r4[j + 1];
      x32 = r4[j + 2];
      x33 = r4[j + 3];
      x34 = r4[j + 4];

      x40 = r5[j];
      x41 = r5[j + 1];
      x42 = r5[j + 2];
      x43 = r5[j + 3];
      x44 = r5[j + 4];

      x = -x00 - (x01 << 1) - x02 + x20 + (x21 << 1) + x22;
      y = -x00 + x02 - (x10 << 1) + (x12 << 1) - x20 + x22;

      a = (abs(x) + abs(y)) >> 1;
      rout1[1 + j] = a > 255 ? 255 : a;

      x = -x10 - (x11 << 1) - x12 + x30 + (x31 << 1) + x32;
      y = -x10 + x12 - (x20 << 1) + (x22 << 1) - x30 + x32;

      a = (abs(x) + abs(y)) >> 1;
      rout2[1 + j] = a > 255 ? 255 : a;

      x = -x20 - (x21 << 1) - x22 + x40 + (x41 << 1) + x42;
      y = -x20 + x22 - (x30 << 1) + (x32 << 1) - x40 + x42;

      a = (abs(x) + abs(y)) >> 1;
      rout3[1 + j] = a > 255 ? 255 : a;

      x = -x01 - (x02 << 1) - x03 + x21 + (x22 << 1) + x23;
      y = -x01 + x03 - (x11 << 1) + (x13 << 1) - x21 + x23;

      a = (abs(x) + abs(y)) >> 1;
      rout1[2 + j] = a > 255 ? 255 : a;

      x = -x11 - (x12 << 1) - x13 + x31 + (x32 << 1) + x33;
      y = -x11 + x13 - (x21 << 1) + (x23 << 1) - x31 + x33;

      a = (abs(x) + abs(y)) >> 1;
      rout2[2 + j] = a > 255 ? 255 : a;

      x = -x21 - (x22 << 1) - x23 + x41 + (x42 << 1) + x43;
      y = -x21 + x23 - (x31 << 1) + (x33 << 1) - x41 + x43;

      a = (abs(x) + abs(y)) >> 1;
      rout3[2 + j] = a > 255 ? 255 : a;

      x = -x02 - (x03 << 1) - x04 + x22 + (x23 << 1) + x24;
      y = -x02 + x04 - (x12 << 1) + (x14 << 1) - x22 + x24;

      a = (abs(x) + abs(y)) >> 1;
      rout1[3 + j] = a > 255 ? 255 : a;

      x = -x12 - (x13 << 1) - x14 + x32 + (x33 << 1) + x34;
      y = -x12 + x14 - (x22 << 1) + (x24 << 1) - x32 + x34;

      a = (abs(x) + abs(y)) >> 1;
      rout2[3 + j] = a > 255 ? 255 : a;

      x = -x22 - (x23 << 1) - x24 + x42 + (x43 << 1) + x44;
      y = -x22 + x24 - (x32 << 1) + (x34 << 1) - x42 + x44;

      a = (abs(x) + abs(y)) >> 1;
      rout3[3 + j] = a > 255 ? 255 : a;
    }
  }
}

/*
 *
 *--------------- Task déroulé en i ---------------
 *
 */
void sobel_task_u(Mat frame, Mat out, int start, int end) {
  int x, y;
  int a,
  x00, x01, x02,
  x10, x11, x12,
  x20, x21, x22,
  x30, x31, x32,
  x40, x41, x42;

  uchar * r1, * r2, * r3, * r4, * r5, * rout1, * rout2, * rout3;
  r4 = frame.ptr(start);
  r5 = frame.ptr(start + 1);
  for (int i = start; i < end - 1; i += 3) {
    r1 = r4;
    r2 = r5;
    r3 = frame.ptr(2 + i);
    r4 = frame.ptr(3 + i);
    r5 = frame.ptr(4 + i);
    rout1 = out.ptr(1 + i);
    rout2 = out.ptr(2 + i);
    rout3 = out.ptr(3 + i);
    for (int j = 0; j < frame.cols - 2; j++) {
      x00 = r1[j];
      x01 = r1[j + 1];
      x02 = r1[j + 2];

      x10 = r2[j];
      x11 = r2[j + 1];
      x12 = r2[j + 2];

      x20 = r3[j];
      x21 = r3[j + 1];
      x22 = r3[j + 2];

      x30 = r4[j];
      x31 = r4[j + 1];
      x32 = r4[j + 2];

      x40 = r5[j];
      x41 = r5[j + 1];
      x42 = r5[j + 2];

      x = -x00 - x01 * 2 - x02 + x20 + x21 * 2 + x22;
      y = -x00 + x02 - x10 * 2 + x12 * 2 - x20 + x22;

      a = (abs(x) + abs(y)) >> 1;
      rout1[1 + j] = a > 255 ? 255 : a;

      x = -x10 - x11 * 2 - x12 + x30 + x31 * 2 + x32;
      y = -x10 + x12 - x20 * 2 + x22 * 2 - x30 + x32;

      a = (abs(x) + abs(y)) >> 1;
      rout2[1 + j] = a > 255 ? 255 : a;

      x = -x20 - x21 * 2 - x22 + x40 + x41 * 2 + x42;
      y = -x20 + x22 - x30 * 2 + x32 * 2 - x40 + x42;

      a = (abs(x) + abs(y)) >> 1;
      rout3[1 + j] = a > 255 ? 255 : a;
    }
  }
}

/*
 *
 *--------------- Task ---------------
 *
 */
void sobel_task(Mat frame, Mat out, int start, int end) {
  int x, y;
  int a,
  x00, x01, x02,
  x10, x11, x12,
  x20, x21, x22;

  uchar * r1, * r2, * r3, * rout1;
  r2 = frame.ptr(start - 1);
  r3 = frame.ptr(start);

  for (int i = start; i < end - 1; i++) {
    r1 = r2;
    r2 = r3;
    r3 = frame.ptr(i + 1);
    rout1 = out.ptr(i);
    for (int j = 0; j < frame.cols - 2; j++) {
      x00 = r1[j];
      x01 = r1[j + 1];
      x02 = r1[j + 2];

      x10 = r2[j];
      x11 = r2[j + 1];
      x12 = r2[j + 2];

      x20 = r3[j];
      x21 = r3[j + 1];
      x22 = r3[j + 2];

      x = -x00 - (x01 << 1) - x02 + x20 + (x21 << 1) + x22;
      y = -x00 + x02 - (x10 << 1) + (x12 << 1) - x20 + x22;

      a = (abs(x) + abs(y)) >> 1;
      rout1[j + 1] = a > 255 ? 255 : a;
    }
  }
}

/*
 *
 *--------------- Threads ---------------
 *
 */
void sobel_thread(Mat &f, Mat &out) {
  int n = thread::hardware_concurrency();
  thread* threads[n];
  int band_width = f.rows / n;

  threads[0] = new thread(sobel_task_u, f, out, 1, band_width);
  for (int i = 1; i < n - 1; i++) {
    threads[i] = new thread(sobel_task_u, f, out, i * band_width - 1, (i + 1) * band_width);
  }
  threads[n - 1] = new thread(sobel_task_u, f, out, (n - 1) * band_width - 1, f.rows);

  for (thread * t: threads)
    t -> join();
}

/*
 *
 *--------------- MAIN FUNCTION ---------------
 *
 */
int main() {
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
  Mat3b frame; // couleur
  Mat frame_gray; // niveau de gris
  Mat grad;
  Mat grad_x;
  Mat grad_y;
  Mat abs_grad_y;
  Mat abs_grad_x;

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
  //namedWindow("Video gray levels", WINDOW_AUTOSIZE);
  //namedWindow("Video Edge detection", WINDOW_AUTOSIZE);
  // placement arbitraire des  fenêtre sur écran
  // sinon les fenêtres sont superposée l'une sur l'autre
  // moveWindow("Video input", 10, 30);
  //moveWindow("Video gray levels", 800, 30);
  //moveWindow("Video Edge detection", 800, 500);

  // --------------------------------------------------
  // boucle infinie pour traiter la séquence vidéo
  //
  double e, s;
  double mean_sobel[7];
  string labels[7] = {"Opencv : ", "Naif : ", "Naif deroule : ", "Pointeurs : ",
   "Pointeurs deroule i : ", "Poiteur deroules i&j : ", "Multithread : "};
  int n_frames = 0;

  cap.read(frame);
  while (key != 'q' && n_frames < 24) {
    //
    // acquisition d'une trame video - librairie OpenCV
    cap.read(frame);
    // conversion en niveau de gris - librairie OpenCV
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    grad = frame_gray.clone();

    gettimeofday( & start, NULL);
    for (int i = 0; i < 50; ++i) {
      sobel0(frame_gray, grad);
    }
    gettimeofday( & end, NULL);
    e = ((double) end.tv_sec * 1000000.0 + (double) end.tv_usec);
    s = ((double) start.tv_sec * 1000000.0 + (double) start.tv_usec);
    mean_sobel[0] = (mean_sobel[0] * n_frames + (e - s) / 50) / (n_frames + 1);

    gettimeofday( & start, NULL);
    for (int i = 0; i < 50; ++i) {
      sobel1(frame_gray, grad);
    }
    gettimeofday( & end, NULL);
    e = ((double) end.tv_sec * 1000000.0 + (double) end.tv_usec);
    s = ((double) start.tv_sec * 1000000.0 + (double) start.tv_usec);
    mean_sobel[1] = (mean_sobel[1] * n_frames + (e - s) / 50) / (n_frames + 1);

    gettimeofday( & start, NULL);
    for (int i = 0; i < 50; ++i) {
      sobel2(frame_gray, grad);
    }
    gettimeofday( & end, NULL);
    e = ((double) end.tv_sec * 1000000.0 + (double) end.tv_usec);
    s = ((double) start.tv_sec * 1000000.0 + (double) start.tv_usec);
    mean_sobel[2] = (mean_sobel[2] * n_frames + (e - s) / 50) / (n_frames + 1);

    gettimeofday( & start, NULL);
    for (int i = 0; i < 50; ++i) {
      sobel3(frame_gray, grad);
    }
    gettimeofday( & end, NULL);
    e = ((double) end.tv_sec * 1000000.0 + (double) end.tv_usec);
    s = ((double) start.tv_sec * 1000000.0 + (double) start.tv_usec);
    mean_sobel[3] = (mean_sobel[3] * n_frames + (e - s) / 50) / (n_frames + 1);

    gettimeofday( & start, NULL);
    for (int i = 0; i < 50; ++i) {
      sobel4(frame_gray, grad);
    }
    gettimeofday( & end, NULL);
    e = ((double) end.tv_sec * 1000000.0 + (double) end.tv_usec);
    s = ((double) start.tv_sec * 1000000.0 + (double) start.tv_usec);
    mean_sobel[4] = (mean_sobel[4] * n_frames + (e - s) / 50) / (n_frames + 1);

    gettimeofday( & start, NULL);
    for (int i = 0; i < 50; ++i) {
      sobel5(frame_gray, grad);
    }
    gettimeofday( & end, NULL);
    e = ((double) end.tv_sec * 1000000.0 + (double) end.tv_usec);
    s = ((double) start.tv_sec * 1000000.0 + (double) start.tv_usec);
    mean_sobel[5] = (mean_sobel[5] * n_frames + (e - s) / 50) / (n_frames + 1);

    gettimeofday( & start, NULL);
    for (int i = 0; i < 50; ++i) {
      sobel_thread(frame_gray, grad);
    }
    gettimeofday( & end, NULL);
    e = ((double) end.tv_sec * 1000000.0 + (double) end.tv_usec);
    s = ((double) start.tv_sec * 1000000.0 + (double) start.tv_usec);
    mean_sobel[6] = (mean_sobel[6] * n_frames + (e - s) / 50) / (n_frames + 1);
    // -------------------------------------------------
    // visualisation
    // taille d'image réduite pour meuilleure disposition sur écran

    /*resize(frame, frame, Size(), 0.5, 0.5);
    resize(frame_gray, frame_gray, Size(), 0.5, 0.5);
    resize(grad, grad, Size(), 1, 1);
    imshow("Video input",frame);
    imshow("Video gray levels", frame_gray);
    imshow("Video Edge detection", grad);*/
    key = waitKey(5);
    n_frames++;
  }
  
  for(int i=0; i<7;i++)
  	cout << labels[i] << mean_sobel[i] << endl;
}
