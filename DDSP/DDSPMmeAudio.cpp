/*-
 * Copyright (c) discoDSP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by discoDSP
 *        http://www.discodsp.com/ and contributors.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include ".\ddspmmeaudio.h"

CDDSPMmeAudio::CDDSPMmeAudio(void)
{
	mme_out_wave_header=NULL;
	mme_out_dsp_buffer=NULL;
	mme_out_data_buffer=NULL;

	mme_out_audio_running=0;
	mme_out_num_buffers=0;
	mme_out_current_buffer=0;
}

CDDSPMmeAudio::~CDDSPMmeAudio(void)
{
	AudioOutClose();
	AudioOutDestroyDataBuffers();
}

void CALLBACK mme_wo_proc(HWAVEOUT hwo,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{
	// get ddsp mme audio objectpointer
	CDDSPMmeAudio* po=(CDDSPMmeAudio*)dwInstance;
	
	// safe synchro
	// papp_dlg->mme_synchro.Lock();

	// feed mme buffer
	if(uMsg==WOM_DONE)
	{
		if(po->AudioOutIsRunning())
			po->AudioOutFillNextBuffer();
	}

	// release synchro
	// papp_dlg->mme_synchro.Unlock();
}

int CDDSPMmeAudio::AudioOutInit(int const driver,int const num_buffers,int const buffer_samples,int const sample_rate,int const num_channels,int const bit_depth,void (*pdspcb)(double**,int const,int const))
{
	// set output dsp callback
	mme_out_dsp_callback=pdspcb;

	// initalize driver, size, and buffer count
	mme_out_driver=driver;
	mme_out_buffer_samples=buffer_samples;
	mme_out_num_buffers=num_buffers;
	mme_out_current_buffer=0;
	
	// initialize the waveformatex
	mme_out_wave_format.wFormatTag		=	WAVE_FORMAT_PCM;
	mme_out_wave_format.nChannels		=	num_channels;
	mme_out_wave_format.nSamplesPerSec	=	sample_rate;
	mme_out_wave_format.wBitsPerSample	=	bit_depth;
	mme_out_wave_format.nBlockAlign		=	mme_out_wave_format.nChannels*(mme_out_wave_format.wBitsPerSample/8);
	mme_out_wave_format.nAvgBytesPerSec	=	mme_out_wave_format.nSamplesPerSec*mme_out_wave_format.nBlockAlign;
	mme_out_wave_format.cbSize			=	0;
	
	// enable audio semaphore
	mme_out_audio_running=1;

	// open the preferred Digital Audio Out device. Note: myWindow is a handle to some open window */
	if(waveOutOpen(&mme_out_handle,mme_out_driver,&mme_out_wave_format,(DWORD)mme_wo_proc,(DWORD)this,CALLBACK_FUNCTION)!=MMSYSERR_NOERROR)
	{
		// disable audio semaphore
		mme_out_audio_running=0;

		// waveOutOpen failed
		return 0;
	}
	else
	{
		// create waveheaders
		mme_out_wave_header=new WAVEHDR[num_buffers];

		// create mme data buffers pointers
		mme_out_data_buffer=new char*[num_buffers];
		
		// get mme output data size in bytes
		int const mme_out_data_size=buffer_samples*num_channels*(bit_depth/8);

		// create and clear data buffers and clear headers
		for(int cb=0;cb<num_buffers;cb++)
		{
			mme_out_data_buffer[cb]=new char[mme_out_data_size];
			memset(mme_out_data_buffer[cb],0,mme_out_data_size);
			memset(&mme_out_wave_header[cb],0,sizeof(WAVEHDR));
		}

		// create dsp channel pointers
		mme_out_dsp_buffer=new double*[num_channels];

		// create dsp channel buffers
		for(int ch=0;ch<num_channels;ch++)
			mme_out_dsp_buffer[ch]=new double[buffer_samples];

		// fill all the sound buffers
		for(int cb=0;cb<num_buffers;cb++)
			AudioOutFillNextBuffer();

		// wave out running
		return 1;
	}

	// unhandled error
	return 0;
}

