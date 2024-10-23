// Fundamentos de Procesamiento Digital de Se�ales: Teor�a e Implementaci�n Pr�ctica
// Jorge Iv�n Mar�n-Hurtado, Alexander L�pez-Parrado, Luis Miguel Capacho-Valbuena
// ISBN: 978-958-48-2955-9
// (c) 2017
// =============================================================================================
// C�digo fuente de la Secci�n 6.2 para el c�lculo de la Transformada R�pida de Fourier (FFT).
//
//  Funci�n para calcular la FFT para vectores con el formato
//
//  |R|I|R|I| ... |R|I|
//  con cada valor |R| o |I| de tipo float.
//
//  Los datos deben ser entrados con el ordenamiento bit-reversal
// =============================================================================================

void fbasicfft( float* in, float *out, int Nfft, int inv );

#define  fft(in, out, N) fbasicfft(in, out, N, 1)
#define ifft(in, out, N) fbasicfft(in, out, N, -1)

