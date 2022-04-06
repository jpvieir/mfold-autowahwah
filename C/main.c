// WAHWAH M FOLD FILTER
// este codigo implementa um filtro wah wah que gera um efeito automatico de wahwah a partir de M filtros passa banda
// com frequencia de corte variavel, seguindo uma onda senoidal
// sao introduzidos como parametros uma frequencia minima e maxima de corte para os passa banda,
// uma frequencia de oscilacao do wahwah, e a frequencia de banda
// foi usada a estrutura de bandpass da p. 50 do DAFX, em que 5 coeficientes compoem o filtro
// o coef. d depende da frequencia de corte (no caso deste codigo, d varia pois FC eh variavel)

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#define M 5


int main(void) {
 

// declaracao dos arquivos
	FILE * signal;
	FILE * output;

// variaveis
	short amostra,saida; // leitura e escrita
	int minf = 20; int maxf = 5000;
	int incf = (maxf - minf)/M; 
	float fmed, finc;
	int i=0; int fs=16000;
	int l =1;
	float fosc = 1;
	float F1,y,bp, mod; float mix = 0.8;
	float fb= 80; float input;
   // coefs lowpass
   float c,d; float alp[3*M], xh[3*M];
   float f[fs];
   float amp;


// defining the input and output signal:
    signal = fopen("whitenoise.pcm", "rb");
    if (signal == NULL){ // checking if the file was opened
      printf("Error opening file.\n");
    }
    output = fopen("whitenoiseWahWahAP2.pcm", "wb");
    if (output == NULL){ // checking if the file was opened
      printf("Error opening file.\n");
    }
  
    finc = (maxf - minf)/M;
    amp = (finc)/2;
    fmed = (minf + minf + finc)/2;

  for(i=0;i<(fs);i++){
       f[i] = fmed+ amp*sin(2*M_PI*1*i/fs);
   }

  l=0;
  for(i=0;i<3*M;i++){
      xh[i]=0;
      alp[i] = 0;
  }
  c=(tan(M_PI*fb/fs)-1)/(tan(M_PI*fb/fs)+1); l=0;
  
  while(fread(&amostra, sizeof(short),1,signal)==1){
  
    y=0;
  
    for(i=0;i<M;i++){

      xh[2+3*i] = amostra;  
      if(1){d=-cos(2*M_PI*(f[l]+i*finc)/fs);}
      //else{d=-cos(2*M_PI*(f[l+fs/2]+i*finc)/fs);}
      
      // y(n) = -c*x(n) + d*(1-c)*x(n-1) + x(n-2) -d*(1-c)*y(n-1) + c*y(n-2)  
      alp[2+3*i] = -c*xh[2+3*i] + d*(1-c)*xh[1+3*i] + xh[0+3*i] - d*(1-c)*alp[1+3*i] + c*alp[0+3*i];
      alp[0+3*i] = alp[1+3*i]; alp[1+3*i] = alp[2+3*i]; xh[0+3*i] = xh[1+3*i]; xh[1+3*i] = xh[2+3*i];
      bp = (0.5)*(amostra - alp[2+3*i]);
      y = y + bp;}


    l++;
    if(l==fs){l=0;}
	 // y= y/120;
   // y = (1-mix)*amostra + (mix)*y;
    //y = 2*y;
    saida=(short)y;
    fwrite(&saida,sizeof(short),1,output);
  }

  printf("Operation ended.");
  fclose(signal);
  fclose(output);

  return 0;

}