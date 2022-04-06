#include <FixedPoints.h>
#include <FixedPointsCommon.h>
#define M 2
#define fs 16000

//functions
SQ15x16 pwr(SQ15x16 var1, short var2);
SQ15x16 sinefixed(SQ15x16 arg);
SQ15x16 tangentfixed(SQ15x16 argt);
SQ15x16 cosinefixed(SQ15x16 arg);
SQ15x16 sqrtfixed(SQ15x16 argsq);
//#define FIXED_POINTS_USE_NAMESPACE
//#define FIXED_POINTS_NO_RANDOM
short in_ADC0, in_ADC1;  //variables for 2 ADCs values (ADC0, ADC1)
int POT0, POT1, POT2;
short out_DAC0, out_DAC1; //variables for 3 pots (ADC8, ADC9, ADC10)
int LED = 3;
int FOOTSWITCH = 7; 
int TOGGLE = 2; 
int n=0; int i; 
//float amostra,c,d,tg;
int minf, maxf;
//float amp; float LFO[fs]; float fc[fs];
//float bp[2*M], hp[2*M],lp[2*M], Q,F1,y, lowp; float fb = 70;
SQ15x16 xh[3*M],alp[3*M],F1,y, fc[fs], amp, amostra, fmed, finc; 
SQ15x16 Q, argsin, sqrtQ; short Qt;
SQ15x16 c,tg,d,bp; short fb;
short t1,t2,cycle_time;
const SQ15x16 pi = 3.14159; const SQ15x16 fsfixed = 16000.00; const SQ15x16 hundred = 100; const SQ15x16 erromax = 0.001; 
const SQ15x16 one = 1.00; const SQ15x16 half = 0.50;
const SQ15x16 mix = 0.3;
float fosc;

/// functions:

void fvalues(){
  fb=100;
  minf = 20; maxf = 3500; fosc = 1;
  finc = (maxf - minf)/M;
  if(M>1){
  fmed = (minf + (minf + finc))/2;}
  else{
  fmed = (maxf + minf)/2;}
  
  amp = finc/2;

  for(n=0;n<(fs);n++){
    //LFO[n] = amp*sin(2*M_PI*1*n/fs);}
    fc[n] = fmed + amp*sin(2*M_PI*fosc*n/fs);
  }

  for(n=0;n<3*M;n++){alp[i]=0;xh[i]=0;} 
  c=(tan(M_PI*fb/fs)-1)/(tan(M_PI*fb/fs)+1);
  n=0;
}

// power
SQ15x16 pwr(SQ15x16 var1, short var2){
  SQ15x16 power = 1; int i;
  for(i=1;i<=var2;i++){
    power = power * var1;}
    return power;
  }
// sin 
SQ15x16 sinefixed(SQ15x16 arg){

  SQ15x16 factor3, factor5, factor7,factor9, a2, a3,a5,a7,a9;
  factor3 = -1.0/6.0; factor5= 1.0/120.0; factor7= -1.0/5040.0; factor9 = 1.0/362880.0;
  a3 = pwr(arg,3); a2 = pwr(arg,2); a5 = a3*a2; a7= a5*a2; a9= a7*a2;
  return (arg*1 + factor3*a3 + factor5*a5 + factor7*a7 + factor9*a9);
  }
// tg
SQ15x16 tangentfixed(SQ15x16 argt){
  SQ15x16 factor3, factor5, factor7,factor9, a2, a3,a5,a7,a9;
  factor3 = 1.0/3.0; factor5 = 2.0/15.0; factor7 = 17.0/315.0; factor9 = 62.0/2835.0; 
  a3 = pwr(argt,3); a2=pwr(argt,2); a5 = a3*a2; a7 = a5*a2; a9 = a7*a2;
  return (argt + factor3*a3 + factor5*a5 + factor7*a7 + factor9*a9);
  }
// cos
SQ15x16 cosinefixed(SQ15x16 arg){

  SQ15x16 factor2, factor4, factor6,factor8, a2, a4,a6,a8;
  factor2 = -1.0/2.0; factor4= 1.0/24.0; factor6= -1.0/720.0; factor8 = 1.0/40320.0;
  a2 = pwr(arg,2); a4 = a2*a2; a6= a4*a2; a8= a6*a2;
  return (1 + factor2*a2 + factor4*a4 + factor6*a6 + factor8*a8);
  }

// squareroot
SQ15x16 sqrtfixed(SQ15x16 argsq){
  SQ15x16 mid,endd, start, temp,k;
  short l=0;

  start = one/argsq;
  if(start > one){
    while(start>one){
      argsq = hundred*argsq;
      start = one/argsq;
      l++;}
  }
    
  start = 0; endd = argsq;
  while (endd- start >= erromax){
    mid = (start + endd)*half;
    if ((mid*mid) < temp){start = mid;}
    else{endd = mid;}
  }    
    return (mid/pwr(10,l));
}
