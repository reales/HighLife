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
// HighLife Implementation File                                                                                                          //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "windows.h"
#include "resource.h"
#include "Highlife.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CRITICAL_SECTION	gl_CriticalSection;
int					gl_CriticalSection_Count=0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHighLife::CHighLife(audioMasterCallback audioMaster):AudioEffectX(audioMaster,NUM_PROGRAMS,NUM_PARAMETERS)
{
	// set critical section pointer
	pcritical_section=&gl_CriticalSection;

	// initialize critical section and increments its reference counter
	if(gl_CriticalSection_Count==0)
	{
		gl_CriticalSection_Count++;
		InitializeCriticalSection(pcritical_section);
	}

	// call all sounds off
	plug_all_sounds_off();

	// reset voice-poly sample pos trackers
	gui_recent_update=true;

	for(int vp=0;vp<MAX_POLYPHONY;vp++)
		vt_opos[vp]=0;

	// cursors load
	hcursor_diag=LoadCursor(NULL,IDC_SIZENWSE);
	hcursor_move=LoadCursor(NULL,IDC_SIZEALL);
	hcursor_arro=LoadCursor(NULL,IDC_ARROW);
	hcursor_szwe=LoadCursor(NULL,IDC_SIZEWE);
	hcursor_szns=LoadCursor(NULL,IDC_SIZENS);
	hcursor_beam=LoadCursor(NULL,IDC_IBEAM);

	// main menus set id
	menu_file.SetID(16128);
	menu_view.SetID(16256);
	menu_options.SetID(16384);
	
	// big lcd menus set id
	menu_hl_program.SetID(16384+1024);
	menu_vst_program.SetID(16384+1280);
	
	// freezer menus set id
	menu_vel_splits.SetID(16384+512);
	menu_spl_length.SetID(16384+768);
	
	// zone menus set id
	menu_zone.SetID(16384+1536);
	menu_zone_loop.SetID(16032);
	menu_zone_trigger.SetID(16064);
	
	// sample editor menus set id
	menu_sample_edit.SetID(16384+256);
	menu_sample_loop.SetID(16384+264);
	menu_sample_ampl.SetID(16384+272);
	menu_sample_filt.SetID(16384+280);
	menu_sample_spfx.SetID(16384+288);
	menu_sample_cuep.SetID(16384+306);

	// menu file
	menu_file.AddTextOption("New Program...");
	menu_file.AddTextOption("New Bank...");
	menu_file.AddSeparator();
	menu_file.AddTextOption("Import Sample(s)...");
	menu_file.AddTextOption("Import Akai Program(s)...");
	menu_file.AddTextOption("Import Sfz Program(s)...");
	menu_file.AddSeparator();
	menu_file.AddTextOption("Export Wav File(s)...");
	menu_file.AddTextOption("Export Sfz Program...");
    menu_file.AddSeparator();
    menu_file.AddTextOption("Load HighLife Instrument...");
	menu_file.AddTextOption("Save HighLife Instrument...");


	// menu view
	menu_view.AddTextOption("Performance");
	menu_view.AddTextOption("Editor");
	
	// menu options
	menu_options.AddTextOption("Panic");
	menu_options.AddSeparator();
	menu_options.AddTextOption("Engine: Hermite (Realtime)");
	menu_options.AddTextOption("Engine: Sinc-64 (Bounce)");
	menu_options.AddTextOption("Engine: Sinc-512 (Mastering)");

	// menu vel splits options
	menu_vel_splits.AddTextOption("1");
	menu_vel_splits.AddTextOption("2");
	menu_vel_splits.AddTextOption("4");
	menu_vel_splits.AddTextOption("8");
	menu_vel_splits.AddTextOption("16");
	menu_vel_splits.AddTextOption("32");
	menu_vel_splits.AddTextOption("64");
	menu_vel_splits.AddTextOption("128");

	// menu length split options
	menu_spl_length.AddTextOption("1''");
	menu_spl_length.AddTextOption("2''");
	menu_spl_length.AddTextOption("4''");
	menu_spl_length.AddTextOption("8''");
	menu_spl_length.AddTextOption("16''");

	// menu zone loop
	menu_zone_loop.AddTextOption("Off");
	menu_zone_loop.AddTextOption("Forward");
	menu_zone_loop.AddTextOption("Bi-Directional");
	menu_zone_loop.AddTextOption("Backward");
	menu_zone_loop.AddTextOption("Forward Sustained");

	// menu zone trigger
	menu_zone_trigger.AddTextOption("Attack");
	menu_zone_trigger.AddTextOption("Release");
	
	// menu sample editor edit
	menu_sample_edit.AddTextOption("Cut");
	menu_sample_edit.AddTextOption("Copy");
	menu_sample_edit.AddTextOption("Paste");
	menu_sample_edit.AddTextOption("Trim");
	menu_sample_edit.AddSeparator();
	menu_sample_edit.AddTextOption("Select All");
	
	// menu sample editor loop
	menu_sample_loop.AddTextOption("Set Loop Start");
	menu_sample_loop.AddTextOption("Set Loop End");
	menu_sample_loop.AddTextOption("Set Loop");

	// menu sample editor ampl
	menu_sample_ampl.AddTextOption("Fade In");
	menu_sample_ampl.AddTextOption("Fade Out");
	menu_sample_ampl.AddSeparator();
	menu_sample_ampl.AddTextOption("Normalize");
	menu_sample_ampl.AddTextOption("DC Remove");
	
	// menu sample editor filt
	menu_sample_filt.AddTextOption("Smooth");
	menu_sample_filt.AddTextOption("Enhance");
	
	// menu sample editor spfx
	menu_sample_spfx.AddTextOption("Reverse");
	menu_sample_spfx.AddTextOption("Rectifier");
	menu_sample_spfx.AddSeparator();
	menu_sample_spfx.AddTextOption("Sin Drive");
	menu_sample_spfx.AddTextOption("Tanh Drive");
	menu_sample_spfx.AddSeparator();
	menu_sample_spfx.AddTextOption("Spectral Mirror");

	// menu sample editor cuep
	menu_sample_cuep.AddTextOption("Add Cue Marker");
	menu_sample_cuep.AddTextOption("Remove Cue(s)");
	menu_sample_cuep.AddSeparator();
	menu_sample_cuep.AddTextOption("Auto 20%");
	menu_sample_cuep.AddTextOption("Auto 40%");
	menu_sample_cuep.AddTextOption("Auto 60%");
	menu_sample_cuep.AddTextOption("Auto 80%");
	
	// host init
	host_plug=NULL;
	host_plug_can_process=false;
	host_plug_queue_size=0;
	host_plug_hwnd=NULL;

	// vst init
	setNumInputs(0);
	setNumOutputs(2);
	canProcessReplacing();
	isSynth();
	//setUniqueID('DDHL');
	setUniqueID('7663');
	programsAreChunks();

	// no block events
	midi_num_events=0;

	// demo limit
	noise_count=0;

	// reset user gui vars
	user_gui_page=0;
	user_last_key=0x3c;
	user_program=0;
	user_pressed=0;
	user_ox=0.0;
	user_oy=0.0;
	user_delta_y_accum=0;
	user_master_volume=1.0f;

	// reset sample editor
	user_sed_zone=0;
	user_sed_cue=0;
	user_editor_mode=1;
	user_sed_offset=0;
	user_sed_spp=32;
	user_sed_adapt=1;
	user_sed_sel_sta=0;
	user_sed_sel_len=0;
	
	// user freezing settings reset
	user_route_midi=0;
	user_prg_splits=1;
	user_vel_splits=1;
	user_split_time=2.0f;
	user_low_keyboa=24;
	user_hig_keyboa=95;
	user_ste_keyboa=12;
	user_force_mono=0;
	user_normalization=1;

	// freezing info reset
	wave_processing=0;
	wave_total=0;
	wave_counter=0;
	wave_buf[0]=0;

	// program basic initialization
	for(int p=0;p<NUM_PROGRAMS;p++)
	{
		bank.program[p].pZones=NULL;
		bank.program[p].numZones=0;
	}

	// set editor
	editor=new CHighLifeEditor(this);

	// get hinstance
	CHighLifeEditor* ped=(CHighLifeEditor*)editor;
	HINSTANCE const hinst=ped->hinstance;

	// load editor bitmaps
	hbitmap_but=(HBITMAP)LoadBitmap(hinst,MAKEINTRESOURCE(IDB_BUTTONS));
	hbitmap_fnt=(HBITMAP)LoadBitmap(hinst,MAKEINTRESOURCE(IDB_FONT));
	hbitmap_gui=(HBITMAP)LoadBitmap(hinst,MAKEINTRESOURCE(IDB_GUI));
	hbitmap_ged=(HBITMAP)LoadBitmap(hinst,MAKEINTRESOURCE(IDB_GUI_EDIT));
	hbitmap_kna=(HBITMAP)LoadBitmap(hinst,MAKEINTRESOURCE(IDB_KNOB_ALPHA));
	hbitmap_knb=(HBITMAP)LoadBitmap(hinst,MAKEINTRESOURCE(IDB_KNOB));
	hbitmap_led=(HBITMAP)LoadBitmap(hinst,MAKEINTRESOURCE(IDB_LED));
	hbitmap_whl=(HBITMAP)LoadBitmap(hinst,MAKEINTRESOURCE(IDB_WHEEL));
	hbitmap_k12=(HBITMAP)LoadBitmap(hinst,MAKEINTRESOURCE(IDB_KNOB_12));
	hbitmap_lop=(HBITMAP)LoadBitmap(hinst,MAKEINTRESOURCE(IDB_LOOP));
	hbitmap_kbd=(HBITMAP)LoadBitmap(hinst,MAKEINTRESOURCE(IDB_KBD));
	hbitmap_dig=(HBITMAP)LoadBitmap(hinst,MAKEINTRESOURCE(IDB_DIGITS));

	// reset sample wave sclipboard
	user_clip_sample_size=0;
	user_clip_sample_channels=0;
	user_clip_sample=NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHighLife::~CHighLife(void)
{
	// reset memstream out allocating object
	ms_out.Reset();

	// free gdi resources
	DeleteObject(hbitmap_k12);
	DeleteObject(hbitmap_but);
	DeleteObject(hbitmap_cfg);
	DeleteObject(hbitmap_fnt);
	DeleteObject(hbitmap_gui);
	DeleteObject(hbitmap_ged);
	DeleteObject(hbitmap_kna);
	DeleteObject(hbitmap_knb);
	DeleteObject(hbitmap_led);
	DeleteObject(hbitmap_whl);
	DeleteObject(hbitmap_lop);
	DeleteObject(hbitmap_kbd);
	DeleteObject(hbitmap_dig);

	// reset clipboard
	sed_clipboard_reset();

	// delete critical section and decrements its reference counter
	gl_CriticalSection_Count--;

	if(gl_CriticalSection_Count==0)
		DeleteCriticalSection(pcritical_section);

    if (gl_CriticalSection_Count<0) gl_CriticalSection_Count=0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VstInt32 CHighLife::getProgram(void)
{
	return user_program;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::setProgram(VstInt32 program)
{
	if(user_program!=program)
	{
		crs_lock();
		plug_all_sounds_off();
		user_program=program;
		user_sed_adapt=1;
		crs_unlock();
		gui_refresh();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::setProgramName(char *name)
{
	sprintf(bank.program[user_program].name,name);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::getProgramName(char *name)
{
	sprintf(name,bank.program[user_program].name);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CHighLife::getProgramNameIndexed(VstInt32 category,VstInt32 index,char *text)
{
	sprintf(text,bank.program[index].name);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CHighLife::getVendorString(char* text)
{
	sprintf(text,"discoDSP");
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CHighLife::getEffectName(char* text)
{
	getParameterName(0,text);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CHighLife::getProductString(char* text)
{
	getParameterName(0,text);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VstInt32 CHighLife::canDo(char* text)
{
	if(!strcmp(text,"receiveVstEvents"))return 1;
	if(!strcmp(text,"receiveVstMidiEvent"))return 1;
	if(!strcmp(text,"receiveVstTimeInfo"))return 1;
	return -1;	// explicitly can't do; 0 => don't know
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::resume(void)
{
	DECLARE_VST_DEPRECATED(wantEvents)();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VstInt32 CHighLife::processEvents(VstEvents* ev)
{
	if(host_plug && user_route_midi)
	{
		host_plug->dispatcher(host_plug,effProcessEvents,0,0,ev,0.0f);
	}
	else
	{

		// set number of events
		midi_num_events=ev->numEvents;

		// clamp number of events
		if(midi_num_events>MAX_BLOCK_EVENTS)
			midi_num_events=MAX_BLOCK_EVENTS;

		// write events
		for(int i=0;i<midi_num_events;i++)
		{
			VstMidiEvent* pmevent=(VstMidiEvent*)ev->events[i];
			midi_event[i]=*pmevent;
		}

		// bubble sort
		if(midi_num_events>1)
		{
			BOOL allSorted;

			do
			{
				allSorted=TRUE;

				for(int i=0;i<(midi_num_events-1);i++)
				{
					if(midi_event[i].deltaFrames>midi_event[i+1].deltaFrames)
					{
						// store first event pointer
						VstMidiEvent tmp_event=midi_event[i];

						// swap
						midi_event[i]=midi_event[i+1];
						midi_event[i+1]=tmp_event;

						// not sorted
						allSorted=FALSE;
					}
				}
			}
			while(allSorted==FALSE);
		}
	}

	// want more
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CHighLife::getOutputProperties(VstInt32 index,VstPinProperties* properties)
{
	if(index==0 || index==1)
	{
		sprintf(properties->shortLabel,"HL");
		sprintf(properties->label, "HighLife");
		properties->flags=kVstPinIsActive | kVstPinIsStereo;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::DECLARE_VST_DEPRECATED(process)(float **inputs, float **outputs, VstInt32 sampleframes)
{
	plug_audio_and_events_process(outputs,sampleframes,false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::processReplacing(float **inputs, float **outputs, VstInt32 sampleframes)
{
	plug_audio_and_events_process(outputs,sampleframes,true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VstInt32 CHighLife::getChunk(void** data, bool isPreset)
{
	// clear mem stream allocator
	ms_out.Reset();

	// save preset or full bank
	if(isPreset)
	{
		// save only selected preset
		plug_save_program(user_program,&ms_out);
	}
	else
	{
		// save full bank
		for(int p=0;p<NUM_PROGRAMS;p++)
			plug_save_program(p,&ms_out);
	}

	// set data pointer
	*data=(void*)ms_out.GetData();

	// return chunk size
	return ms_out.GetSize();
}
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VstInt32 CHighLife::setChunk(void* data, VstInt32 byteSize, bool isPreset)
{
	// mem stream in
	CMemStreamIn ms_in(data,byteSize);

	// read preset or full bank
	if(isPreset)
	{
		// read only selected preset
		plug_load_program(user_program,&ms_in);
	}
	else
	{
		// read full bank
		for(int p=0;p<NUM_PROGRAMS;p++)
			plug_load_program(p,&ms_in);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LONG WINAPI PluginWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// get effect pointer
	CHighLife* peffect=(CHighLife*)GetWindowLongPtr(hwnd,GWL_USERDATA);
	
	if(peffect)
	{
		switch(message)
		{
			// timer
		case WM_TIMER:

			// host plugin editor idle
			if(peffect->host_plug)
			{
				if(peffect->host_plug->flags & effFlagsHasEditor)
					peffect->host_plug->dispatcher(peffect->host_plug,effEditIdle,0,0,NULL,0.0f);
			}

			return 0;

			// close plugin window
		case WM_CLOSE:

			if(peffect->host_plug)
			{
				if(peffect->host_plug->flags & effFlagsHasEditor)
					peffect->host_plug->dispatcher(peffect->host_plug,effEditClose,0,0,NULL,0.f);
			}

			return DefWindowProc(hwnd,message,wParam,lParam);
		}
	}

	return DefWindowProc(hwnd,message,wParam,lParam);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) AEffect *MAIN(audioMasterCallback audioMaster)
{
	// get vst version
	if(!audioMaster(0,audioMasterVersion,0,0,0,0))
		return 0;// old version
	
	// create effect
	AudioEffect* effect=new CHighLife(audioMaster);

	// no effect created, return 0
	if(!effect)
		return 0;
	
	// return effect pointer
	return effect->getAeffect();
}

extern "C" __declspec(dllexport) AEffect* VSTPluginMain(audioMasterCallback audioMaster)
{
	return MAIN(audioMaster);
}
#pragma comment (linker, "/EXPORT:main=_VSTPluginMain")
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LONG WINAPI WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// get editor pointer
	CHighLifeEditor* peditor=(CHighLifeEditor*)GetWindowLongPtr(hwnd,GWL_USERDATA);

	if(peditor)
	{
		// get effect and program
		CHighLife* peffect=(CHighLife*)peditor->peffect;
		
		// struct for gdi painting
		PAINTSTRUCT ps;
		
		// get mouse coordinates
		POINT cp;
		GetCursorPos(&cp);
		ScreenToClient(hwnd,&cp);
		int const xm_pos=cp.x;
		int const ym_pos=cp.y;
		int const lw_wpar=LOWORD(wParam);

		switch (message)
		{
		case WM_COMMAND:
			peffect->gui_command(hwnd,lw_wpar);
			break;

		case WM_TIMER:
			peffect->gui_timer(hwnd);
			return 0;

		case WM_MOUSEMOVE:
			peffect->gui_mouse_move(hwnd,xm_pos,ym_pos);
			return 0;
		
		case WM_LBUTTONUP:
			peffect->gui_mouse_up(hwnd,xm_pos,ym_pos);
			return 0;
		
		case WM_LBUTTONDOWN:
			peffect->gui_mouse_down(hwnd,xm_pos,ym_pos);
			return 0;

		case WM_ERASEBKGND:
			return 0;
			
		case WM_PAINT:
			HDC hdc=BeginPaint(hwnd,&ps);
			HDC const dc_mem=CreateCompatibleDC(hdc);
			
			HBITMAP poldbitmap;
			
			if(peffect->user_gui_page==0)
				poldbitmap=(HBITMAP)SelectObject(dc_mem,peffect->hbitmap_gui);
			else
				poldbitmap=(HBITMAP)SelectObject(dc_mem,peffect->hbitmap_ged);

			peffect->gui_paint(dc_mem);

			BitBlt(hdc,0,0,K_EDITOR_WIDTH,K_EDITOR_HEIGHT,dc_mem,0,0,SRCCOPY);
			
			SelectObject(dc_mem,poldbitmap);
			DeleteDC(dc_mem);
			EndPaint(hwnd,&ps);
			peffect->gui_recent_update=true;

			return 0;
		}
	}
	
	return DefWindowProc(hwnd,message,wParam,lParam);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float get_float_param(int const i_param,int const range)
{
	float const step_size=1.0f/(float)range;
	return (float(i_param)*step_size)+(step_size*0.5f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float get_lfo_hz(float const ws)
{
	float const inv=2.0f/get_lfo_rate(ws);
	return 44100.0f/inv;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float get_lfo_rate(float const ws)
{
	return powf(2.0f,-(1.0f-ws)*14.0f)*0.01f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int get_int_param(float const f_param,int const range)
{
	float const f_res=f_param*float(range);

	int result=int(f_res);
	
	if(result>=range)
		result=range-1;
	
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float get_morph(RTPAR* p,float const x)
{
	float c=p->value+p->sense*x;
	if(c<=0.0f)c=0.0f;
	if(c>=1.0f)c=1.0f;
	return c;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float get_env_rate(float const ws)
{
	// instant check
	if(ws<=0.0f)return 1.0f;
	
	// calculate
	return powf(2.0f,-ws*14.0f)*0.01f;
}
