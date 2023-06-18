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

struct DDSP_RIFF_WAVE_FRMT
{
	short          wFormatTag;
	unsigned short wChannels;
	unsigned long  dwSamplesPerSec;
	unsigned long  dwAvgBytesPerSec;
	unsigned short wBlockAlign;
	unsigned short wBitsPerSample;
};

struct DDSP_RIFF_WAVE_SMPL
{
	long dwManufacturer;
	long dwProduct;
	long dwSamplePeriod;
	long dwMIDIUnityNote;
	long dwMIDIPitchFraction;
	long dwSMPTEFormat;
	long dwSMPTEOffset;
	long cSampleLoops;
	long cbSamplerData;
};

struct DDSP_RIFF_WAVE_LOOP
{
	long dwIdentifier;
	long dwType;
	long dwStart;
	long dwEnd;
	long dwFraction;
	long dwPlayCount;
};

struct DDSP_RIFF_WAVE_INST
{
	unsigned char UnshiftedNote;
	char          FineTune;
	char          Gain;
	unsigned char LowNote;
	unsigned char HighNote;
	unsigned char LowVelocity;
	unsigned char HighVelocity;
};

class CDDSPRiffWave
{
public:
	CDDSPRiffWave(void);
	~CDDSPRiffWave(void);

public:
	int						ReadWave(HWND const hwnd, char const* pfilename);
	DDSP_RIFF_WAVE_FRMT*	GetFormat(void);
	DDSP_RIFF_WAVE_SMPL*	GetSample(void);
	DDSP_RIFF_WAVE_LOOP*	GetLoop(long const index);
	DDSP_RIFF_WAVE_INST*	GetInstrument(void);
	char*					GetData(void);
	unsigned long			GetDataLength(void);
	bool					has_smpl,has_inst;

private:
	// wave chunks
	DDSP_RIFF_WAVE_FRMT		chk_frmt;
	DDSP_RIFF_WAVE_SMPL		chk_smpl;
	DDSP_RIFF_WAVE_INST		chk_inst;

	// variable arrays
	char*					psnd_buffer;
	unsigned long			snd_buffer_length;
	DDSP_RIFF_WAVE_LOOP*	psmpl_loop;
};
