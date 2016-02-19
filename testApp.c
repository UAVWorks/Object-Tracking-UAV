#include "testApp.h"

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

float err(float x)
{
    float t= 1/(1+.5 *abs(x));
    float tu=t*exp(-x*x-1.26551+1.000023*t+.37409*t*t*t-.186288 *
    pow(t,4)+.27886*pow(t,5)-1.135203*pow(t,6));
    if(x)
    return (1-tu);
    else
    return(tu-1);
}
void evolve(unsigned char *img,double updt[][9],double *refhist,int w,int h)
{
    double obj[8],hist[257*10],dist;
    exobj(updt,obj);
    IplImage *im,*im2;
    im2=cvCreateImage( cvSize(w,h), IPL_DEPTH_8U, 3);
    memcpy(im2->imageData,img,im2->imageSize);
    im=cvCloneImage(im2);
    cvCvtColor(im2,im, CV_RGB2HSV);
    crhist((unsigned char *)im->imageData,hist,w,h,obj);
    dist=(comphist(refhist,hist));
    for(int i=0;i<N;i++)
    {
        updt[i][4]+=(rand()%((int)(60*dist))-(int)(30*dist));
        updt[i][5]+=(rand()%((int)(60*dist))-(int)(30*dist));
        updt[i][6]+=(rand()%((int)(60*dist))-(int)(30*dist));
         updt[i][0]+=updt[i][4];
        updt[i][1]+=updt[i][5];
        updt[i][2]+=updt[i][6];
        updt[i][3]=(updt[i][2]*60)/80;
        if((updt[i][0]<0 || updt[i][0]>320))
             updt[i][0]=w/2;updt[i][4]=0;

        if((updt[i][1]<0 || updt[i][1]>240))
             updt[i][1]=h/2;updt[i][5]=0;
//        if((updt[i][2]<=40 || updt[i][3]<=40))
//        {
//        /*updt[i][2]=80;
//        updt[i][3]=60;
//        updt[i][6]=updt[i][7]=0;*/
//        }
    }
}

