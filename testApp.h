#ifndef TESTAPP_H
#define TESTAPP_H

#pragma once
#include "opencv.hpp"
#include "surf-algo\surflib.h"
#define in 50
#define N 50

static IplImage *img,*pix2,*pix3;
static IpVec refpts,hipts[in];
static IpVec pts,ppts;static IpPairVec match;
static int refkey[10][2];
static int h,w,t,tg;
static double hist1[256 * 3],hist2[256 * 3],refhist[256*3], obj[8], updt[N][9];
static unsigned char *pix1;

float compkey(int refkey[][2],unsigned int numrefkey,int key[][2],unsigned int
numkey);
void init(unsigned char *img,double *refhist,IpVec *refpts,double updt[][9],int
w,int h);
float comphist(double *hist1,double *hist2);
void crhist(unsigned char *img,double *hist,int w,int h,double obj[4]);
void resample(unsigned char *img,double updt[][9],double *pconf,int w,int
h,int t);
void evolve(unsigned char *,double updt[][9],double *refhist,int,int);
void msrmtupdate(unsigned char *img,double updt[][9], IpVec *refpts,double
*refhist,int w,int h);
void exobj(double updt[][9],double *obj);
void rep(IplImage *img,double obj[4],int w,int h);
bool isrect(CvPoint p[]);

#endif // TESTAPP_H
