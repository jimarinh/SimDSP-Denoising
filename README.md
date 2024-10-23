# SimDSP-Denosing
## Test denosing on SimDSP for Android

This program implements a FFT-based denosing. To run this program, you need to install SimDSP for Android and the compiler toolchain as described in [SimDSP Project WebSite](https://github.com/parrado/SimDSP-Android). 

Once SimDSP is installed, download or clone this repository, and choose the option *Terminal > Run Build Task* on *VisualStudio Code*.  

It implements hard-threshold denoising by splitting the signal in blocks with an overlap of 25%. It uses windowing to reduce musical noise.