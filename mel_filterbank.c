/*
 * mel_filterbank.c
 *
 *  Created on: Apr 23, 2018
 *      Author: anzh1
 */
#include "mel_filterbank.h"

//26 filterbanks ranging from 20 Hz to 3000 Hz.
//Formula to convert from Hertz to Mels is: 1125ln(1+f/700)
#define filter_Mel_low_edge (double)(31.6922)  //1125ln(1+20/700), 20 Hz is lower boundary of human speech
#define filter_Mel_high_edge (double)(2835) //conversion of 8000 Hz to Mels, The maximum frequency we can accurately sample is 8000 Hz. As our sampling frequency is 16000 Hz
#define filter_width (double)(103.8262) //we want 26 filters that are evenly distributed on the Mel scale from 31.6922 to 2835

//28 mel points
void melFilterCenters(int * fftBin){
    double melCenters[26];
    int i;
    for(i = 0; i < 26; i++){
        melCenters[i] = filter_Mel_low_edge + (i+1)*filter_width;
    }
    double hertzPoints[28];
    hertzPoints[0] = 20;
    hertzPoints[27] = 8000;
    int j;
    for(j = 0; j < 26; j++){
        hertzPoints[j+1] = 700*(exp(melCenters[j]/1125)-1);    //conversion from Mels to Hz
    }
    int k;
    for(k = 0; k < 28; k++){
        fftBin[k] = (int)floor((512+1)*hertzPoints[k]/16000);     //conversion from Hz to FFT bin number
    }
}

