/*
 * compute.c
 *
 *  Created on: Dec 2, 2017
 *      Author: samaustin
 */

#include "compute.h"
#include <math.h>
#include "fft.h"
#include "circbuf.h"
#include <string.h>
#include "uart.h"
#include "pwm.h"

#define STANDBYE (uint8_t)(3)
#define FILL (uint8_t)(1)
#define PROCESS (uint8_t)(0)
#define START (uint8_t)(4)
#define RESUME (uint8_t)(4)
#define END (uint8_t)(6)
#define COMPARE (uint8_t)(7)
#define RECORD (uint8_t)(8)

extern int fftcount;
extern Status_t speakingStatus;
extern CircBuf_t * PrimaryBuff;
extern CircBuf_t * SecondaryBuff;
extern int thresholdcount;
extern float averageDifference;
extern int NOP;
extern int n;
extern int i;

void calculate_magnitude_and_compare(complex_t total[],float magnitude[],float recordVector[],float compareVector[]){
    int halfn = n/2;
    for(i =0;i<halfn;i++){
        total[i].real = total[i].real;
        total[i].imag = total[i].imag;
        magnitude[i] = sqrt(total[i].real*total[i].real
                          + total[i].imag*total[i].imag)/n;
        total[i].real =0;
        total[i].imag=0;
    }
    magnitude[0] = magnitude[1]; //clearing highest frequency bin
    averageDifference =0;
    for(i=0;i<halfn;i++){
        if(speakingStatus.comparemode ==1){
            compareVector[i] = magnitude[i];
        }
        else{
            recordVector[i] = magnitude[i];
        }
        averageDifference+=(compareVector[i]-recordVector[i]);
    }


    averageDifference=sqrt(averageDifference*averageDifference)/halfn;
    if(averageDifference<=15){
        P2->OUT |=BIT1;
        P2->OUT &= ~BIT0;
        send_pwm_unlock();
    }
    if(averageDifference>15){
        P2->OUT |=BIT0;
        P2->OUT &= ~BIT1;
    }
    format();
    char sendAvgDifference[8];
    ftoa(averageDifference,sendAvgDifference,4);
    UART_send_n(sendAvgDifference,strlen(sendAvgDifference));
    newline();
    fftcount=0;
}













void test_threshold(){
    __disable_interrupt();
    if(thresholdcount >= 2){
       P1->OUT |=BIT0; //start capturing and performing fft;
       set_speaking_status(START);
       set_buffer_status(PrimaryBuff,FILL);
       set_buffer_status(SecondaryBuff,PROCESS);
       __enable_interrupt();
       return;
    }

    set_speaking_status(END);
    __enable_interrupt();
}















void newline(){
    char l[]="\n";
    UART_send_n(l,1);
}
void format(){
    char d[]="Average Difference: ";
    UART_send_n(d,20);
}

void set_speaking_status(uint8_t status){
    if(status ==4){
        speakingStatus.speaking =1;
        speakingStatus.finishSpeaking =0;
    }
    if(status ==6){
        speakingStatus.speaking =0;
        speakingStatus.finishSpeaking =1;
    }

    if(status ==7){
        speakingStatus.comparemode = 1;
        speakingStatus.recordmode =0;
    }

    if(status ==8){
        speakingStatus.comparemode = 0;
        speakingStatus.recordmode =1;
    }
    if(status ==9){
        speakingStatus.speaking =0;
        speakingStatus.finishSpeaking =0;
    }

}



/*void mffc(float magnitude[],float filterBank[][]){
    int j,g;
    float high_freq_mel =2595*log(1+(6000/700));
    float filterBank[16][256];
    float f[16];
    float mel[12];
    float h[12];
    float step = high_freq_mel /16;
    float index;
    for(j=0,index =0;j<16;index+=step,j++){
        mel[j] = 2595*log(1+(index/700));
        h[j] = 700*(pow(10,(mel[j]/2595))-1);
        f[j] = floor((n+1)*h[i]/6000);
     }
    int f_m_minus,f_m,f_m_plus,k,m;
    for(m=1;m<17;m++){
        f_m_minus = f[m-1];
        f_m =f[m];
        f_m_plus[m+1];
        for(k=f_m_minus;k<f_m;k++){
            filterBank[g-1][k] = (k-f[m-1])/(f_m[m]-f_m[m-1]);
        }
        for(k=f_m;k<f_m_plus;k++){
            filterBank[g-1][k] =(k-f[m-1])/(f_m[m]-f_m[m-1]);
        }
        for(k=0;k<halfn;k++){
            filterBank[m][k]*=magnitude[k];
            filterBank[m][k] =log(filterBank[m][k]);
        }

    }
}*/


int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}
void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}
void ftoa(float n, char *res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}
