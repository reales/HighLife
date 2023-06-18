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

#include ".\ddspmidievents.h"
#include "memory.h"
#include "stdlib.h"
#include "string.h"
#include "windows.h"

CDDSPMidiEvents::CDDSPMidiEvents(void)
{
	num_events=0;
	pevents=NULL;
}

CDDSPMidiEvents::~CDDSPMidiEvents(void)
{
	if(num_events)
	{
		delete[] pevents;
		pevents=NULL;
		num_events=0;
	}
}

int CDDSPMidiEvents::GetNumEvents(void)
{
	return num_events;
}

void CDDSPMidiEvents::AddNewEvent(double const beat_pos,double const note_length,unsigned char const mid0,unsigned char const mid1,unsigned char const mid2,unsigned char const mid3,unsigned char const track)
{
	// allocate evens
	CDDSP_MIDI_EVENT *pnewevents=new CDDSP_MIDI_EVENT[num_events+1];

	// copy events
	for(int e=0;e<num_events;e++)
		pnewevents[e]=pevents[e];

	// new event
	CDDSP_MIDI_EVENT *pne=&pnewevents[num_events];

	// clear new event
	memset(pne,0,sizeof(CDDSP_MIDI_EVENT));

	// fill event structs
	pne->beat_pos=beat_pos;
	pne->note_length=note_length;
	pne->mid0=mid0;
	pne->mid1=mid1;
	pne->mid2=mid2;
	pne->mid3=mid3;
	pne->trck=track;

	// swap events
	CDDSP_MIDI_EVENT *poldevents=pevents;

	// point events to new event array
	pevents=pnewevents;

	// delete old events
	delete[] poldevents;

	// add event count
	num_events++;
}

long CDDSPMidiEvents::ReadMidiVarLen(FILE* pfile)
{
	// get value
	long value=fgetc(pfile);

	// check quantity length
	if(value&0x80)
	{
		value&=0x7f;

		int c;

		do
		{
			value=(value<<7)+((c=fgetc(pfile))&0x7f);
		}
		while(c&0x80);
	}

	return(value);
}

long CDDSPMidiEvents::MidiReadLong(FILE* pfile)
{
	unsigned long retval;
	int i;
	int c;

	// set retval to 0 initially
	retval=0L;

	// read in 4 bytes into retval
	for (i=0;i<4;i++)
	{
		c=fgetc(pfile);
		retval<<=8;
		retval|=(unsigned char)c;
	}

	// all went well
	return(retval);
}

short CDDSPMidiEvents::MidiReadShort(FILE* pfile)
{
	unsigned short retval;
	int i;
	int c;

	// set 'retval to 0 initially
	retval=0;

	// read in 2 bytes into 'retval' */
	for(i=0;i<2;i++)
	{
		c=fgetc(pfile);
		retval<<=8;
		retval|=(unsigned char)c;
	}

	// all went well
	return(retval);
}

int CDDSPMidiEvents::ImportMidiFile(char* pfilename,bool const clear_events)
{
	// clear events
	if(num_events)
	{
		delete[] pevents;
		pevents=NULL;
		num_events=0;
	}

	// open file
	FILE* pfile=fopen(pfilename,"rb");

	if(pfile==NULL)
		return 0;

	// buffer for chunk id input
	char kw_buff[4];

	// read in the first four characters into 'kw_buff'
	fread(kw_buff,sizeof(char),4,pfile);

	// see if 'kw_buff' contains "MThd"
	if(strncmp(kw_buff,"MThd",4)!=0)
		return 0;

	// read the number 6 (header length) from the header */
	unsigned long const header_len=MidiReadLong(pfile);

	// check for correct length
	if(header_len!=6)
		return 0;

	// read the file format
	unsigned short const format=MidiReadShort(pfile);

	// read num tracks
	unsigned short const ntrks=MidiReadShort(pfile);

	// read quarter note division
	unsigned short const division=MidiReadShort(pfile);

	// header info
	double d_division=double(division);

	// running status
	unsigned char running_stat=0;

	// read tracks
	for(int ti=0;ti<ntrks;ti++)
	{
		// reset 'Mf_currtime'. If the file format is 2, then it probably
		// doesn't matter anyway, but for the other types (single and
		// multiple tracks), 'Mf_currtime' should measure the time elapsed
		// since the beginning of the track.
		long midi_currtime=0L;

		// indicate that 'mf_eot()' hasn't been called yet
		int eot_called=0;

		// read in the first four characters into 'kw_buff'
		fread(kw_buff,4,1,pfile);

		// see if 'kw_buff' contains "MTrk"
		if(strncmp(kw_buff,"MTrk",4)!=0)
			return 0;

		// read the length of the track
		long tklen=MidiReadLong(pfile);

		// read the data in the track
		while(tklen)
		{
			long const curr_pos=ftell(pfile);
			ReadMidiEvent(midi_currtime,pfile,running_stat,d_division);
			long const read_bytes=ftell(pfile)-curr_pos;
			tklen-=read_bytes;
		}
	}

	// close file and return success
	fclose(pfile);
	return 1;
}