//256 fft magnitudes, 28 filterbank points, 26 filterbank energies
void melCoefficients(float * magnitude, int * filterbank, float * filterbank_energies){
    int h;
    float i;
    for(h = 1; h <= 256; h++){
        i = (float)h;
        if((i > filterbank[0]) && (i < filterbank[1])){
                filterbank_energies[0] += ((i-filterbank[0])/(filterbank[1]-filterbank[0]))*magnitude[h-1];
        }
        else if(h == filterbank[1]){
            filterbank_energies[0] += magnitude[h-1];
        }
        else if((i > filterbank[1]) && (i < filterbank[2])){
                filterbank_energies[0] += ((filterbank[2]-i)/(filterbank[2]-filterbank[1]))*magnitude[h-1];
                filterbank_energies[1] += ((i-filterbank[1])/(filterbank[2]-filterbank[1]))*magnitude[h-1];
        }
        else if(h == filterbank[2]){
            filterbank_energies[1] += magnitude[h-1];
        }
        else if((i > filterbank[2]) && (i < filterbank[3])){
                filterbank_energies[1] += ((filterbank[3]-i)/(filterbank[3]-filterbank[2]))*magnitude[h-1];
                filterbank_energies[2] += ((i-filterbank[2])/(filterbank[3]-filterbank[2]))*magnitude[h-1];
        }
        else if(h == filterbank[3]){
            filterbank_energies[2] += magnitude[h-1];
        }
        else if((i > filterbank[3]) && (i < filterbank[4])){
                filterbank_energies[2] += ((filterbank[4]-i)/(filterbank[4]-filterbank[3]))*magnitude[h-1];
                filterbank_energies[3] += ((i-filterbank[3])/(filterbank[4]-filterbank[3]))*magnitude[h-1];
        }
        else if(h == filterbank[4]){
            filterbank_energies[3] += magnitude[h-1];
        }
        else if((i > filterbank[4]) && (i < filterbank[5])){
                filterbank_energies[3] += ((filterbank[5]-i)/(filterbank[5]-filterbank[4]))*magnitude[h-1];
                filterbank_energies[4] += ((i-filterbank[4])/(filterbank[5]-filterbank[4]))*magnitude[h-1];
        }
        else if(h == filterbank[5]){
            filterbank_energies[4] += magnitude[h-1];
        }
        else if((i > filterbank[5]) && (i < filterbank[6])){
                filterbank_energies[4] += ((filterbank[6]-i)/(filterbank[6]-filterbank[5]))*magnitude[h-1];
                filterbank_energies[5] += ((i-filterbank[5])/(filterbank[6]-filterbank[5]))*magnitude[h-1];
        }
        else if(h == filterbank[6]){
            filterbank_energies[5] += magnitude[h-1];
        }
        else if((i > filterbank[6]) && (i < filterbank[7])){
                filterbank_energies[5] += ((filterbank[7]-i)/(filterbank[7]-filterbank[6]))*magnitude[h-1];
                filterbank_energies[6] += ((i-filterbank[6])/(filterbank[7]-filterbank[6]))*magnitude[h-1];
        }
        else if(h == filterbank[7]){
            filterbank_energies[6] += magnitude[h-1];
        }
        else if((i > filterbank[7]) && (i < filterbank[8])){
                filterbank_energies[6] += ((filterbank[8]-i)/(filterbank[8]-filterbank[7]))*magnitude[h-1];
                filterbank_energies[7] += ((i-filterbank[7])/(filterbank[8]-filterbank[7]))*magnitude[h-1];
        }
        else if(h == filterbank[8]){
            filterbank_energies[7] += magnitude[h-1];
        }
        else if((i > filterbank[8]) && (i < filterbank[9])){
                filterbank_energies[7] += ((filterbank[9]-i)/(filterbank[9]-filterbank[8]))*magnitude[h-1];
                filterbank_energies[8] += ((i-filterbank[8])/(filterbank[9]-filterbank[8]))*magnitude[h-1];
        }
        else if(h == filterbank[9]){
            filterbank_energies[8] += magnitude[h-1];
        }
        else if((i > filterbank[9]) && (i < filterbank[10])){
                filterbank_energies[8] += ((filterbank[10]-i)/(filterbank[10]-filterbank[9]))*magnitude[h-1];
                filterbank_energies[9] += ((i-filterbank[9])/(filterbank[10]-filterbank[9]))*magnitude[h-1];
        }
        else if(h == filterbank[10]){
            filterbank_energies[9] += magnitude[h-1];
        }
        else if((i > filterbank[10]) && (i < filterbank[11])){
                filterbank_energies[9] += ((filterbank[11]-i)/(filterbank[11]-filterbank[10]))*magnitude[h-1];
                filterbank_energies[10] += ((i-filterbank[10])/(filterbank[11]-filterbank[10]))*magnitude[h-1];
        }
        else if(h == filterbank[11]){
            filterbank_energies[10] += magnitude[h-1];
        }
        else if((i > filterbank[11]) && (i < filterbank[12])){
                filterbank_energies[10] += ((filterbank[12]-i)/(filterbank[12]-filterbank[11]))*magnitude[h-1];
                filterbank_energies[11] += ((i-filterbank[11])/(filterbank[12]-filterbank[11]))*magnitude[h-1];
        }
        else if(h == filterbank[12]){
            filterbank_energies[11] += magnitude[h-1];
        }
        else if((i > filterbank[12]) && (i < filterbank[13])){
                filterbank_energies[11] += ((filterbank[13]-i)/(filterbank[13]-filterbank[12]))*magnitude[h-1];
                filterbank_energies[12] += ((i-filterbank[12])/(filterbank[13]-filterbank[12]))*magnitude[h-1];
        }
        else if(h == filterbank[13]){
            filterbank_energies[12] += magnitude[h-1];
        }
        else if((i > filterbank[13]) && (i < filterbank[14])){
                filterbank_energies[12] += ((filterbank[14]-i)/(filterbank[14]-filterbank[13]))*magnitude[h-1];
                filterbank_energies[13] += ((i-filterbank[13])/(filterbank[14]-filterbank[13]))*magnitude[h-1];
        }
        else if(h == filterbank[14]){
            filterbank_energies[13] += magnitude[h-1];
        }
        else if((i > filterbank[14]) && (i < filterbank[15])){
                filterbank_energies[13] += ((filterbank[15]-i)/(filterbank[15]-filterbank[14]))*magnitude[h-1];
                filterbank_energies[14] += ((i-filterbank[14])/(filterbank[15]-filterbank[14]))*magnitude[h-1];
        }
        else if(h == filterbank[15]){
            filterbank_energies[14] += magnitude[h-1];
        }
        else if((i > filterbank[15]) && (i < filterbank[16])){
                filterbank_energies[14] += ((filterbank[16]-i)/(filterbank[16]-filterbank[15]))*magnitude[h-1];
                filterbank_energies[15] += ((i-filterbank[15])/(filterbank[16]-filterbank[15]))*magnitude[h-1];
        }
        else if(h == filterbank[16]){
            filterbank_energies[15] += magnitude[h-1];
        }
        else if((i > filterbank[16]) && (i < filterbank[17])){
                filterbank_energies[15] += ((filterbank[17]-i)/(filterbank[17]-filterbank[16]))*magnitude[h-1];
                filterbank_energies[16] += ((i-filterbank[16])/(filterbank[17]-filterbank[16]))*magnitude[h-1];
        }
        else if(h == filterbank[17]){
            filterbank_energies[16] += magnitude[h-1];
        }
        else if((i > filterbank[17]) && (i < filterbank[18])){
                filterbank_energies[16] += ((filterbank[18]-i)/(filterbank[18]-filterbank[17]))*magnitude[h-1];
                filterbank_energies[17] += ((i-filterbank[17])/(filterbank[18]-filterbank[17]))*magnitude[h-1];
        }
        else if(h == filterbank[18]){
            filterbank_energies[17] += magnitude[h-1];
        }
        else if((i > filterbank[18]) && (i < filterbank[19])){
                filterbank_energies[17] += ((filterbank[19]-i)/(filterbank[19]-filterbank[18]))*magnitude[h-1];
                filterbank_energies[18] += ((i-filterbank[18])/(filterbank[19]-filterbank[18]))*magnitude[h-1];
        }
        else if(h == filterbank[19]){
            filterbank_energies[18] += magnitude[h-1];
        }
        else if((i > filterbank[19]) && (i < filterbank[20])){
                filterbank_energies[18] += ((filterbank[20]-i)/(filterbank[20]-filterbank[19]))*magnitude[h-1];
                filterbank_energies[19] += ((i-filterbank[19])/(filterbank[20]-filterbank[19]))*magnitude[h-1];
        }
        else if(h == filterbank[20]){
            filterbank_energies[19] += magnitude[h-1];
        }
        else if((i > filterbank[20]) && (i < filterbank[21])){
                filterbank_energies[19] += ((filterbank[21]-i)/(filterbank[21]-filterbank[20]))*magnitude[h-1];
                filterbank_energies[20] += ((i-filterbank[20])/(filterbank[21]-filterbank[20]))*magnitude[h-1];
        }
        else if(h == filterbank[21]){
            filterbank_energies[20] += magnitude[h-1];
        }
        else if((i > filterbank[21]) && (i < filterbank[22])){
                filterbank_energies[20] += ((filterbank[22]-i)/(filterbank[22]-filterbank[21]))*magnitude[h-1];
                filterbank_energies[21] += ((i-filterbank[21])/(filterbank[22]-filterbank[21]))*magnitude[h-1];
        }
        else if(h == filterbank[22]){
            filterbank_energies[21] += magnitude[h-1];
        }
        else if((i > filterbank[22]) && (i < filterbank[23])){
                filterbank_energies[21] += ((filterbank[23]-i)/(filterbank[23]-filterbank[22]))*magnitude[h-1];
                filterbank_energies[22] += ((i-filterbank[22])/(filterbank[23]-filterbank[22]))*magnitude[h-1];
        }
        else if(h == filterbank[23]){
            filterbank_energies[22] += magnitude[h-1];
        }
        else if((i > filterbank[23]) && (i < filterbank[24])){
                filterbank_energies[22] += ((filterbank[24]-i)/(filterbank[24]-filterbank[23]))*magnitude[h-1];
                filterbank_energies[23] += ((i-filterbank[23])/(filterbank[24]-filterbank[23]))*magnitude[h-1];
        }
        else if(h == filterbank[24]){
            filterbank_energies[23] += magnitude[h-1];
        }
        else if((i > filterbank[24]) && (i < filterbank[25])){
                filterbank_energies[23] += ((filterbank[25]-i)/(filterbank[25]-filterbank[24]))*magnitude[h-1];
                filterbank_energies[24] += ((i-filterbank[24])/(filterbank[25]-filterbank[24]))*magnitude[h-1];
        }
        else if(h == filterbank[25]){
            filterbank_energies[24] += magnitude[h-1];
        }
        else if((i > filterbank[25]) && (i < filterbank[26])){
                filterbank_energies[24] += ((filterbank[26]-i)/(filterbank[26]-filterbank[25]))*magnitude[h-1];
                filterbank_energies[25] += ((i-filterbank[25])/(filterbank[26]-filterbank[25]))*magnitude[h-1];
        }
        else if(h == filterbank[26]){
            filterbank_energies[25] += magnitude[h-1];
        }
        else if((i > filterbank[26]) && (i < filterbank[27])){
                filterbank_energies[25] += ((i-filterbank[26])/(filterbank[27]-filterbank[26]))*magnitude[h-1];
        }
    }
}

void logEnergies(float * filterbank_energies){
    int i;
    for(i = 0; i < 26; i++){
        filterbank_energies[i] = (float)log10((double)filterbank_energies[i]);
    }
}