int CDDSPMmeAudio::AudioOutClose(void)
{
	// switch off audio semaphore
	mme_out_audio_running=0;

	// reset audio output
	if(waveOutReset(mme_out_handle)==MMSYSERR_NOERROR)
	{
		// unprepare headers
		for(int cb=0;cb<mme_out_num_buffers;cb++)
		{
			if(mme_out_wave_header[cb].dwFlags & WHDR_PREPARED)
				waveOutUnprepareHeader(mme_out_handle,&mme_out_wave_header[cb],sizeof(WAVEHDR));
		}
	
		// close audio
		if(waveOutClose(mme_out_handle)==MMSYSERR_NOERROR)
		{
			// destroy data buffers
			AudioOutDestroyDataBuffers();

			// closed ok
			return 1;
		}
		else
		{
			// waveOutClose failed
			return 0;
		}
	}
	else
	{
		// waveOutReset failed 
		return 0;
	}

	// unhandled error
	return 0;
}

void CDDSPMmeAudio::AudioOutFillNextBuffer(void)
{
	// check if the buffer is already prepared (should not !)
	if(mme_out_wave_header[mme_out_current_buffer].dwFlags & WHDR_PREPARED)
		waveOutUnprepareHeader(mme_out_handle,&mme_out_wave_header[mme_out_current_buffer],sizeof(WAVEHDR));

	// call the user function to fill the buffer with anything you want
	mme_out_dsp_callback(mme_out_dsp_buffer,mme_out_wave_format.nChannels,mme_out_buffer_samples);

	// sample counter
	int sc=0;

	// 16 bit
	if(mme_out_wave_format.wBitsPerSample==16)
	{
		short* psampledata=(short*)mme_out_data_buffer[mme_out_current_buffer];

		// amplify and output
		for(int s=0;s<mme_out_buffer_samples;s++)
		{
			for(int ch=0;ch<mme_out_wave_format.nChannels;ch++)
				psampledata[sc++]=short(mme_out_dsp_buffer[ch][s]*32767.0);
		}
	}
	
	// set up the waveheader and buffer to be sent to the waveout API
	WAVEHDR* pwoh=&mme_out_wave_header[mme_out_current_buffer];
	
	// fill current header
	pwoh->lpData=(char*)mme_out_data_buffer[mme_out_current_buffer];
	pwoh->dwBufferLength=mme_out_buffer_samples*(mme_out_wave_format.wBitsPerSample/8)*mme_out_wave_format.nChannels;
	pwoh->dwFlags=0;
	
	// prepare header
	waveOutPrepareHeader(mme_out_handle,pwoh,sizeof(WAVEHDR));

	// send the header info to the waveout queue
	waveOutWrite(mme_out_handle,pwoh,sizeof(WAVEHDR));

	// flip buffers
	if((++mme_out_current_buffer)>=mme_out_num_buffers)
		mme_out_current_buffer=0;
}

int CDDSPMmeAudio::AudioOutIsRunning(void)
{
	return mme_out_audio_running;
}

int	CDDSPMmeAudio::AudioOutGetSampleRate(void)
{
	if(mme_out_audio_running)
		return mme_out_wave_format.nSamplesPerSec;

	// 44100 default
	return 44100;
}

void CDDSPMmeAudio::AudioOutDestroyDataBuffers(void)
{
	// delete wave headers
	if(mme_out_wave_header!=NULL)
	{
		delete[] mme_out_wave_header;
		mme_out_wave_header=NULL;
	}

	// delete mme buffers
	if(mme_out_data_buffer!=NULL)
	{
		for(int cb=0;cb<mme_out_num_buffers;cb++)
			delete[] mme_out_data_buffer[cb];

		// delete data buffer pointers
		delete[] mme_out_data_buffer;

		// set null data buffer
		mme_out_data_buffer=NULL;
	}

	// delete dsp buffers
	if(mme_out_dsp_buffer!=NULL)
	{
		for(int ch=0;ch<mme_out_wave_format.nChannels;ch++)
			delete[] mme_out_dsp_buffer[ch];

		// delete dsp buffer pointers
		delete[] mme_out_dsp_buffer;

		// set null dsp buffer
		mme_out_dsp_buffer=NULL;
	}
}
