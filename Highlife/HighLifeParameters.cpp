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
// HighLife Parameters Implementation                                                                                                  //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include ".\highlife.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void fmt_phs(char* txt,float const f_phase)
{
	if(f_phase<0.95f)
		sprintf(txt,"%d Degrees",int(f_phase*360.0f));
	else
		sprintf(txt,"Free Run");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void format_db(float const f_lin,char* pbuf)
{
	if(f_lin>=1.0f)
		sprintf(pbuf,"-0.0 dB");
	else if(f_lin>0.0f)
		sprintf(pbuf,"%.1f dB",20.0f*log10f(f_lin));
	else
		sprintf(pbuf,"-oo dB");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void const format_env_ms(char* txt,float const ws)
{
	if(ws<=0.0f)
	{
		sprintf(txt,"Instant");
	}
	else
	{
		float const delta=get_env_rate(ws);
		float const samples=1.0/delta;
		float const ms=samples/44.1f;
		sprintf(txt,"%.2f ms",ms);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::setParameter(VstInt32 index, float value)
{
	hlf_Program* pprog=&bank.program[user_program];

	// set enviroment parameters
	if(index==1)user_master_volume=value;

	// set program parameters
	if(index==16)pprog->efx_chr_del=value;
	if(index==17)pprog->efx_chr_fdb=value;
	if(index==18)pprog->efx_chr_rat=value;
	if(index==19)pprog->efx_chr_mod=value;
	if(index==20)pprog->efx_del_del=value;
	if(index==21)pprog->efx_del_fdb=value;
	if(index==22)pprog->efx_rev_roo=value;
	if(index==23)pprog->efx_rev_wid=value;
	if(index==24)pprog->efx_rev_dam=value;
	if(index==25)pprog->efx_dft_frq=value;
			
	// select zone for automation
	if(index==128)
	{
		user_sed_zone=int(float(pprog->numZones)*value);
		user_sed_adapt=1;
	}

	// sense param (polarized value)
	float const sense=value*2.0f-1.0f;

	// set sample zone parameters
	if(user_sed_zone<pprog->numZones)
	{
		// get zone pointer
		hlf_Zone* pz=&pprog->pZones[user_sed_zone];

		if(index==129)pz->pWave->loop_start=int(float(pz->pWave->num_samples)*value);
		if(index==130)pz->pWave->loop_end=int(float(pz->pWave->num_samples)*value);
		if(index==131)pz->cue_pos[0]=int(float(pz->pWave->num_samples)*value);
		if(index==132)pz->glide_auto=value;
		if(index==133)pz->vel_amp=value;
		if(index==134)pz->vel_mod=value;
		//if(index==135)pw->playmode=value;
		if(index==136)pz->flt_type=value;
		if(index==137)pz->sys_pit_rng=value;
		if(index==138)pz->flt_kbd_trk=value;
		if(index==139)pz->flt_vel_trk=value;
		if(index==140)pz->flt_cut_frq.value=value;
		if(index==141)pz->flt_cut_frq.sense=sense;
		if(index==142)pz->flt_res_amt.value=value;
		if(index==143)pz->flt_res_amt.sense=sense;
		if(index==144)pz->glide.value=value;
		if(index==145)pz->glide.sense=sense;
		if(index==146)pz->amp_env_att.value=value;
		if(index==147)pz->amp_env_att.sense=sense;
		if(index==148)pz->amp_env_dec.value=value;
		if(index==149)pz->amp_env_dec.sense=sense;
		if(index==150)pz->amp_env_sus.value=value;
		if(index==151)pz->amp_env_sus.sense=sense;
		if(index==152)pz->amp_env_rel.value=value;
		if(index==153)pz->amp_env_rel.sense=sense;
		if(index==154)pz->amp_env_amt.value=value;
		if(index==155)pz->amp_env_amt.sense=sense;
		if(index==156)pz->mod_env_att.value=value;
		if(index==157)pz->mod_env_att.sense=sense;
		if(index==158)pz->mod_env_dec.value=value;
		if(index==159)pz->mod_env_dec.sense=sense;
		if(index==160)pz->mod_env_sus.value=value;
		if(index==161)pz->mod_env_sus.sense=sense;
		if(index==162)pz->mod_env_rel.value=value;
		if(index==163)pz->mod_env_rel.sense=sense;
		if(index==164)pz->mod_env_cut.value=value;
		if(index==165)pz->mod_env_cut.sense=sense;
		if(index==166)pz->mod_env_pit.value=value;
		if(index==167)pz->mod_env_pit.sense=sense;
		if(index==168)pz->mod_lfo_phs.value=value;
		if(index==169)pz->mod_lfo_phs.sense=sense;
		if(index==170)pz->mod_lfo_rat.value=value;
		if(index==171)pz->mod_lfo_rat.sense=sense;
		if(index==172)pz->mod_lfo_amp.value=value;
		if(index==173)pz->mod_lfo_amp.sense=sense;
		if(index==174)pz->mod_lfo_cut.value=value;
		if(index==175)pz->mod_lfo_cut.sense=sense;
		if(index==176)pz->mod_lfo_res.value=value;
		if(index==177)pz->mod_lfo_res.sense=sense;
		if(index==178)pz->mod_lfo_pit.value=value;
		if(index==179)pz->mod_lfo_pit.sense=sense;
		if(index==180)pz->efx_chr_lev.value=value;
		if(index==181)pz->efx_chr_lev.sense=sense;
		if(index==182)pz->efx_del_lev.value=value;
		if(index==183)pz->efx_del_lev.sense=sense;
		if(index==184)pz->efx_rev_lev.value=value;
		if(index==185)pz->efx_rev_lev.sense=sense;
	}

	// update parameter ramper
	plug_ramp_all_voices();

	// gui refresh
	gui_refresh();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float CHighLife::getParameter(VstInt32 index)
{
	hlf_Program* pprog=&bank.program[user_program];

	// return enviroment parameters
	if(index==1)return user_master_volume;

	// return program parameters
	if(index==16)return pprog->efx_chr_del;
	if(index==17)return pprog->efx_chr_fdb;
	if(index==18)return pprog->efx_chr_rat;
	if(index==19)return pprog->efx_chr_mod;
	if(index==20)return pprog->efx_del_del;
	if(index==21)return pprog->efx_del_fdb;
	if(index==22)return pprog->efx_rev_roo;
	if(index==23)return pprog->efx_rev_wid;
	if(index==24)return pprog->efx_rev_dam;
	if(index==25)return pprog->efx_dft_frq;
		
	// return zone selected for automation
	if(index==128)return float(user_sed_zone)/float(pprog->numZones);

	// sample zone layer automation to vst params
	if(user_sed_zone<pprog->numZones)
	{
		// get zone pointer
		hlf_Zone* pz=&pprog->pZones[user_sed_zone];

		if(index==129)return float(pz->pWave->loop_start)/float(pz->pWave->num_samples);
		if(index==130)return float(pz->pWave->loop_end)/float(pz->pWave->num_samples);
		if(index==131)return float(pz->cue_pos[0])/float(pz->pWave->num_samples);
		if(index==132)return pz->glide_auto;
		if(index==133)return pz->vel_amp;
		if(index==134)return pz->vel_mod;
		//if(index==135)return pw->playmode;
		if(index==136)return pz->flt_type;
		if(index==137)return pz->sys_pit_rng;
		if(index==138)return pz->flt_kbd_trk;
		if(index==139)return pz->flt_vel_trk;
		if(index==140)return pz->flt_cut_frq.value;
		if(index==141)return pz->flt_cut_frq.sense*0.5f+0.5f;
		if(index==142)return pz->flt_res_amt.value;
		if(index==143)return pz->flt_res_amt.sense*0.5f+0.5f;
		if(index==144)return pz->glide.value;
		if(index==145)return pz->glide.sense*0.5f+0.5f;
		if(index==146)return pz->amp_env_att.value;
		if(index==147)return pz->amp_env_att.sense*0.5f+0.5f;
		if(index==148)return pz->amp_env_dec.value;
		if(index==149)return pz->amp_env_dec.sense*0.5f+0.5f;
		if(index==150)return pz->amp_env_sus.value;
		if(index==151)return pz->amp_env_sus.sense*0.5f+0.5f;
		if(index==152)return pz->amp_env_rel.value;
		if(index==153)return pz->amp_env_rel.sense*0.5f+0.5f;
		if(index==154)return pz->amp_env_amt.value;
		if(index==155)return pz->amp_env_amt.sense*0.5f+0.5f;
		if(index==156)return pz->mod_env_att.value;
		if(index==157)return pz->mod_env_att.sense*0.5f+0.5f;
		if(index==158)return pz->mod_env_dec.value;
		if(index==159)return pz->mod_env_dec.sense*0.5f+0.5f;
		if(index==160)return pz->mod_env_sus.value;
		if(index==161)return pz->mod_env_sus.sense*0.5f+0.5f;
		if(index==162)return pz->mod_env_rel.value;
		if(index==163)return pz->mod_env_rel.sense*0.5f+0.5f;
		if(index==164)return pz->mod_env_cut.value;
		if(index==165)return pz->mod_env_cut.sense*0.5f+0.5f;
		if(index==166)return pz->mod_env_pit.value;
		if(index==167)return pz->mod_env_pit.sense*0.5f+0.5f;
		if(index==168)return pz->mod_lfo_phs.value;
		if(index==169)return pz->mod_lfo_phs.sense*0.5f+0.5f;
		if(index==170)return pz->mod_lfo_rat.value;
		if(index==171)return pz->mod_lfo_rat.sense*0.5f+0.5f;
		if(index==172)return pz->mod_lfo_amp.value;
		if(index==173)return pz->mod_lfo_amp.sense*0.5f+0.5f;
		if(index==174)return pz->mod_lfo_cut.value;
		if(index==175)return pz->mod_lfo_cut.sense*0.5f+0.5f;
		if(index==176)return pz->mod_lfo_res.value;
		if(index==177)return pz->mod_lfo_res.sense*0.5f+0.5f;
		if(index==178)return pz->mod_lfo_pit.value;
		if(index==179)return pz->mod_lfo_pit.sense*0.5f+0.5f;
		if(index==180)return pz->efx_chr_lev.value;
		if(index==181)return pz->efx_chr_lev.sense*0.5f+0.5f;
		if(index==182)return pz->efx_del_lev.value;
		if(index==183)return pz->efx_del_lev.sense*0.5f+0.5f;
		if(index==184)return pz->efx_rev_lev.value;
		if(index==185)return pz->efx_rev_lev.sense*0.5f+0.5f;
	}

	return 0.0f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::getParameterLabel(VstInt32 index, char *label)
{
	sprintf(label,"");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::getParameterDisplay(VstInt32 index, char *text)
{
	hlf_Program* pprog=&bank.program[user_program];

	// display enviroment parameters
	if(index==1)format_db(user_master_volume,text);
	
	// display program parameters
	if(index==16)sprintf(text,"%.1f Ms",(pprog->efx_chr_del*float(MAX_CHO_SIZE)*500.0f)/getSampleRate());
	if(index==17)sprintf(text,"%d%%",int(pprog->efx_chr_fdb*100.0f));
	if(index==18)sprintf(text,"%.2f Hz",get_lfo_hz(pprog->efx_chr_rat));
	if(index==19)sprintf(text,"%d%%",int(pprog->efx_chr_mod*100.0f));
	if(index==20){if(get_int_param(pprog->efx_del_syn,2)){sprintf(text,"%.2f Ticks",pprog->efx_del_del*4.0f);}else{sprintf(text,"%.1f Ms",pprog->efx_del_del*1000.0f);}}
	if(index==21)sprintf(text,"%d%%",int(pprog->efx_del_fdb*100.0f));
	if(index==22)sprintf(text,"%d%%",int(pprog->efx_rev_roo*100.0f));
	if(index==23)sprintf(text,"%d%%",int(pprog->efx_rev_wid*100.0f));
	if(index==24)sprintf(text,"%d%%",int(pprog->efx_rev_dam*100.0f));
	if(index==25)sprintf(text,"%.2f Hz",get_lfo_hz(pprog->efx_dft_frq));
		
	// display wave parameters
	if(user_sed_zone<pprog->numZones)
	{
		// get zone pointer
		hlf_Zone* pz=&pprog->pZones[user_sed_zone];

		if(index==128)sprintf(text,pz->name);
		if(index==129)sprintf(text,"%d",pz->pWave->loop_start);
		if(index==130)sprintf(text,"%d",pz->pWave->loop_end);
		if(index==131)sprintf(text,"%d",pz->cue_pos[0]);
		if(index==132)sprintf(text,"%d",get_int_param(pz->glide_auto,NUM_GLIDE_MODES));
		if(index==133)sprintf(text,"%d%%",int(pz->vel_amp*100.0f));
		if(index==134)sprintf(text,"%d%%",int(pz->vel_mod*100.0f));
		//if(index==135)
		if(index==136)sprintf(text,"%d",get_int_param(pz->flt_type,NUM_FILTER_MODES));
		if(index==137)sprintf(text,"%d Cents",int(pz->sys_pit_rng*2400.0f));
		if(index==138)sprintf(text,"%d%%",int(pz->flt_kbd_trk*100.0f));
		if(index==139)sprintf(text,"%d%%",int(pz->flt_vel_trk*100.0f));
		if(index==140)sprintf(text,"%d Hz",int(pz->flt_cut_frq.value*pz->flt_cut_frq.value*getSampleRate()*0.5f));
		if(index==141)sprintf(text,"%f",pz->flt_cut_frq.sense);
		if(index==142)sprintf(text,"%d%%",int(pz->flt_res_amt.value*100.0f));
		if(index==143)sprintf(text,"%f",pz->flt_res_amt.sense);
		if(index==144)sprintf(text,"%d%%",int(pz->glide.value*100.0f));
		if(index==145)sprintf(text,"%f",pz->glide.sense);	
		if(index==146)format_env_ms(text,pz->amp_env_att.value);
		if(index==147)sprintf(text,"%f",pz->amp_env_att.sense);
		if(index==148)format_env_ms(text,pz->amp_env_dec.value);
		if(index==149)sprintf(text,"%f",pz->amp_env_dec.sense);
		if(index==150)sprintf(text,"%d%%",int(pz->amp_env_sus.value*100.0f));
		if(index==151)sprintf(text,"%f",pz->amp_env_sus.sense);
		if(index==152)format_env_ms(text,pz->amp_env_rel.value);
		if(index==153)sprintf(text,"%f",pz->amp_env_rel.sense);
		if(index==154)sprintf(text,"%d%%",int(pz->amp_env_amt.value*100.0f));
		if(index==155)sprintf(text,"%f",pz->amp_env_amt.sense);	
		if(index==156)format_env_ms(text,pz->mod_env_att.value);
		if(index==157)sprintf(text,"%f",pz->mod_env_att.sense);
		if(index==158)format_env_ms(text,pz->mod_env_dec.value);
		if(index==159)sprintf(text,"%f",pz->mod_env_dec.sense);
		if(index==160)sprintf(text,"%d%%",int(pz->mod_env_sus.value*100.0f));
		if(index==161)sprintf(text,"%f",pz->mod_env_sus.sense);
		if(index==162)format_env_ms(text,pz->mod_env_rel.value);
		if(index==163)sprintf(text,"%f",pz->mod_env_rel.sense);
		if(index==164)sprintf(text,"%.1f%%",(pz->mod_env_cut.value*200.0f)-100.0f);
		if(index==165)sprintf(text,"%f",pz->mod_env_cut.sense);
		if(index==166)sprintf(text,"%.1f%%",(pz->mod_env_pit.value*200.0f)-100.0f);
		if(index==167)sprintf(text,"%f",pz->mod_env_pit.sense);
		if(index==168)fmt_phs(text,pz->mod_lfo_phs.value);
		if(index==169)sprintf(text,"%f",pz->mod_lfo_phs.sense);
		if(index==170){if(pz->mod_lfo_syn){sprintf(text,"%d Tick(s)",int(1.0f+(1.0f-pz->mod_lfo_rat.value)*255.0f));}else{sprintf(text,"%.2f Hz",get_lfo_hz(pz->mod_lfo_rat.value));}}
		if(index==171)sprintf(text,"%f",pz->mod_lfo_rat.sense);
		if(index==172)sprintf(text,"%.1f%%",pz->mod_lfo_amp.value*100.0f);
		if(index==173)sprintf(text,"%f",pz->mod_lfo_amp.sense);
		if(index==174)sprintf(text,"%.1f%%",(pz->mod_lfo_cut.value*200.0f)-100.0f);
		if(index==175)sprintf(text,"%f",pz->mod_lfo_cut.sense);
		if(index==176)sprintf(text,"%.1f%%",(pz->mod_lfo_res.value*200.0f)-100.0f);
		if(index==177)sprintf(text,"%f",pz->mod_lfo_res.sense);
		if(index==178)sprintf(text,"%.1f%%",(pz->mod_lfo_pit.value*200.0f)-100.0f);
		if(index==179)sprintf(text,"%f",pz->mod_lfo_pit.sense);
		if(index==180)format_db(pz->efx_chr_lev.value,text);
		if(index==181)sprintf(text,"%f",pz->efx_chr_lev.sense);
		if(index==182)format_db(pz->efx_del_lev.value,text);
		if(index==183)sprintf(text,"%f",pz->efx_del_lev.sense);
		if(index==184)format_db(pz->efx_rev_lev.value,text);
		if(index==185)sprintf(text,"%f",pz->efx_rev_lev.sense);
	}
	else
	{
		if(index==128)sprintf(text,"Not Available");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLife::getParameterName(VstInt32 index, char *text)
{
	hlf_Program* pprog=&bank.program[user_program];

	// default name
	sprintf(text,"Reserved");

	if(index==0)sprintf(text,"HighLife R2");

	// enviroment parameter names
	if(index==1)sprintf(text,"Mst Vol");

	// program parameter names
	if(index==16)sprintf(text,"Efx Cho Dly");
	if(index==17)sprintf(text,"Efx Cho Fdb");
	if(index==18)sprintf(text,"Efx Cho Rat");
	if(index==19)sprintf(text,"Efx Cho Mod");
	if(index==20)sprintf(text,"Efx Dly Tim");
	if(index==21)sprintf(text,"Efx Dly Fdb");
	if(index==22)sprintf(text,"Efx Rev Roo");
	if(index==23)sprintf(text,"Efx Rev Wid");
	if(index==24)sprintf(text,"Efx Rev Dam");
	if(index==25)sprintf(text,"Efx Dft Rat");
	
	// sample zone layer automation parameter names
	if(index==128)sprintf(text,"Zone Edit");
	if(index==129)sprintf(text,"Zone Loop Start");
	if(index==130)sprintf(text,"Zone Loop End");
	if(index==131)sprintf(text,"Zone Offset Start");
	if(index==132)sprintf(text,"Gli Aut");
	if(index==133)sprintf(text,"Vel Amp");
	if(index==134)sprintf(text,"Vel Mod");
//	if(index==135)sprintf(text,"Ply Mod");
	if(index==136)sprintf(text,"Flt Typ");
	if(index==137)sprintf(text,"Sys Pit Rng");
	if(index==138)sprintf(text,"Flt Cut Kbd Trk");
	if(index==139)sprintf(text,"Flt Cut Vel Trk");
	if(index==140)sprintf(text,"Flt Cut");
	if(index==141)sprintf(text,"Flt Cut Sen");
	if(index==142)sprintf(text,"Flt Res");
	if(index==143)sprintf(text,"Flt Res Sen");
	if(index==144)sprintf(text,"Gli Amt");
	if(index==145)sprintf(text,"Gli Amt Sen");
	if(index==146)sprintf(text,"Amp Env Att");
	if(index==147)sprintf(text,"Amp Env Att Sen");
	if(index==148)sprintf(text,"Amp Env Dec");
	if(index==149)sprintf(text,"Amp Env Dec Sen");
	if(index==150)sprintf(text,"Amp Env Sus");
	if(index==151)sprintf(text,"Amp Env Sus Sen");
	if(index==152)sprintf(text,"Amp Env Rel");
	if(index==153)sprintf(text,"Amp Env Rel Sen");
	if(index==154)sprintf(text,"Amp Env Amt");
	if(index==155)sprintf(text,"Amp Env Amt Sen");
	if(index==156)sprintf(text,"Mod Env Att");
	if(index==157)sprintf(text,"Mod Env Att Sen");
	if(index==158)sprintf(text,"Mod Env Dec");
	if(index==159)sprintf(text,"Mod Env Dec Sen");
	if(index==160)sprintf(text,"Mod Env Sus");
	if(index==161)sprintf(text,"Mod Env Sus Sen");
	if(index==162)sprintf(text,"Mod Env Rel");
	if(index==163)sprintf(text,"Mod Env Rel Sen");
	if(index==164)sprintf(text,"Mod Env Cut");
	if(index==165)sprintf(text,"Mod Env Cut Sen");
	if(index==166)sprintf(text,"Mod Env Pit");
	if(index==167)sprintf(text,"Mod Env Pit Sen");
	if(index==168)sprintf(text,"Mod Lfo Phs");
	if(index==169)sprintf(text,"Mod Lfo Phs Sen");
	if(index==170)sprintf(text,"Mod Lfo Rat");
	if(index==171)sprintf(text,"Mod Lfo Rat Sen");
	if(index==172)sprintf(text,"Mod Lfo Amp");
	if(index==173)sprintf(text,"Mod Lfo Amp Sen");
	if(index==174)sprintf(text,"Mod Lfo Cut");
	if(index==175)sprintf(text,"Mod Lfo Cut Sen");
	if(index==176)sprintf(text,"Mod Lfo Res");
	if(index==177)sprintf(text,"Mod Lfo Res Sen");
	if(index==178)sprintf(text,"Mod Lfo Pit");
	if(index==179)sprintf(text,"Mod Lfo Pit Sen");
	if(index==180)sprintf(text,"Efx Cho Lev");
	if(index==181)sprintf(text,"Efx Cho Lev Sen");
	if(index==182)sprintf(text,"Efx Dly Lev");
	if(index==183)sprintf(text,"Efx Dly Lev Sen");
	if(index==184)sprintf(text,"Efx Rev Lev");
	if(index==185)sprintf(text,"Efx Rev Lev Sen");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CHighLife::canParameterBeAutomated(VstInt32 index)
{
	// all parameters can be automated
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VstInt32 CHighLife::getNumAutomatableParameters(void)
{
	return NUM_PARAMETERS;
}
	