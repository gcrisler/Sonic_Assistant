/*
 * algorithmsSonic.h
 *
 *  Created on: Aug 28, 2019
 *      Author: gcrisler
 */

#ifndef ALGORITHMSSONIC_H_
#define ALGORITHMSSONIC_H_

#include <msp430.h>

#include <stdint.h>

int EMA_function(float alpha, long int latest, int stored);

typedef struct Sonic_Algorithms_Results_fixed_point_
{

    int32_t Dema;   //Double exponential moving average.

}Sonic_Results_fixed_point;


#endif /* ALGORITHMSSONIC_H_ */
