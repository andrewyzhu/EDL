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
#include"mel_filterbank.h"
#include "gpio.h"
#include "dct.h"

#define STANDBYE (uint8_t)(3)
#define FILL (uint8_t)(1)
#define PROCESS (uint8_t)(0)
#define START (uint8_t)(4)
#define RESUME (uint8_t)(4)
#define END (uint8_t)(6)
#define COMPARE (uint8_t)(7)
#define RECORD (uint8_t)(8)
#define HALF (int)(256)

extern Status_t speakingStatus;
extern CircBuf_t * PrimaryBuff;
extern int thresholdcount;
extern float forwardDifference;
extern float backwardDifference;
extern float leftDifference;
extern float rightDifference;
extern int binSize;
extern int NOP;
extern int n;
extern int i;

void test_threshold(){
    __disable_interrupt();
    if(thresholdcount >= 2){
       P1->OUT |=BIT0; //start capturing and performing fft;
       set_speaking_status(START);
       set_buffer_status(PrimaryBuff,FILL);
       __enable_interrupt();
       return;
    }

    set_speaking_status(END);
    __enable_interrupt();
}

void calculate_magnitude_and_compare(complex_t total[],float fc[],float bc[],float lc[],float rc[],float compareVector[],float mfcc[]){
    int halfn = n/2;
    float power[HALF];
    //float dct_results[26];
    //float mel_filterbank_energy[26];
    int fftbin[28];
    for(i =0;i<halfn;i++){
        power[i]=0;
        total[i].real = total[i].real/halfn;
        total[i].imag = total[i].imag/halfn;
        power[i] = total[i].real*total[i].real+ total[i].imag*total[i].imag;
        total[i].real =0;
        total[i].imag=0;
        if(i<26){
           // mel_filterbank_energy[i]=0;
        }
    }
    power[0]=power[6];
    power[1]=power[6];
    power[2]=power[6];
    power[3]=power[6];
    power[4]=power[6];
    power[5]=power[6];

    melFilterCenters(fftbin);
    melCoefficients(power,fftbin,mfcc);
    logEnergies(mfcc);
    //dct(mel_filterbank_energy,dct_results,mfcc);

    forwardDifference =0;
    backwardDifference=0;
    leftDifference=0;
    rightDifference=0;

    for(i=0;i<binSize;i++){
        if(speakingStatus.comparemode ==1){
            compareVector[i] = mfcc[i];
            rightDifference+=(compareVector[i]-rc[i]);
            leftDifference+=(compareVector[i]-lc[i]);
            backwardDifference+=(compareVector[i]-bc[i]);
            forwardDifference+=(compareVector[i]-fc[i]);
        }
        else{
            if(speakingStatus.rightcommand ==1){
                rc[i] = mfcc[i];
            }
            else if(speakingStatus.leftcommand ==1){
                lc[i] = mfcc[i];
            }
            else if(speakingStatus.backwardcommand ==1){
                bc[i] = mfcc[i];
            }
            else{
                fc[i] = mfcc[i];
            }
        }
    }
    forwardDifference=sqrt(forwardDifference*forwardDifference)/binSize;
    backwardDifference=sqrt(backwardDifference*backwardDifference)/binSize;
    leftDifference=sqrt(leftDifference*leftDifference)/binSize;
    rightDifference=sqrt(rightDifference*rightDifference)/binSize;

    if(speakingStatus.comparemode ==1){
        look_for_match(forwardDifference,backwardDifference,leftDifference,rightDifference);
        //print_diff(forwardDifference,backwardDifference,leftDifference,rightDifference);
    }
}


void set_speaking_status(uint8_t status){
    if(status == RESUME){
        speakingStatus.speaking =1;
        speakingStatus.finishSpeaking =0;
    }
    else if(status ==END){
        speakingStatus.speaking =0;
        speakingStatus.finishSpeaking =1;
    }

    else if(status ==COMPARE){
        speakingStatus.comparemode = 1;
        speakingStatus.recordmode =0;
        speakingStatus.forwardcommand =0;
        speakingStatus.backwardcommand =0;
        speakingStatus.leftcommand =0;
        speakingStatus.rightcommand =0;
        P2->OUT &= ~BIT0;
        P2->OUT &= ~BIT1;
        P2->OUT &= ~BIT2;
    }

    else {
        speakingStatus.comparemode = 0;
        speakingStatus.recordmode =1;
        if(speakingStatus.forwardcommand ==1){
            speakingStatus.backwardcommand =1;
            speakingStatus.forwardcommand =0;
            P2->OUT |=BIT0;
            P2->OUT |=BIT1;
            P2->OUT &= ~BIT2;
        }
        else if(speakingStatus.backwardcommand ==1){
            speakingStatus.backwardcommand =0;
            speakingStatus.leftcommand =1;
            P2->OUT &= ~BIT0;
            P2->OUT &= ~BIT1;
            P2->OUT |=BIT2;
        }
        else if(speakingStatus.leftcommand ==1){
            speakingStatus.leftcommand =0;
            speakingStatus.rightcommand =1;
            P2->OUT |=BIT1;
            P2->OUT &= ~BIT0;
            P2->OUT |=BIT2;
        }
        else if(speakingStatus.rightcommand ==1){
            speakingStatus.rightcommand =0;
            speakingStatus.forwardcommand =1;
            P2->OUT |=BIT0;
            P2->OUT |=BIT1;
            P2->OUT |=BIT2;
        }
        else{
            speakingStatus.rightcommand =0;
            speakingStatus.forwardcommand =1;
            speakingStatus.leftcommand =0;
            speakingStatus.backwardcommand =0;
            P2->OUT |=BIT0;
            P2->OUT |=BIT1;
            P2->OUT |=BIT2;
        }
    }

}