void CDDSPMidiEvents::ReadMidiEvent(long& Mf_currtime,FILE* pfile,unsigned char &run_stat,double &d_ppqn)
{
	// read delta-time (time since last event)
	long delta_time=ReadMidiVarLen(pfile);

	// update 'Mf_currtime'
	Mf_currtime+=delta_time;

	// read event type
	int const stat=fgetc(pfile);

	// parameter being currently read
	int cur_param;

	// parameters
	unsigned char params[2];

	// is it a new event type?
	if(stat&0x80)
	{
		// set new running status
		run_stat=(unsigned char)stat;

		// start reading at 0th param
		cur_param=0;
	}
	else
	{
		// record 1st parameter
		params[0]=(unsigned char)stat;

		// start reading at 1st param
		cur_param=1;			
	}

	// num params
	int num_params=0;

	// sysex or meta
	if(run_stat>=0xF0)
	{
		num_params=-1;
	}
	else
	{
		// parameters
		if(run_stat<0xC0 || run_stat>=0xE0)
			num_params=2;
		else
			num_params=1;
	}

	// read the parameters corresponding to the status byte
	for(int i=num_params-cur_param;i>0;i--,cur_param++)
	{
		// input character
		int const c=fgetc(pfile);

		// record parameter
		params[cur_param]=(unsigned char)c;
	}

	// get position
	double const d_event_pos=(double)Mf_currtime/(double)d_ppqn;

	// report event and parameters to user, depending on type of event
	switch(run_stat&0xF0)
	{
		// note off
	case 0x80: 
		OnMidiNoteOff(d_event_pos,run_stat&0xF,params[0],params[1]);
		return;

		// note on
	case 0x90:		
		OnMidiNoteOn(d_event_pos,run_stat&0xF,params[0],params[1]);
		return;

		// pressure
	case 0xA0:
		AddNewEvent(d_event_pos,0.0,run_stat,params[0],params[1],0,run_stat&0xF);
		return;

		// controller
	case 0xB0:
		AddNewEvent(d_event_pos,0.0,run_stat,params[0],params[1],0,run_stat&0xF);
		return;

		// program
	case 0xC0:
		AddNewEvent(d_event_pos,0.0,run_stat,params[0],0,0,run_stat&0xF);
		return;

		// channel pressure
	case 0xD0:
		AddNewEvent(d_event_pos,0.0,run_stat,params[0],0,0,run_stat&0xF);
		return;

		// pitch bend
	case 0xE0:
		AddNewEvent(d_event_pos,0.0,run_stat,params[0],params[1],0,run_stat&0xF);
		return;

		// sysex
	case 0xF0:
		OnMidiSysEx(d_event_pos,run_stat,pfile);
		return;
	}
}

