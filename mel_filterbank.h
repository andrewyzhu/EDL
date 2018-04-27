/*
 * mel_filterbank.h
 *
 *  Created on: Apr 24, 2018
 *      Author: samaustin
 */


#include "msp.h"
#include <math.h>

#ifndef MEL_FILTERBANK_H_
#define MEL_FILTERBANK_H_

void melFilterCenters(int * fftbin);

void melCoefficients(float * magnitude, int * filterbank, float * filterbank_energies);

void logEnergies(float * filterbank_energies);



#endif /* MEL_FILTERBANK_H_ */
