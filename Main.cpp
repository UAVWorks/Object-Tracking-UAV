#include "main.h"
#include "testapp.h"
// set up DMA descriptors (one for each frame, then repeat)
// small descriptor model, only start address needs to be fetched
tDMA_descriptor DMA_PPI0_first = {&DMA_PPI0_second, sFrame0};
tDMA_descriptor DMA_PPI0_second = {&DMA_PPI0_third , sFrame1};
tDMA_descriptor DMA_PPI0_third = {&DMA_PPI0_fourth, sFrame2};
tDMA_descriptor DMA_PPI0_fourth = {&DMA_PPI0_first , sFrame3};
volatile int current_in_Frame = -1; // 0, 1, 2 or 3 ... indicates the last frame that was received COMPLETELY
bool Set_PACK32 = false;
bool Set_Entire_Field = false;

// User program
void main()
{
    // unblock Core B if dual core operation is desired
    #ifndef RUN_ON_SINGLE_CORE
    *pSICA_SYSCR &= 0xFFDF; // clear bit 5 to unlock
    #endif
    // set Clocks
    Set_PLL( (short)(CORECLK/CLKIN), (short)(CORECLK/SYSCLK));
    // sets Core and System Clocks to the values defined in system.h

    // initialise SDRAM
    InitSDRAM();
    *pTC_PER = 0x0770; // set DMA traffic control register to favour unidirectional transfers to SDRAM
    // initialise Video Encoder ADV7179
    Reset_ADV7179();
    // initialise Video Decoder ADV7183
    Reset_ADV7183();
    // initialise PPI0 and associated DMA channel for Video IN
    current_in_Frame = -1; // no frames received yet
    semaphore_frames_received = false; // do not start output stream yet
    semaphore_frames_completed = false; // do not start output stream
    yet
    Set_Entire_Field = false;
    #ifdef ENTIRE_FIELD_MODE
    Set_Entire_Field = true;
    #endif
    Set_PACK32 = false;
    #ifdef PACK_32
    Set_PACK32 = true;
    #endif
    InitPPI0(Set_Entire_Field, Set_PACK32, &DMA_PPI0_first,
    PIXEL_PER_LINE, LINES_PER_FRAME);
    // initialise Interrupts
    InitInterrupts_coreA();

    // enable transfers
    EnablePPI0();
    w=PIXEL_PER_LINE,h=LINES_PER_FRAME,t=0;
    // main loop, just wait for interrupts
    while(1)
    {
        // idle(); // do nothing
        // check for PPI framing error
        if (*pPPI0_STATUS & FT_ERR)
        {
            // error occurred -- clear error and restart video transfer
            *pPPI0_STATUS &= ~FT_ERR;
            semaphoreResetVideo = true;
            while(semaphoreResetVideo); // wait for core B to reset
            video
            DisablePPI0();
            current_in_Frame = -1; // no frames received yet
            semaphore_frames_received = false;
            InitPPI0(Set_Entire_Field, Set_PACK32,
            &DMA_PPI0_first, PIXEL_PER_LINE, LINES_PER_FRAME);
            EnablePPI0();
        }
        if (semaphore_frames_received)
        {
            semaphore_frames_received = false;
            //Call process Video with the currentFrame -1 (the frame just filled)
            ProcessVideo((current_in_Frame-1) % 4);
        }

    } // while(1)
} // main
void ProcessVideo(int FrameToProcess)
{
    pix2=cvCreateImage( cvSize(w,h), IPL_DEPTH_8U, 3);
    switch(FrameToProcess)
    {
        case 0:memcpy(pix2->imageData,(const void *)sFrame0,pix2->imageSize);
        break;
        case 1:memcpy(pix2->imageData,(const void *)sFrame1,pix2->imageSize);
        break;
        case 2:memcpy(pix2->imageData,(const void *)sFrame2,pix2->imageSize);
        break;
        case 3:memcpy(pix2->imageData,(const void *)sFrame3,pix2->imageSize);
    }
    pix3=cvCloneImage(pix2);
    if(t==0)
    {
        (refpts).clear();
        cvCvtColor(pix3,pix2, CV_RGB2HSV);
        crhist((unsigned char *)pix2->imageData,refhist,w,h,obj);
        obj[0]=w/2;obj[1]=h/2;obj[2]=80;obj[3]=60;obj[4]=obj[5]=1;obj[6]=obj[7]=0;
        init((unsigned char *)pix3->imageData,refhist,&refpts,updt,w,h);
        obj[0]=w/2;obj[1]=h/2;obj[2]=80;obj[3]=60;obj[4]=obj[5]=1;obj[6]=obj[7]=0;
        rep(pix3,obj,w,h);
    }
    if(t>0)
    {
        evolve((unsigned char *)pix3->imageData,updt,refhist,w,h);
        msrmtupdate((unsigned char *)pix3->imageData,updt,&refpts,refhist,w,h);
        exobj(updt,obj);
        rep(pix3,obj,w,h);
        resample((unsigned char *)pix3->imageData,updt,0,w,h,t);
    }
    switch(FrameToProcess)
    {
        case 0: memcpy((void *)sFrame0,pix3->imageData,pix2->imageSize);break;
        case 1:memcpy((void *)sFrame1,pix3->imageData,pix2->imageSize);break;
        case 2:memcpy((void *)sFrame2,pix3->imageData,pix2->imageSize);break;
        case 3:memcpy((void *)sFrame3,pix3->imageData,pix2->imageSize);
    }
    //Do processing in here
    //Set Semaphore to indicate that the frame is complete
    semaphore_frames_completed = true;
}
