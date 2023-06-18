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

#include "stdio.h"

struct CDDSP_MIDI_EVENT
{
	double			beat_pos;
	double			note_length;
	
	unsigned char	mid0;
	unsigned char	mid1;
	unsigned char	mid2;
	unsigned char	mid3;
	unsigned char	trck;
	unsigned char	res1;
	unsigned char	res2;
	unsigned char	res3;
};

class CDDSPMidiEvents
{
public:
	CDDSPMidiEvents(void);
	~CDDSPMidiEvents(void);

public:
	void	AddNewEvent(double const beat_pos,double const note_length,unsigned char const mid0,unsigned char const mid1,unsigned char const mid2,unsigned char const mid3,unsigned char const track);
	int		ImportMidiFile(char* pfilename,bool const clear_events);
	int		GetNumEvents(void);

private:
	long	MidiReadLong(FILE* pfile);
	short	MidiReadShort(FILE* pfile);
	long	ReadMidiVarLen(FILE* pfile);
	void	ReadMidiEvent(long& Mf_currtime,FILE* pfile,unsigned char &run_stat,double &d_ppqn);
	void	OnMidiNoteOff(double const d_event_pos,int const channel,int const note,int const velo);
	void	OnMidiNoteOn(double const d_event_pos,int const channel,int const note,int const velo);
	void	OnMidiSysEx(double const d_event_pos,unsigned char const stat,FILE* pfile);

private:
	int					num_events;
	CDDSP_MIDI_EVENT*	pevents;
};
