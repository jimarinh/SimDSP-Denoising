//Archivo de cabecera de SimDSP
#include <simdsp.h>


#include "fft_f.h"
#include "window.h"

#define Nfft 1024
#define Nx Nfft/2
#define Nstep Nfft/4
#define umbral 3e6

//Frecuencia de muestreo
#define F_S 44100

//Buffers para hacer el procesamiento interno de la convolución rápida
float X_n[Nfft*2];
float X_k[Nfft*2];
float Y_n[Nfft*2];
float Y_k[Nfft*2];
float Yadd[Nfft];

//Señal de salida
short y[Nstep];

//Bloque temporal  donde se almacenan las muestras del bloque anterior y las muestras nuevas
short xt[Nx]; 


//Funci�n para hacer el bitreversal1024 para un bloque de Nfft = 256
void bitreversal1024(float *x_n, float *x_k) {
	int cbr;
	for (int c = 0; c<Nfft; c++) {
			cbr = 	((c & 1) << 9) |
					((c & 2) << 7) |
					((c & 4) << 5) |
					((c & 8) << 3) |
					((c & 0x10) << 1) |
					((c & 0x20) >> 1) |
					((c & 0x40) >> 3) |
					((c & 0x80) >> 5) |
					((c & 0x100) >> 7) |
					((c & 0x200) >> 9);
			x_k[cbr*2] 	= x_n[2*c]; 	  //Parte real
			x_k[cbr*2+1]= x_n[2*c+1];   //Parte imaginaria
		}
}

//Rutina que simulará la ISR que se invoca una vez ha finalizado
//la transferencia por DMA, audio apunta a los datos que se procesarán
void process_data(short *x){

	int n,k;

  //Invoca captura por DMA nuevamente
  captureBlock(process_data );

  for (n=0;n<Nstep;n++) {
    xt[n+Nstep] = x[n]; 
  }

	//Completa el bloque de entrada con ceros, fija la parte imaginaria a 0.0, y enventana
	for(n=0;n<Nx;n++) {
		X_n[2*n]   = xt[n] * w[n];
		X_n[2*n+1] = 0.0;
	}
	for(;n<Nfft;n++) {
		X_n[2*n]   = 0.0;
		X_n[2*n+1] = 0.0;
	}
	
	//Calcula la FFT
	bitreversal1024(X_n,X_k);
	fft(X_k,X_k,Nfft);

	float maxmag = 0.0;
	
	//Aplica el denoising
	for(k=0;k<Nfft;k++) {
		float mag = X_k[2*k] * X_k[2*k] + X_k[2*k+1] * X_k[2*k+1];
		if(mag>maxmag) maxmag = mag;
		if (mag > umbral) {
			Y_k[2*k]   = X_k[2*k]; //Parte real
			Y_k[2*k+1] = X_k[2*k+1]; //Parte imaginaria
		} else {
			Y_k[2*k]   = 0.0; //Parte real
			Y_k[2*k+1] = 0.0; //Parte imaginaria
		}
	}

	//println("Max_mag %g", maxmag);

	//Calcula la IFFT 
	bitreversal1024(Y_k,Y_n);
	ifft(Y_n,Y_n,Nfft);

	//Aplica la adici�n de bloques de salida seg�n el m�todo overlap-add (descarta la parte imaginaria de y, pues es cero.
	for(n=0;n<Nfft-Nstep;n++) {
		Yadd[n] += Y_n[2*n];
	}
	for(;n<Nfft;n++) {
		Yadd[n] = Y_n[2*n];
	}
	
	//Escribe al buffer de salida
	for(n=0;n<Nstep;n++) {
		y[n] = Yadd[n];
	}
	
	//Prepara el buffer de adici�n para el siguiente traslape
	for(n=0;n<Nfft-Nstep;n++) {
		Yadd[n] = Yadd[n+Nstep];
	}
	
	//Prepara el buffer de entrada
	for(n=0;n<Nstep;n++) {
		xt[n] = xt[n+Nstep];
	}
	
  //Reproduce señal
  playBlock(y);


}


//Funcion de inicializacion
void dsp_setup()
{
  //Habilita sistema de audio del teléfono
  enableAudio(Nstep,F_S);

  for(int n=0; n<Nfft; n++) {
		Yadd[n] = 0.0;
	}


  //Imprime tasa de muestreo actual
  println("Sampling frequency is %d Hz and buffer size is %d samples",F_S,Nstep);

  //Inicia captura por DMA
  captureBlock(process_data );
}