bool isrect(CvPoint p[])
{
    if(abs(sqrt(pow((float)p[0].x-p[2].x,2)+pow((float)p[0].y-p[2].y,2)) -
    sqrt(pow((float)p[1].x-p[3].x,2)+pow((float)p[1].y-p[3].y,2)))<9 )
    return 1;
    else
    return 0;
}
void msrmtupdate(unsigned char *img,double updt[][9],IpVec *refpts,double
*refhist,int w,int h)
{
    long double tot=0,dist2=0;
    double hist[257 * 10]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    static double ptr[4],p;
    int key[10][2];
    IpVec pts,tpts;
    IplImage *cl,*tmp2;
    cl=cvCreateImage(cvSize(w,h),IPL_DEPTH_8U,3);
    memcpy(cl->imageData,(img),cl->imageSize);
    tmp2=cvCloneImage(cl);
    surfDetDes(cl,tpts ,false, 5, 4, 2, 0.00004f);
    IpPairVec matches;
    IpVec ipts, ref_ipts;
    CvPoint src_corners[4] = {cvPoint(0,0), cvPoint(80,0), cvPoint(80, 60),
    cvPoint(0, 60)};
    CvPoint dst_corners[4];
    getMatches(tpts,*refpts,matches);
    int tt=0;
    tt=translateCorners(matches, src_corners, dst_corners);
    if (translateCorners(matches, src_corners, dst_corners))
     {
         // Draw box around object
         for(int i = 0; i < 4; i++ )

         {
             CvPoint r1 = dst_corners[i%4];
             CvPoint r2 = dst_corners[(i+1)%4];
             cvLine( cl, cvPoint(r1.x, r1.y), cvPoint(r2.x, r2.y), cvScalar(255,255,255),3 );
         }
         for (unsigned int i = 0; i < matches.size(); ++i)
         drawIpoint(cl, matches[i].first);
     }
    CvPoint cpt;
    cpt.x=((dst_corners[0].x)+(dst_corners[2].x))/2;
    cpt.y=((dst_corners[0].y)+(dst_corners[2].y))/2;
    p++;
    if(tt)
    {
        if((abs(ptr[2]-abs(dst_corners[0].x-dst_corners[1].x))>=30 ||
        abs(ptr[3]-abs(dst_corners[0].y-dst_corners[3].y))>=30 ||
        !isrect(dst_corners)) && p>3 )
        {
             tt=0;
        }
        else
        {
            cvCvtColor(tmp2,cl ,CV_RGB2HSV);
            ptr[0]=cpt.x;ptr[1]=cpt.y;ptr[2]=abs(dst_corners[0].xst_corners[1].x);ptr[3]=abs(dst_corners[0].y-dst_corners[3].y);
            crhist((unsigned char *)cl->imageData,hist,w,h,ptr);
            dist2=.1*(double)exp(-2*pow(comphist(hist,refhist),2));
        }
    }
    for(int i=0;i<N;i++)
    {
    if(tt && dist2>.05 )
    {
        updt[i][0]=cpt.x;
        updt[i][1]=cpt.y;
        updt[i][2]=ptr[2];
        updt[i][3]=ptr[3];
        updt[i][4]=1;
        updt[i][5]=1;
        updt[i][8]=1;
        tot++;
    }
    else
     {
        double pt[4];
        for(int k=0;k<4;k++)
        {
            pt[k]=updt[i][k];
        }
        cvCvtColor(tmp2,cl, CV_RGB2HSV);
        crhist((unsigned char *)cl->imageData,hist,w,h,pt);
        dist2=.1*(double)exp(-100*pow(comphist(hist,refhist),2));
        updt[i][8]=dist2;
        tot+=updt[i][8];
    }
    }
    for(int i=0;i<N;i++)
      updt[i][8]/=(double)tot;

}
void exobj(double updt[][9],double *obj)
{
    for(int i=0;i<8;i++)
    {
        obj[i]=0;
        for(int j=0;j<N;j++)
            obj[i]+=(double)updt[j][8]*updt[j][i];
    }
}
void init(unsigned char *img,double *refhist,IpVec *refpts,double updt[][9],int w,int h)
{
    double obj[8]={0};
    unsigned char pix[60*80*3]={0,0};
    obj[0]=w/2;obj[1]=h/2;obj[2]=80;obj[3]=60;obj[4]=obj[5]=1;obj[6]=obj[7]=0;
    int x=obj[0],y=obj[1],wt=obj[2],ht=obj[3];
    int p=0,l=0;
    IpVec pt;
    IplImage *im;
    im=cvCreateImage(cvSize(w,h),IPL_DEPTH_8U,3);
    memcpy(im->imageData,img,im->imageSize);
    surfDetDes(im,pt,false,5,4,2,0.00004f);
    p=0;
    for(int k=0;k<pt.size();k++)
    {
        if((pt.at(k).x>=(w/2-wt/2)) && (pt.at(k).x<=(w/2+wt/2)) &&
        (pt.at(k).y>=(h/2-ht/2)) && (pt.at(k).y<=(h/2+ht/2)) )
        {
            Ipoint tmp;
            pt.at(k).x-=(w/2-wt/2);

            pt.at(k).y-=(h/2-ht/2);
            (*refpts).push_back(pt.at(k));
        }
    }
    for(int i=0;i<N;i++)
    {
        updt[i][0]=obj[0]+(rand()%60-30);
        updt[i][1]=obj[1]+(rand()%60-30);
        updt[i][2]=obj[2];
        updt[i][3]=obj[3];
        updt[i][4]=obj[4];
        updt[i][5]=obj[5];
        updt[i][6]=obj[6];
        updt[i][7]=obj[7];
        updt[i][8]=(double)1/N;
        img[3*(int)(w*updt[i][1]+updt[i][0])]=0;img[3*(int)(w*updt[i][1]+updt[i
        ][0])+1]=255;img[3*(int)(w*updt[i][1]+updt[i][0])+1]=0;
    }
}
void resample(unsigned char *img,double updt[][9],double *pconf,int w,int h,int t)
{
    long double obj[8]={0},tot=0;
    double temp[N][10];
    temp[0][9]=0;
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<9;j++)
        {
        temp[i][j]=updt[i][j];
        }
        if (i>0)
        temp[i][9]=temp[i-1][9]+updt[i][8];
        tot+=updt[i][8];
    }
    for(int i=0;i<N;i++)
    {
        double tmp=rand()%100+1;
        tmp=tmp/100;
        updt[i][1]=temp[N-1][1];
        updt[i][0]=temp[N-1][0];
        updt[i][4]=temp[N-1][4];
        updt[i][5]=temp[N-1][5];
        updt[i][8]=1/N;
        int rt=0,lt=N,key=(rt+lt)/2;
        while(rt!=lt+1 && lt!=rt+1)
        {
            key=(rt+lt)/2;
            if(temp[key][9]==tmp)
                break;
            else if(temp[key][9]>tmp)
                lt=key;
            else if(temp[key][9]<tmp)
                rt=key;
        }
        updt[i][1]=temp[key][1];
        updt[i][0]=temp[key][0];
        updt[i][3]=temp[key][3];
        updt[i][2]=temp[key][2];
        updt[i][4]=temp[key][4];
        updt[i][5]=temp[key][5];
        updt[i][6]=temp[key][6];
        updt[i][7]=temp[key][7];
        updt[i][8]=(double)1/N;
        img[3*(int)(w*updt[i][1]+updt[i][0])]=0;img[3*(int)(w*updt[i][1]+updt[i
        ][0])+1]=255;img[3*(int)(w*updt[i][1]+updt[i][0])+1]=0;
    }
}

