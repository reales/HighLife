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

#pragma once

#include "windows.h"
#include "mmsystem.h"

class CDDSPMmeAudio
{
public:
	CDDSPMmeAudio(void);
	~CDDSPMmeAudio(void);

public:
	int		AudioOutInit(int const driver,int const num_buffers,int const buffer_samples,int const sample_rate,int const num_channels,int const bit_depth,void (*pdspcb)(double**,int const,int const));
	void	AudioOutFillNextBuffer(void);
	int		AudioOutIsRunning(void);
	int		AudioOutGetSampleRate(void);
	int		AudioOutClose(void);

private:
	void	AudioOutDestroyDataBuffers(void);

private:
	// mme waveout handlers
	HWAVEOUT		mme_out_handle;
	WAVEFORMATEX	mme_out_wave_format;
	WAVEHDR*		mme_out_wave_header;
	int				mme_out_current_buffer;
	int				mme_out_driver;
	int				mme_out_num_buffers;
	int				mme_out_buffer_samples;
	int				mme_out_audio_running;
	void			(*mme_out_dsp_callback)(double** poutputs,int const num_channels,int const num_samples);

	// bit mme buffer holder
	char**			mme_out_data_buffer;
	
	// 64 bit dsp buffer holder
	double**		mme_out_dsp_buffer;
};
