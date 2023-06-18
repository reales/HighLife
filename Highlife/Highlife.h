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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// discoDSP Highlife Include File
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// standard includes
#include "HighLifeDefines.h"
#include "stdlib.h"

// highlife includes
#include "./HighLifeVoice.h"
#include "./HighLifeEditor.h"
#include "./HighLifeFx.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef hiir::Downsampler2xFpu<8> Downsampler;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CHighLife : public AudioEffectX
{
public:
	CHighLife(audioMasterCallback audioMaster);
	~CHighLife(void);

public:
	virtual VstInt32	getProgram(void);
	virtual void	setProgram(VstInt32 program);
	virtual void	setProgramName(char *name);
	virtual void	getProgramName(char *name);
	virtual bool	getProgramNameIndexed(VstInt32 category,VstInt32 index,char *text);
	virtual void	setParameter(VstInt32 index,float value);
	virtual float	getParameter(VstInt32 index);
	virtual void	getParameterLabel(VstInt32 index,char *label);
	virtual void	getParameterDisplay(VstInt32 index,char *text);
	virtual void	getParameterName(VstInt32 index,char *text);
	virtual bool	canParameterBeAutomated(VstInt32 index);
	virtual VstInt32	getNumAutomatableParameters(void);
	virtual bool	getVendorString(char* text);
	virtual bool	getEffectName(char* text);
	virtual bool	getProductString(char* text);
	virtual	VstInt32	canDo(char* text);
	virtual void	resume(void);
	virtual VstInt32	processEvents(VstEvents* ev);
	virtual bool	getOutputProperties(VstInt32 index,VstPinProperties* properties);
	virtual void	DECLARE_VST_DEPRECATED(process)(float **inputs, float **outputs, VstInt32 sampleframes);
	virtual void	processReplacing(float **inputs, float **outputs, VstInt32 sampleframes);
	virtual VstInt32	getChunk(void** data, bool isPreset = false);
	virtual VstInt32	setChunk(void* data, VstInt32 byteSize, bool isPreset = false);

public:
	void crs_lock(void);
	void crs_unlock(void);

public:
	void plug_save_program(int const index,CAllocatingMemStreamOut* pmo);
	void plug_load_program(int const index,CMemStreamIn* pmi);
	void plug_audio_and_events_process(float** outputs,int const sampleframes,bool const replace);
	void plug_block_process(float **outputs,int const block_samples,bool const replace);
	int	 plug_get_free_voice(void);
	void plug_ramp_all_voices(void);
	void plug_note_on(UCHAR const key,UCHAR const vel);
	void plug_note_off(UCHAR const key,UCHAR const vel);
	void plug_all_sounds_off(void);
	void plug_trigger(bool const legato,int const state);

public:
	void gui_format_note(int const key,char* buf);
	bool gui_verify_coord(int x,int y,int zx,int zy,int cx,int cy);
	void gui_bitmap(HDC const hdc,int const x,int const y,int const w,int const h,int const xo,int const yo,HBITMAP const phbitmap);
	bool gui_bitmap_ab(HDC const hdc,int const x,int const y,int const w,int const h,int const xo,int const yo,HBITMAP const phbitmap,int const trans);
	void gui_solid_rect(HDC const hdc,int const x,int const y,int const w,int const h,COLORREF const color);
	void gui_invert(HDC const hdc,int const x,int const y,int const w,int const h);
	void gui_mouse_down(HWND const hwnd,int const xm_pos,int const ym_pos);
	void gui_mouse_move(HWND const hwnd,int const xm_pos,int const ym_pos);
	void gui_mouse_up(HWND const hwnd,int const xm_pos,int const ym_pos);
	int	 gui_get_int_mode(void);
	void gui_set_int_mode(int const im);
	void gui_tweak_int(int& i_param,int const amount,int const min,int const max);
	void gui_sed_center_around_sel(void);
	int	 gui_sed_sample_to_coord(int const sample);
	int	 gui_sed_coord_to_sample(int const x);
	void gui_sed_fix_sel(int const wave_len);
	void gui_timer(HWND const hwnd);
	void gui_refresh(void);
	void gui_command(HWND const hwnd,int const lw_wpar);

public:
	void gui_paint_wheel(HDC const hdc,int const x,int const y,int value);
	void gui_paint_knob_12(HDC const hdc,int const x,int const y,float const value);
	void gui_paint_knob_sin(HDC const hdc,int const x,int const y,float const value);
	void gui_paint_knob(HDC const hdc,int const x,int const y,RTPAR* pp);
	void gui_paint_led(HDC const hdc,int const x,int const y,int const mode);
	void gui_paint_but(HDC const hdc,int const x,int const y,int const m);
	void gui_paint_fnt(HDC const hdc,int x,int y,char* pchar,int const trunc);
	void gui_paint_dig(HDC const hdc,int const x,int const y,int const v);
	void gui_paint_dig_cent(HDC const hdc,int const x,int const y,int const v);
	void gui_paint(HDC const hdc);

public:
	void tool_sample_import_dlg(HWND const hwnd);
	void tool_sample_browse_dlg(HWND const hwnd,hlf_Zone* pz);
	void tool_init_dsp_buffer(float* pbuf,int const numsamples,int dfix);
	void tool_mix_dsp_buffer(float* pbuf_src,float* pbuf_dest,int const numsamples);
	void tool_copy_dsp_buffer(float* pbuf_src,float* pbuf_dest,int const numsamples);
public:
	bool host_instance_vst(HWND const hwnd,char* dll_path);
	void host_open_vst_dialog(HWND const hwnd);
	void host_instance_free(void);
	void host_plug_midi_send(UCHAR md0,UCHAR md1,UCHAR md2,UCHAR md3,int const deltaframe);
	void host_generate_preset(HWND hwnd);
	void host_open_plugin_editor(HWND const hwnd_parent);
	void host_close_plugin_editor(void);
	void host_process_mix(float* psamplesl,float* psamplesr,int const num_samples);

public:
	void wav_save(HWND const hwnd,hlf_Zone* pz,char* pfilename);
	void wav_export(HWND const hwnd);

public:
	void akp_import(HWND const hwnd);

public:
	void sfz_import(HWND const hwnd);
	void sfz_export(HWND const hwnd);

    void ins_import(HWND const hwnd);
	void ins_export(HWND const hwnd);

public:
	void ogg_load(HWND const hwnd,hlf_Zone* pz,char* pfilename);

public:
	void sed_sel_vol_change(float const a,float const b);
	void sed_sel_1st_order_iir(float const fc);
	void sed_sel_normalize(void);
	void sed_sel_dc_remove(void);
	void sed_sel_reverse(void);
	void sed_sel_rectify(void);
	void sed_sel_mathdrive(int const mode);
	void sed_sel_cut_marker(int& position);
	void sed_sel_cut(void);
	void sed_sel_copy(void);
	void sed_sel_paste(void);
	void sed_sel_trim_marker(int& position);
	void sed_sel_trim(void);
	void sed_crossfade_loop(float* pbuffer,int const loop_start,int const loop_length);
	void sed_crossfade_loop_equal_power(float* pbuffer,int const loop_start,int const loop_length);
	void sed_crossfade_loop_convenient(float* pbuffer,int const loop_start,int const loop_length);
	void sed_normalize(float* pbuffer,int const numsamples);
	void sed_clipboard_reset(void);
	void sed_add_cue(void);
	void sed_sel_remove_cues(void);
	
public:
	// critical section
	CRITICAL_SECTION* pcritical_section;

	// gui menus
	CDDSPGuiMenu menu_file;
	CDDSPGuiMenu menu_view;
	CDDSPGuiMenu menu_options;
	CDDSPGuiMenu menu_hl_program;
	CDDSPGuiMenu menu_vst_program;
	CDDSPGuiMenu menu_vel_splits;
	CDDSPGuiMenu menu_spl_length;
	CDDSPGuiMenu menu_zone;
	CDDSPGuiMenu menu_zone_loop;
	CDDSPGuiMenu menu_zone_trigger;
	CDDSPGuiMenu menu_sample_edit;
	CDDSPGuiMenu menu_sample_loop;
	CDDSPGuiMenu menu_sample_ampl;
	CDDSPGuiMenu menu_sample_filt;
	CDDSPGuiMenu menu_sample_spfx;
	CDDSPGuiMenu menu_sample_cuep;

	// user gui vars
	int		user_gui_page;
	int		user_last_key;
	int		user_keyb_sta[128];
	int		user_pressed;
	int		user_program;
	float	user_master_volume;
	int		user_ox;
	int		user_oy;
	int		user_delta_y_accum;

	void	gui_about(HWND const hwnd);

	// user wave clipboard
	int		user_clip_sample_size;
	int		user_clip_sample_channels;
	float** user_clip_sample;

	// gui flags
	bool	gui_recent_update;

	// sample editor
	int		user_sed_zone;
	int		user_sed_cue;
	int		user_editor_mode;
	int		user_sed_offset;
	int		user_sed_spp;
	int		user_sed_adapt;
	int		user_sed_sel_sta;
	int		user_sed_sel_len;

	// freezing user settings
	int		user_prg_splits;
	int		user_vel_splits;
	float	user_split_time;
	int		user_ste_keyboa;
	int		user_low_keyboa;
	int		user_hig_keyboa;
	int		user_route_midi;	
	int		user_force_mono;
	int		user_normalization;

	// wave processing counter info
	int		wave_processing;
	int		wave_total;
	int		wave_counter;
	char	wave_buf[1024];

	// bank
	hlf_Bank bank;

	// midi state
	HIGHLIFE_INPUT_STATE midi_state;
	
	// highlife voices
	CHighLifeVoice	voice[MAX_POLYPHONY];
	int				vt_opos[MAX_POLYPHONY];

	// fx
	CFxChorus		fx_cho;
	CFxDelay		fx_del;
	CFxReverb		fx_rev;
	CDDSPPhaser		fx_phs_l;
	CDDSPPhaser		fx_phs_r;
	CFxCompressor	fx_cmp_l;
	CFxCompressor	fx_cmp_r;

	// host events
	VstMidiEvent	midi_event[MAX_BLOCK_EVENTS];
	int				midi_num_events;

	// demo noise counter
	int		noise_count;

	// mix buffers
	float	mixer_buffer[NUM_OUTPUTS][MAX_BUFFER_SIZE];
	float	fxchr_buffer[NUM_OUTPUTS][MAX_BUFFER_SIZE];
	float	fxdel_buffer[NUM_OUTPUTS][MAX_BUFFER_SIZE];
	float	fxrev_buffer[NUM_OUTPUTS][MAX_BUFFER_SIZE];

	// gui bitamp handlers
	HBITMAP hbitmap_k12;
	HBITMAP hbitmap_but;
	HBITMAP hbitmap_cfg;
	HBITMAP hbitmap_fnt;
	HBITMAP hbitmap_gui;
	HBITMAP hbitmap_ged;
	HBITMAP hbitmap_kna;
	HBITMAP hbitmap_knb;
	HBITMAP hbitmap_led;
	HBITMAP hbitmap_whl;
	HBITMAP hbitmap_lop;
	HBITMAP	hbitmap_kbd;
	HBITMAP	hbitmap_dig;

	// gui cursor handlers
	HCURSOR	hcursor_diag;
	HCURSOR	hcursor_move;
	HCURSOR	hcursor_arro;
	HCURSOR	hcursor_szwe;
	HCURSOR	hcursor_szns;
	HCURSOR	hcursor_beam;

	// host
	AEffect*		host_plug;
	bool			host_plug_can_process;
	int				host_plug_queue_size;
	VstMidiEvent	host_plug_event[MAX_BLOCK_EVENTS];
	HWND			host_plug_hwnd;

	// input/output
	CAllocatingMemStreamOut	ms_out;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LONG WINAPI WindowProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
LONG WINAPI PluginWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float get_float_param(int const i_param,int const range);
float get_lfo_hz(float const ws);
