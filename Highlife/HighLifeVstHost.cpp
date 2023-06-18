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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HighLife VST Host Implementation                                                                                                    //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include ".\highlife.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef AEffect* (*PVSTMAIN)(audioMasterCallback audioMaster);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int useHostCount=0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CHighLife::host_instance_vst(HWND const hwnd,char* dll_path)
{
	HMODULE	h_dll=LoadLibrary(dll_path);

	if(h_dll==NULL)
	{
		// no dll
		host_plug=NULL;
		MessageBox(hwnd,"Host Error #1: LoadLibrary() Failed","discoDSP HighLife",MB_OK | MB_ICONERROR);
		return false;
	}

	// get proc main address
	PVSTMAIN _pMain = NULL;
	_pMain = (PVSTMAIN)GetProcAddress(h_dll,"main");

	if(!_pMain)
	{
		// no plug's main entry function
		host_plug=NULL;
		MessageBox(hwnd,"Host Error #2: GetProcAddress() Failed: Can't access plugin's entry function.","discoDSP HighLife",MB_OK | MB_ICONERROR);
		return false;
	}

	//obtain AEffect structure, create effect
	host_plug=_pMain(audioMaster);

	// no effect created by plug's main
	if(host_plug==NULL)
	{
		MessageBox(hwnd,"Host Error #3: Plugin's entry function failed.","discoDSP HighLife",MB_OK | MB_ICONERROR);
		return false;
	}

	// init plugin 
	host_plug->dispatcher(host_plug, effOpen, 0, 0, NULL, 0.0f);
	host_plug->dispatcher(host_plug, effSetSampleRate, 0, 0, 0, getSampleRate());
	host_plug->dispatcher(host_plug, effSetBlockSize, 0, getBlockSize(), NULL, 0.0f);
	host_plug->dispatcher(host_plug, effMainsChanged, 0, 1, NULL, 0.0f);

	// allow plug process
	host_plug_can_process=true;

	// allow midi trhu
	user_route_midi=1;

	// success
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::host_open_vst_dialog(HWND const hwnd)
{
	CDDSPGuiFileDialog dlg;
	
	if(dlg.OpenFileDialog(hwnd,"discoDSP HighLife - Load Plugin","*.dll","VST DLL Plugin Files\0*.dll\0\0","dll",NULL,false))
	{
		host_instance_free();
		host_instance_vst(hwnd,dlg.GetFileName(0));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::host_instance_free(void)
{
	// get editor windoze (this plugin)
	CHighLifeEditor* peditor=(CHighLifeEditor*)editor;

	// reset instance
	if(host_plug)
	{
		// safe
		host_plug_can_process=false;

		// close window
		host_close_plugin_editor();

		// free plugin instance
		host_plug->dispatcher(host_plug,effMainsChanged,0,0,0,0.f);
		host_plug->dispatcher(host_plug,effClose,0,0,0,0.f);
		host_plug_queue_size=0;
		host_plug=NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::host_plug_midi_send(UCHAR md0,UCHAR md1,UCHAR md2,UCHAR md3,int const deltaframe)
{
	if(host_plug!=NULL)
	{
		if(host_plug_queue_size<MAX_BLOCK_EVENTS)
		{
			// get event
			VstMidiEvent* pevent=&host_plug_event[host_plug_queue_size];

			// prepare event
			pevent->byteSize=24;
			pevent->deltaFrames=deltaframe;
			pevent->detune=0;
			pevent->flags=0;
			pevent->midiData[0]=md0;
			pevent->midiData[1]=md1;
			pevent->midiData[2]=md2;
			pevent->midiData[3]=md3;
			pevent->noteLength=0;
			pevent->noteOffset=0;
			pevent->noteOffVelocity=0;
			pevent->reserved1=0;
			pevent->reserved2=0;
			pevent->type=kVstMidiType;

			// increment queue
			host_plug_queue_size++;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::host_generate_preset(HWND hwnd)
{
	// off process
	host_plug_can_process=false;

	// off midi routing
	user_route_midi=0;

	// plugin
	if(host_plug)
	{
		// reset wave counter
		wave_counter=0;

		// get temp blocksize
		int const block_size=256;

		// set temporal block size and sample rate
		host_plug->dispatcher(host_plug,effSetSampleRate,0,0,0,FREEZE_SR);
		host_plug->dispatcher(host_plug,effSetBlockSize,0,block_size,NULL,0.f);

		// get key and velo division
		int const kbd_range=(user_hig_keyboa+1)-user_low_keyboa;
		int const kbd_divisions=kbd_range/user_ste_keyboa;
/*		int const top_div_size = kbd_range%user_ste_keyboa;
		if(top_div_size!=0){
			++kbd_divisions;
		}
*/


		// check range
		// TODO: remove this crappy restriction
		if((kbd_range%user_ste_keyboa)!=0 || kbd_range<user_ste_keyboa)
		{
			MessageBox(hwnd,"Invalid Key Range/Step Settings\nPlease make sure that the number of semitones in the selected range is a multiple of the selected step size","discoDSP HighLife",MB_OK | MB_ICONERROR);
			return;
		}

		// get vel range length
		int const vel_range=128/user_vel_splits;

		// compute
		int const temp_size=int(double(user_split_time)*(double)getSampleRate());

		// get wavesize and num channels
		int const num_samples=(temp_size/block_size)*block_size;
		int const num_channels=host_plug->numOutputs;

		// get number of waves
		wave_total=user_prg_splits*kbd_divisions*user_vel_splits;

		// get size in bytes / megas
		int const num_bytes=temp_size*sizeof(float)*wave_total*num_channels;
		double const num_megas=double(num_bytes)/1048576.0;

		char buf[1024];
		sprintf(buf,"Freezer Operation Will Take %.1f Megabytes In Total (About %.1f Megabytes Per Frozen Program).",num_megas,num_megas/double(user_prg_splits));

		if(MessageBox(hwnd,buf,"discoDSP HighLife",MB_OKCANCEL | MB_ICONINFORMATION)!=IDOK)
		{
			// restore main state
			host_plug->dispatcher(host_plug,effSetSampleRate,0,0,0,getSampleRate());
			host_plug->dispatcher(host_plug,effSetBlockSize,0,getBlockSize(),NULL,0.f);
			host_plug_can_process=true;
			return;
		}

		int cs;

		// retrieve current program
		int const vsti_program=host_plug->dispatcher(host_plug,effGetProgram,0,0,NULL,0.0f);

		// create pins array
		float** pins=new float*[host_plug->numInputs];

		// create pins wavedata
		for(int i=0;i<host_plug->numInputs;i++)
			pins[i]=new float[block_size];
	
		// create pouts array
		float** pouts=new float*[host_plug->numOutputs];

		// create pouts wavedata
		for(int o=0;o<host_plug->numOutputs;o++)
			pouts[o]=new float[block_size];
	
		// get current program
		int const i_hl_curr_prg=user_program;

		// program loop
		for(int i=0;i<user_prg_splits;i++)
		{
			int const vsti_prg_i=vsti_program+i;
			int const high_prg_i=i_hl_curr_prg+i;

			if(vsti_prg_i<host_plug->numPrograms && high_prg_i<NUM_PROGRAMS)
			{
				// set program(s)
				user_program=high_prg_i;
				host_plug->dispatcher(host_plug,effSetProgram,0,vsti_prg_i,NULL,0.0f);

				// get program name
				char prg_name[256];
				host_plug->dispatcher(host_plug,effGetProgramName,0,0,prg_name,0.0f);

				// get program pointer and initialize it
				hlf_Program* pprog=&bank.program[high_prg_i];
				pprog->init();

				// set prg name
				host_plug->dispatcher(host_plug,effGetProgramName,0,0,pprog->name,0.0f);

				// create new zones
				pprog->numZones=kbd_divisions*user_vel_splits;
				pprog->pZones=new hlf_Zone[pprog->numZones];

				// initialize wavetables
				for(int z=0;z<pprog->numZones;z++)
				{
					hlf_Zone* pz=&pprog->pZones[z];

					// init zone
					pz->init( &bank.defaultZone );

					// allocate wave in zone
					pz->allocWave(num_channels,num_samples);

					// set sample rate
					pz->pWave->sample_rate=FREEZE_SR;

					// init musical properties
					pz->pWave->setNumTicks();
					
					// init loop
					pz->pWave->loop_start=(num_samples*2)/3;
					pz->pWave->loop_end=num_samples-4;
					pz->pWave->loop_mode=1;
				}

				// program zone index
				int pzi=0;

				// process
				for(int ck=user_low_keyboa;ck<=user_hig_keyboa;ck+=user_ste_keyboa)
				{
					for(int cv=0;cv<128;cv+=vel_range)
					{
						// wavebuf
						sprintf(wave_buf,"Freezing... %.1f%%",(float)wave_counter*100.0f/(float)wave_total);
						wave_processing=1;
						InvalidateRect(hwnd,NULL,FALSE);
						UpdateWindow(hwnd);

						// get wave pointer
						user_sed_zone=pzi;
						hlf_Zone* pz=&pprog->pZones[pzi++];
					
						// init input range
						pz->lo_input_range.midi_key=ck;
						pz->hi_input_range.midi_key=ck+(user_ste_keyboa-1);
						pz->lo_input_range.midi_vel=cv;
						pz->hi_input_range.midi_vel=cv+(vel_range-1);

						// init midi properties
						pz->midi_root_key=ck+user_ste_keyboa/2;

						// init label and path
						char note_buf[4];
						gui_format_note(pz->midi_root_key,note_buf);
						sprintf(pz->name,"%s %s",pprog->name,note_buf);
						sprintf(pz->pWave->path,"Not Available");

						// render vsti samples
						for(int s=0;s<pz->pWave->num_samples;s+=block_size)
						{
							// clear pins wavedata
							for(i=0;i<host_plug->numInputs;i++)
								tool_init_dsp_buffer(pins[i],block_size,0);

							// clear pouts wavedata
							for(int o=0;o<host_plug->numOutputs;o++)
								tool_init_dsp_buffer(pouts[o],block_size,0);

							// send note on at first sample
							if(s==0)
								host_plug_midi_send(0x90,pz->midi_root_key,pz->hi_input_range.midi_vel,0,0);

							// prepare event group
							VstEvents event_group;
							event_group.numEvents=host_plug_queue_size;
							event_group.reserved=0;

							// place event(s)
							for(int q=0;q<host_plug_queue_size;q++)
								event_group.events[q]=(VstEvent*)&host_plug_event[q];

							// vst processing, send the events
							host_plug->dispatcher(host_plug,effProcessEvents,0,0,&event_group,0.0f);

							// plug process
							if(host_plug->flags & effFlagsCanReplacing) {
								host_plug->processReplacing(host_plug,pins,pouts,block_size);
							} else {
								host_plug->DECLARE_VST_DEPRECATED(process)(host_plug, pins, pouts, block_size);
							}

							// set no midi events
							host_plug_queue_size=0;

							// copy samples to wavetable
							for(int o=0;o<host_plug->numOutputs;o++)
								tool_copy_dsp_buffer(pouts[o],pz->pWave->data[o]->pBuf+WAVE_PAD+s,block_size);
						}

						// wave postprocessing, loop crossfade and normalization
						for(int o=0;o<pz->pWave->num_channels;o++)
						{
//							sed_crossfade_loop(pz->pWave->data[o]->pBuf+WAVE_PAD,pz->pWave->loop_start,pz->pWave->loop_end-pz->pWave->loop_start);
//							TODO: make loop crossfade type selectable/tunable (fade between equal power and equal gain)
							sed_crossfade_loop_convenient(pz->pWave->data[o]->pBuf+WAVE_PAD,pz->pWave->loop_start,pz->pWave->loop_end-pz->pWave->loop_start);

							if(user_normalization)
								sed_normalize(pz->pWave->data[o]->pBuf+WAVE_PAD,pz->pWave->num_samples);
						}

						wave_counter++;

						// send note off
						host_plug_midi_send(0x80,pz->midi_root_key,64,0,0);

						// render until silence
						bool very_loud;

						int silent_counter=0;

						// stuck note limit (60 seconds)
						int const max_loud_time=44100*60;

						// seeking silence render/loop
						do
						{
							// threshold flag
							very_loud=false;

							// clear pins wavedata
							for(i=0;i<host_plug->numInputs;i++)
								tool_init_dsp_buffer(pins[i],block_size,0);

							// clear pouts wavedata
							for(int o=0;o<host_plug->numOutputs;o++)
								tool_init_dsp_buffer(pouts[o],block_size,0);

							// silent add
							silent_counter+=block_size;

							// max time send all note offs and stuff
							if(silent_counter>max_loud_time)
							{
								host_plug_midi_send(0xB0,120,1,0,0);
								host_plug_midi_send(0xB0,123,1,0,0);
							}

							// prepare event group
							VstEvents event_group;
							event_group.numEvents = host_plug_queue_size;
							event_group.reserved  = 0;

							// place event(s)
							for(int q=0;q<host_plug_queue_size;q++)
								event_group.events[q]=(VstEvent*)&host_plug_event[q];

							// vst processing, send the events
							host_plug->dispatcher(host_plug,effProcessEvents,0,0,&event_group,0.0f);

							// plug process
							if(host_plug->flags & effFlagsCanReplacing) {
								host_plug->processReplacing(host_plug,pins,pouts,block_size);
							} else {
								host_plug->DECLARE_VST_DEPRECATED(process)(host_plug,pins,pouts,block_size);
							}

							// check level
							for(int o=0;o<host_plug->numOutputs;o++)
							{
								for(cs=0;cs<block_size;cs++)
								{
									float const abs_level=fabsf(pouts[o][cs]);

									if(abs_level>0.0001f)
										very_loud=true;
								}
							}

							// set no midi events
							host_plug_queue_size=0;

							// max time
							if(silent_counter>max_loud_time)
								very_loud=false;
						}
						while(very_loud);
					}
				}
			}
		}

		// set original program(s)
		host_plug->dispatcher(host_plug,effSetProgram,0,vsti_program,NULL,0.0f);
		user_program=i_hl_curr_prg;

		// delete pins wavedata
		for(int i=0;i<host_plug->numInputs;i++)
			delete[] pins[i];

		// delete pins array
		delete[] pins;

		// delete pouts wavedata
		for(int o=0;o<host_plug->numOutputs;o++)
			delete[] pouts[o];

		// delete pouts array
		delete[] pouts;

		// restore main state
		host_plug->dispatcher(host_plug,effSetSampleRate,0,0,0,getSampleRate());
		host_plug->dispatcher(host_plug,effSetBlockSize,0,getBlockSize(),NULL,0.f);	

		// ask for free plug
		if(MessageBox(hwnd,"Unload VSTi?" ,"discoDSP Highlife",MB_YESNO | MB_ICONINFORMATION)==IDYES)
			host_instance_free();
	}

	// adapt
	user_sed_adapt=1;
	user_sed_zone=0;

	// wavebuf
	wave_processing=2;
	InvalidateRect(hwnd,NULL,FALSE);

	// off process
	host_plug_can_process=true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::host_open_plugin_editor(HWND const hwnd_parent)
{
	CHighLifeEditor* ped=(CHighLifeEditor*)editor;
	HINSTANCE const hinst=ped->hinstance;

	// close if previously open
	host_close_plugin_editor();

	// open vst gui
	if(hwnd_parent && host_plug)
	{
		// create plugin window if any
		if(host_plug->flags & effFlagsHasEditor)
		{
			// inc host window reference count
			useHostCount++;

			// register host window class if first time
			if(useHostCount==1)
			{
				WNDCLASS windowClass;
				windowClass.style=0;
				windowClass.lpfnWndProc=PluginWindowProc;
				windowClass.cbClsExtra=0;
				windowClass.cbWndExtra=0;
				windowClass.hInstance=hinst;
				windowClass.hIcon=0;
				windowClass.hCursor=0;
				windowClass.hbrBackground=0;
				windowClass.lpszMenuName=0;
				windowClass.lpszClassName="VSTDDHighLifeHost_OSS";
				RegisterClass(&windowClass);
			}

			// create plugin window
			host_plug_hwnd=CreateWindowEx(WS_EX_TOOLWINDOW,"VSTDDHighLifeHost_OSS","discoDSP Highlife",WS_VISIBLE | WS_SYSMENU | WS_CAPTION,64,64,64,16,hwnd_parent,NULL,hinst,NULL);
			SetWindowLong(host_plug_hwnd,GWL_USERDATA,(intptr_t)this);

			// get plugin editor window size
			ERect * er;
			host_plug->dispatcher(host_plug,effEditOpen,0,0,host_plug_hwnd,0.0f);
			host_plug->dispatcher(host_plug,effEditGetRect,0,0,&er,0.0f);
			SetWindowPos(host_plug_hwnd,hwnd_parent,0,0,er->right+6,er->bottom+22,SWP_NOMOVE | SWP_NOZORDER);
			host_plug->dispatcher(host_plug,DECLARE_VST_DEPRECATED(effEditTop),0,0,NULL,0.0f);
			ShowWindow(host_plug_hwnd,SW_SHOWNORMAL);

			// set plugin window caption
			char buf[256];
			char pname[128];
			host_plug->dispatcher(host_plug,effGetEffectName,0,0,pname,0.f);
			sprintf(buf,"discoDSP Highlife - %s",pname);
			SetWindowText(host_plug_hwnd,buf);

			// set plugin window timer
			SetTimer(host_plug_hwnd,1,16,NULL);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::host_close_plugin_editor(void)
{
	// close window if opened
	if(host_plug_hwnd && ::IsWindow(host_plug_hwnd))
	{
		CHighLifeEditor* ped=(CHighLifeEditor*)editor;
		HINSTANCE const hinst=ped->hinstance;

		if(host_plug->flags & effFlagsHasEditor)
			host_plug->dispatcher(host_plug,effEditClose,0,0,NULL,0.f);

		KillTimer(host_plug_hwnd,1);
		DestroyWindow(host_plug_hwnd);

		useHostCount--;

		if(useHostCount==0)
			UnregisterClass("VSTDDHighLifeHost_OSS",hinst);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::host_process_mix(float* psamplesl,float* psamplesr,int const num_samples)
{
	if(host_plug && host_plug_can_process)
	{
		float**			pins = NULL;

		if(host_plug->numInputs) 
		{
			// create pins array
			float** pins=new float*[host_plug->numInputs];

			// create pins wavedata
			for(int i=0;i<host_plug->numInputs;i++)
			{
				pins[i]=new float[num_samples];
				tool_init_dsp_buffer(pins[i],num_samples,0);
			}
		}

		// create pouts array
		float** pouts=new float*[host_plug->numOutputs];

		// create pouts wavedata
		for(int o=0;o<host_plug->numOutputs;o++)
		{
			pouts[o]=new float[num_samples];
		}

		// host plugin process
		if(host_plug->flags & effFlagsCanReplacing)
		{
			host_plug->processReplacing(host_plug, pins, pouts, num_samples);
		}
		else
		{
			// zero outputs first
			for(int o = 0; o < host_plug->numOutputs; o++) {
				tool_init_dsp_buffer(pouts[o],num_samples,0);
			}

			host_plug->DECLARE_VST_DEPRECATED(process)(host_plug,pins,pouts,num_samples);
		}

		// multioutput mix (panned l/r)
		if(host_plug->numOutputs>1)
		{
			for(int o=0;o<host_plug->numOutputs;o++)
			{
				float* pvstout=pouts[o];
				float* phostbu=mixer_buffer[o&(NUM_OUTPUTS-1)];

				for(int s=0;s<num_samples;s++)
				{
					phostbu[s]+=pvstout[s];
				}
			}
		}

		// mono mix
		if(host_plug->numOutputs==1)
		{
			// mono mix
			float* pmonoout=pouts[0];
			for(int s=0;s<num_samples;s++)
			{
				psamplesl[s]+=pmonoout[s];
				psamplesr[s]+=pmonoout[s];
			}
		}

		if(host_plug->numInputs) 
		{
			// delete pins wavedata
			if (pins)
			{
				for(int i=0;i<host_plug->numInputs;i++)
					if (pins[i]) delete[] pins[i];
	
				// delete pins array
				delete[] pins;
		    }
		}

		// delete pouts wavedata
		for(int o=0;o<host_plug->numOutputs;o++)
			delete[] pouts[o];

		// delete pouts array
		delete[] pouts;
	}
}
