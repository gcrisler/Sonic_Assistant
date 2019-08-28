/*
 * algorithms_Sonic.c
 *
 *  Created on: Aug 28, 2019
 *      Author: gcrisler
 */

include "algorithmsSonic.h";
int sensor_pin = 0;

int ema_a = 0.06;
int ema_ema = 0;
int ema = 0;



void loop() {

  ema = EMA_function(ema_a, algorithms_Results.deltaTOF, ema);
  ema_ema = EMA_function(ema_a, ema, ema_ema);

  Sonic_Results_fixed_point.Dema = 2*ema - ema_ema;


}

int EMA_function(float alpha, int latest, int stored){
  return round(alpha*latest) + round((1-alpha)*stored);
}

