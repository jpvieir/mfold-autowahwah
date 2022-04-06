#include "func.h"

///////////////
void setup()
{
    Serial.begin(9600);
    fvalues();
    //turn on the timer clock in the power management controller
    pmc_set_writeprotect(false);
    pmc_enable_periph_clk(ID_TC4);
  
    //we want wavesel 01 with RC
    TC_Configure(TC1, 1, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK2);
    TC_SetRC(TC1, 1, 656); // sets <> 8 Khz interrupt rate
    //8KHz,  value = 1312 (10500000/8000 = 1312) --------------- 8KHz (1/8KHz = 125us)
    //16KHz, value = 656 (10500000/16000 = 656)
    //32KHz, value = 328 (10500000/32000 = 328)
    //44.1Hz, value = 238 (10500000/44100 = 238) -------------- 44.1KHz (1/44KHz=22.6us)
    //48KHz, value = 218 (10500000/48000 = 218)
    //88.2KHz, value = 119 (10500000/88200 = 119)
    //96KHz, value = 109  (10500000/96000 = 109)
  
    TC_Start(TC1, 1);
  
    // enable timer interrupts on the timer
    TC1->TC_CHANNEL[1].TC_IER = TC_IER_CPCS;
    TC1->TC_CHANNEL[1].TC_IDR = ~TC_IER_CPCS;
  
    //Enable the interrupt in the nested vector interrupt controller
    //TC4_IRQn where 4 is the timer number * timer channels (3) + the channel number
    //(=(1*3)+1) for timer1 channel1
    NVIC_EnableIRQ(TC4_IRQn);
  
    //ADC Configuration
    ADC->ADC_MR |= 0x80;   // DAC in free running mode.
    ADC->ADC_CR = 2;       // Starts ADC conversion.
    ADC->ADC_CHER = 0x1CC0; // Enable ADC channels 0,1,8,9 and 10
  
  
    //pedalSHIELD pin configuration
    pinMode(LED, OUTPUT);
    pinMode(FOOTSWITCH, INPUT_PULLUP);
    pinMode(TOGGLE, INPUT_PULLUP);
 
   //DAC Configuration
   analogWrite(DAC0,0);  // Enables DAC0
 //  analogWrite(DAC1,0);  // Enables DAC0

 //for(n=0;n<8000;n++){Serial.println(LFO[n]);}
}
 
void loop()
{
  //Read the ADCs
  while((ADC->ADC_ISR & 0x1CC0)!=0x1CC0);// wait for ADC 0, 1, 8, 9, 10 conversion complete.
  in_ADC0=ADC->ADC_CDR[7];               // read data from ADC0
  in_ADC1=ADC->ADC_CDR[6];               // read data from ADC1  
  POT0=ADC->ADC_CDR[10];                 // read data from ADC8        
  POT1=ADC->ADC_CDR[11];                 // read data from ADC9   
  POT2=ADC->ADC_CDR[12];                 // read data from ADC10 
     

   // fc = map(POT1,0,4095,400,1200);
   // fb = map(POT0,0,4095,10,150);
    
   //Turn on the LED if the effect is ON.
    if (digitalRead(FOOTSWITCH)) digitalWrite(LED, HIGH);
    else  digitalWrite(LED, LOW);

    Serial.println(cycle_time);
    //Serial.println(fb);
}

void TC4_Handler(){
    
    //Clear status allowing the interrupt to be fired again.
    TC_GetStatus(TC1, 1);
       
 // filtro:
 t1 = micros();
 //fb = map(POT1, 0, 4095, 40,200);
  y=0;
 amostra = in_ADC0 - 2048;
 for(i=0;i<M;i++){
      xh[2+3*i] = amostra;
      d=-cosinefixed(2*pi*(fc[n]+ i*finc)/fsfixed);
      // y(n) = -c*x(n) + d*(1-c)*x(n-1) + x(n-2) -d*(1-c)*y(n-1) + c*y(n-2)  
      alp[2+3*i] = -c*xh[2+3*i]+ d*(1-c)*xh[1+3*i] + xh[0+3*i] - d*(1-c)*alp[1+3*i] + c*alp[0+3*i];
      alp[0+3*i] = alp[1+3*i]; alp[1+3*i] = alp[2+3*i]; xh[0+3*i] = xh[1+3*i]; xh[1+3*i] = xh[2+3*i];
      bp = (0.5)*(xh[2+3*i] - alp[2+3*i]);
      y = y + bp;
 }
 
     n++;
    if(n==fs){n=0;}
    //y = 0.2*amostra + 0.8*y;
    //y = 2*y;
 // saida:
    out_DAC0= static_cast<short>(y+2048);
   // out_DAC0= map(out_DAC0, 0, 4095, 1, POT2);

    t2 = micros();
    cycle_time = t2 - t1;
    //out_DAC0=map(out_DAC0,0,4095,1,POT2);
   // out_DAC1=map(in_ADC1,0,4095,1,POT2);
  
  
    //Write the DACs
    dacc_set_channel_selection(DACC_INTERFACE, 0);       //select DAC channel 0
    dacc_write_conversion_data(DACC_INTERFACE, out_DAC0);//write on DAC
    dacc_set_channel_selection(DACC_INTERFACE, 1);       //select DAC channel 1
    dacc_write_conversion_data(DACC_INTERFACE, 0);       //write on DAC
  }