void look_for_match(float forwardDifference,float backwardDifference,float leftDifference, float rightDifference){
    uint8_t command0='0';
    uint8_t command1='1';
    uint8_t command2='2';
    uint8_t command3='3';
    uint8_t command4='4';

    if((forwardDifference<=0.01)&&(backwardDifference>=forwardDifference)&&(leftDifference>=forwardDifference)&&(rightDifference>=forwardDifference) && (forwardDifference > 0)){
        P2->OUT |=BIT0;
        P2->OUT |=BIT1;
        P2->OUT |=BIT2;
        UART_send_byte(command1);
    }
    else if((backwardDifference<=0.01) &&(leftDifference>=backwardDifference)&&(rightDifference>=backwardDifference)&& (backwardDifference > 0)){
        P2->OUT |=BIT0;
        P2->OUT |=BIT1;
        P2->OUT &= ~BIT2;
        UART_send_byte(command2);

    }
    else if((leftDifference<=0.01)&&(rightDifference>=leftDifference) && (leftDifference > 0)){
        P2->OUT &= ~BIT0;
        P2->OUT &= ~BIT1;
        P2->OUT |=BIT2;
        UART_send_byte(command3);

    }
    else if((rightDifference<=.01) && (rightDifference > 0)){
        P2->OUT |=BIT1;
        P2->OUT &= ~BIT0;
        P2->OUT |=BIT2;
        UART_send_byte(command4);

     }
    else{
        P2->OUT |=BIT0;
        P2->OUT &= ~BIT1;
        P2->OUT &= ~BIT2;
        UART_send_byte(command0);

    }
}
/*void fftsum(complex_t total[][],complex_t data[],int index){
    int c,t;
    int halfn=n/2;
    for(c=0;c<halfn;c++){
        if(index<4){
            total[c][0] = data[c];
        }
        else if(index<8){
            total[c][1] = data[c];
        }
        else if(index<12){
            total[c][2] = data[c];
        }
        else if(index<16){
            total[c][3] = data[c];
        }
        else if(index<20){
            total[c][4] = data[c];
        }
        else if(index<24){
            total[c][5] = data[c];
        }
        else if(index<28){
            total[c][6] = data[c];
        }
        else if(index<32){
            total[c][7] = data[c];
        }
   }
}*/


void print_diff(float diff1,float diff2,float diff3,float diff4){
    format();
    newline();
    char sendfDifference[12];
    char sendbDifference[12];
    char sendrDifference[12];
    char sendlDifference[12];
    ftoa(diff1,sendfDifference,2);
    ftoa(diff2,sendbDifference,2);
    ftoa(diff3,sendlDifference,2);
    ftoa(diff4,sendrDifference,2);
    forwardformat();
    UART_send_n(sendfDifference,strlen(sendfDifference));
    newline();
    backwardformat();
    UART_send_n(sendbDifference,strlen(sendbDifference));
    newline();
    leftformat();
    UART_send_n(sendlDifference,strlen(sendlDifference));
    newline();
    rightformat();
    UART_send_n(sendrDifference,strlen(sendrDifference));
    newline();
    newline();
}



void forwardformat(){
    char fw[]="  Forward Command= ";
    UART_send_n(fw,19);
}
void backwardformat(){
    char fw[]="  Backward Command= ";
    UART_send_n(fw,20);
}
void leftformat(){
    char fw[]="  Left Command= ";
    UART_send_n(fw,15);
}
void rightformat(){
    char fw[]="  Right Command= ";
    UART_send_n(fw,16);
}

void newline(){
    char l[]="\n";
    UART_send_n(l,1);
}
void format(){
    char d[]="Average Difference: ";
    UART_send_n(d,20);
}

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