void crhist(unsigned char *img,double *hist,int w,int h,double obj[4])
{
    long int i=0;
    int x,y,wt,ht,tot=0;
    x=obj[0];y=obj[1];wt=obj[2];ht=obj[3];
    double mr=0,mg=0,mb=0;

    for(i=0;i<=(257*4);i++)
        hist[i]=0;
    for(i=w*(y-ht/2);i<w*h && i<w*(y+ht/2) && i>0; i++)
    {
        if(i%w > x-wt/2 && i%w < x+wt/2)
        {
            tot++;
            int r=img[3*i],g=img[3*i+1],b=img[3*i+2];
            hist[r]++;
            hist[256+g]++;
            hist[256*2+b]++;
        }
    }
    mr=mg=mb=wt*(ht-1);

    for(i=0;i<256;i++)
    {
        hist[i]/=mr;
        hist[256+i]/=mg;
        hist[256*2+i]/=mb;
    }
}

void rep(IplImage *img,double obj[4],int w,int h)
{
    int x1,y1,wt,ht,tot=0;
    CvPoint p1,p2,p3,p4,c;
    x1=obj[0];y1=obj[1];wt=obj[2];ht=obj[3];
    p1.x=x1-wt/2;p1.y=y1-ht/2;p2.x=x1-
    wt/2;p2.y=y1+ht/2;p3.x=x1+wt/2;p3.y=y1-ht/2;p4.x=x1+wt/2;p4.y=y1+ht/2;
    CvMat im;
    cvRectangle(img,p1,p4,cvScalar(0,255,0,100),1,8,0);
}

float comphist(double *hist1,double *hist2)
{
    double dist[3]={0.0,0.0,0.0};
    for(int i=0;i<256;i++)
    {
        dist[0]+=sqrt(hist1[i]*hist2[i]);
        dist[1]+=sqrt(hist1[256+i]*hist2[256+i]);
        dist[2]+=sqrt(hist1[256*2+i]*hist2[256*2+i]);
    }

    dist[0]=sqrt(abs(1-dist[0]));
    dist[1]=sqrt(abs(1-dist[1]));
    dist[2]=sqrt(abs(1-dist[2]));
    return((dist[0]+dist[1]+dist[2])/3);
}