void CDDSPMidiEvents::OnMidiNoteOff(double const d_event_pos,int const channel,int const note,int const velo)
{
	for(int e=0;e<num_events;e++)
	{
		CDDSP_MIDI_EVENT* pevent_nton_to_finish=&pevents[e];

		if(pevent_nton_to_finish->note_length==-1.0 && pevent_nton_to_finish->mid1==note && (pevent_nton_to_finish->mid0&0xF)==channel)
			pevent_nton_to_finish->note_length=d_event_pos-pevent_nton_to_finish->beat_pos;
	}
}

void CDDSPMidiEvents::OnMidiNoteOn(double const d_event_pos,int const channel,int const note,int const velo)
{
	if(velo>0)
		AddNewEvent(d_event_pos,-1.0,0x90+channel,note,velo,0,channel);
	else
		OnMidiNoteOff(d_event_pos,channel,note,0x40);
}

// META-EVENT MESSAGE TYPES
#define META_SEQNUM		0x00	/* Sequence number */
#define META_TEXT		0x01	/* Text event */
#define META_COPYRIGHT	0x02	/* Copyright notice */
#define META_SEQNAME	0x03	/* Sequence/track name */
#define META_INSTNAME	0x04	/* Instrument name */
#define META_LYRIC		0x05	/* Lyric */
#define META_MARKER		0x06	/* Marker */
#define META_CUEPT		0x07	/* Cue point */
#define META_EOT		0x2f	/* End of track */
#define META_TEMPO		0x51	/* Set tempo */
#define META_SMPTE		0x54	/* SMPTE offset */
#define META_TIMESIG	0x58	/* Time signature */
#define META_KEYSIG		0x59	/* Key signature */
#define META_SEQSPEC	0x7f	/* Sequencer-specific event */

unsigned long stol(unsigned char* str,int const len)
{
	unsigned long retval=0;

	for(int i=0;i<len;i++)
	{
		retval<<=8;
		retval|=str[i];
	}

	return retval;
}

void CDDSPMidiEvents::OnMidiSysEx(double const d_event_pos,unsigned char const stat,FILE* pfile)
{
	// temp stack
	int sharp,minor;
	int numer,denom,clocks,qnotes;
	int hour,min,sec,frame,fract;
	int tempo;

	short seqnum;

	// sysex event
	if(stat==0xF0)
	{
		// read length of meta-event
		long const length=ReadMidiVarLen(pfile);

		if(length>0)
		{
			// allocate memory for sysex-event
			unsigned char* data=new unsigned char[length];

			// read sysex
			fread(data,sizeof(unsigned char),length,pfile);

			// free memory
			delete[] data;
		}
	}

	// meta message
	if(stat==0xFF)
	{
		// type of meta-event
		int const msg_type=fgetc(pfile);

		// read length of meta-event
		long const length=ReadMidiVarLen(pfile);

		if(length>0)
		{
			// allocate memory for meta-event
			unsigned char* data=new unsigned char[length];

			// read meta event data
			fread(data,sizeof(unsigned char),length,pfile);

			// meta event parse
			switch(msg_type)
			{
				// sequence number
			case META_SEQNUM:
				seqnum=((short)data[0]<< 8)|data[1];
				break;

				// text event(s)
			case META_TEXT:
			case META_COPYRIGHT:
			case META_SEQNAME:
			case META_INSTNAME:
			case META_LYRIC:
			case META_MARKER:
			case META_CUEPT:
				break;

				// end of track
			case META_EOT:
				//				eot_called=1;
				break;

				// set tempo
			case META_TEMPO:
				tempo	=(int)stol((unsigned char*)data,3);
				break;

				// smpte offset
			case META_SMPTE:
				hour	=(int)data[0];
				min		=(int)data[1];
				sec		=(int)data[2];
				frame	=(int)data[3];
				fract	=(int)data[4];
				break;

				// time signature
			case META_TIMESIG:
				numer	=(int)data[0];
				denom	=(int)data[1];
				clocks	=(int)data[2];
				qnotes	=(int)data[3];
				break;

				// key signature
			case META_KEYSIG:				
				sharp	=(int)data[0];
				minor	=(int)data[1];
				break;

				// sequencer specific
			case META_SEQSPEC:
				break;
			}

			// free memory
			delete[] data;
		}
	}
}

