#ifndef __LED_H
#define __LED_H

void LED_Init(void);
void LED_SetBlue(void);
void LED_SetYellow(void);
void LED_SetGreen(void);
void LED_BlinkRed(float freq_hz, int duration_ms);
float Get_BlinkFreq_ByPPM(float ppm, float ppm_danger);

#endif
