/*-
 * SFZ PARSER for DiscoDsp HighLife VST Sampler
 *   
 * file name: scSfzParser.cpp
 *   
 * Copyright (c) Andreas Sumerauer 2009
 * All rights reserved.
 *
 * the sfz format specification is provided by  Cakewalk Inc.
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

#include "sbSfzParser.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sbSfzFileName::getTitle(char* sfzTitle)
{
	char* startPos = strrchr( sfzName, '\\') + 1;
	strcpy( sfzTitle, startPos );
	char* endPos = strrchr( sfzTitle, '.' );
	if (endPos != NULL)
		*endPos = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sbSfzFileName::getPath( char* itemPath, char* itemName)
{
	if ( itemPath != itemName ){
		strcpy( itemPath, itemName );
	}
	char* endPos = strrchr( itemPath, '\\' );
	*endPos = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sbSfzFileName::getFullSampleName( char* fullName, char* sampleName)
{
	if (sampleName[1] == ':'){
		// samplePath is an absolute path (including drive letter) 
		strcpy( fullName, sampleName );
		// 
		// TODO: issue a warning if sample is not on same drive with sfz file
		//
	}else{
		// samplePath is a relative path 
		getPath( fullName, sfzName );
		while ( strncmp(sampleName, "..\\", 3) == 0 ){
			sampleName +=3;
			getPath( fullName, fullName );
		}
		strcat(fullName, "\\");
		strcat(fullName, sampleName);
	}
	// 
	// TODO: check if file can be loaded !
	// (careful: an already loaded sample could cause a load error)
	//
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sbSfzZone::init()
{
	memset( smpfilename, 0, sizeof( smpfilename ) );
	lokey = 0;
	hikey = 127;
	lovel = 0;
	hivel = 127;
	offset = 0;
	tune = 0;
	pitch_keytrack = 100;
	root_key = -1;
	transpose = 0;
	volume = 0.0f;
	pan = 0.0f;
	loop_start = -1;
	loop_end = -1;
	loop_mode = -1;
	for (int i=0; i<128 ; ++i ){
		locc[i] = 0;
		hicc[i] = 127;
	}
	lobend = -8192;
	hibend = 8192;
	lochanaft = 0;
	hichanaft = 127;
	lopolyaft = 0;
	hipolyaft = 127;
	trigger = TRIG_ATTACK;
	group = 0;
	off_by = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool sbSfzParser::readFile(char* sfzFileName, HWND const hwnd, hlf_Program* pprg )
{
	bool returnVal = false;
	pprg->init();
	fileName.init( sfzFileName );
	// init program name
	// Better don't use GetFileTitle() here since it does not always include the file extension
	// ( depending on user settings in the windows file browser )
	fileName.getTitle( pprg->name );

	// open the file
	FILE* pFile=fopen( sfzFileName, "rb" );

	// file opened ok
	if(pFile)
	{	
		// set default values for group parameters
		group.init();
		// process lines
		while(!feof(pFile))
		{
			// get line
			if( !fetchLine( pFile ) ) break;

			// split line into opcodes and group/region markers
			while (pBlock != NULL){
				splitBlock();
				if ( params != NULL )
					getBlockOpcodes();
				if ( zoneComplete ){
					if ( params == &region )
						loadHighlifeZone( hwnd, pprg );
					newRegion();
				}
			}
		}
		// write last zone
		if ( params == &region )
			loadHighlifeZone( hwnd, pprg );
		// close file
		fclose(pFile);
		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool sbSfzParser::fetchLine( FILE* pFile )
{
	pBlock = NULL;
	memset( line, 0, sizeof (line) );
	if( fgets( line, 512, pFile) == NULL ){
		return false;
	}else{
		pBlock = line;
		return true;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool sbSfzParser::splitBlock() 
{
	char* pResume = NULL;
	char* pEnd = strchr ( pBlock, 0 );
	opCount = 0;

	// truncate comments
	char * pChar = strchr ( pBlock, '/' );
	if (pChar != NULL) *pChar = 0;

	// find <group> and <region> markers 
	pChar =  strchr ( pBlock, '<' );
	if (pChar != NULL){
		if ( strncmp(pChar, "<group>", 7 ) == 0 ){
			if ( pChar+7 < pEnd )
				pResume = pChar + 7;
			headerState = HD_GROUP;
		}else if ( strncmp(pChar, "<region>", 8) == 0 ){
			if ( pChar+8 < pEnd )
				pResume = pChar + 8;
			headerState = HD_REGION;
		}else{
			char* pChar1 = strchr( pChar, '>' );
			if ( pChar1 != 0 ){
				// matching '>' found. This seems to be an unknown header.
				// ignore until next valid header is found
				if ( pChar1+1 < pEnd )
				pResume = pChar1 + 1;
				headerState = HD_UNSUPPORTED;
			} else {				
				// no matching '>' present - sfz file is corrupted!
				// TODO: break off here cleanly.
				headerState = HD_UNDEFINED;
			}
		}
		zoneComplete = true;
		*pChar = 0;
	}
	// find '=' in remaining string
	pChar = strchr ( pBlock, '=' );
	while (pChar != 0){
		rVal[opCount] = pChar + 1;
		*pChar = 0;
		pChar -= 1;
		while( pChar  >= pBlock ){
			if ( *pChar == ' ' ){
				*pChar = 0;
				++pChar;
				break;
			}
			if ( pChar == pBlock )
				break;
			--pChar;
		}
		lVal[opCount] = pChar;
		pChar = strchr ( rVal[opCount], '=' );
		++opCount;
	}

	// remove whitespace from the end of each rVal substring!
	for ( int i=0 ; i<opCount ; ++i ){
		pChar = strchr( rVal[i], 0 );
		--pChar;
		while( *pChar == ' ' || *pChar == '\n' || *pChar == '\r' ){
			*pChar = 0;
			--pChar;
		}
	}
	pBlock = pResume; // hand new value for pBlock back (resume pointer or NULL if line is completed)
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool sbSfzParser::newRegion()
{
	zoneComplete = false;
	if( headerState <= HD_UNSUPPORTED ){
		params = NULL;
		return false;
	}
	if( headerState == HD_GROUP ){
		group.init();
		params = &group;
	}else if ( headerState == HD_REGION ){
		memcpy(&region, &group, sizeof(sbSfzZone) );
		params = &region;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool sbSfzParser::loadHighlifeZone( HWND hwnd, hlf_Program* pprg )
{
	int zone_index = 0;
	hlf_Zone* pz = pprg->allocZone(&zone_index);
	if (pz == NULL) return false;

	// load sample
	pz->init( &pBank->defaultZone );
	if (pz->loadWave(hwnd, region.smpfilename) == 0){
		// TODO: let user locate Sample on HD. if no wave is found reset to default wave
		// TODO: put repeated code into a single module
		pprg->deleteZone(zone_index);
		return false;
	}

	// import zone parameters
	pz->lo_input_range.midi_key = region.lokey;
	pz->hi_input_range.midi_key = region.hikey;
	pz->lo_input_range.midi_vel = region.lovel;
	pz->hi_input_range.midi_vel = region.hivel;
	if (region.offset > 0){
		pz->cue_pos[0]=region.offset;
		pz->num_cues=1;
	}
	pz->midi_keycents = region.pitch_keytrack;
	// override keyroot
	// TODO: check if rootkey is set to midi note 60 if no 
	// keyroot is specified in sample file 
	if( region.root_key >= 0 )
		pz->midi_root_key = region.root_key;
	pz->midi_fine_tune = region.tune;
	pz->midi_coarse_tune = region.transpose;
	pz->mp_gain = int( region.volume );
	pz->mp_pan = int( region.pan*0.5f );
	// override loop points
	if( region.loop_mode >= 0 ){
		pz->pWave->loop_mode = region.loop_mode;
		if( region.loop_start >= 0 )
			pz->pWave->loop_start = region.loop_start;
		if( region.loop_end >= 0)
			pz->pWave->loop_end = region.loop_end;
	}
	for(int i=0 ; i<128 ; ++i){
		pz->lo_input_range.midi_cc[i] = region.locc[i];
		pz->hi_input_range.midi_cc[i] = region.hicc[i];
	}
	pz->lo_input_range.midi_bend=region.lobend;
	pz->hi_input_range.midi_bend=region.hibend;
	pz->lo_input_range.midi_chanaft=region.lochanaft;
	pz->hi_input_range.midi_chanaft=region.hichanaft;
	pz->lo_input_range.midi_polyaft=region.lopolyaft;
	pz->hi_input_range.midi_polyaft=region.hipolyaft;
	if( region.trigger == 1 )pz->midi_trigger=1;
	pz->res_group=region.group;
	pz->res_offby=region.off_by;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sbSfzParser::getBlockOpcodes()
{
	for ( int i=0 ; i<opCount ; i++ ){
		int opIdx = opcode.eval( lVal[i] );
		switch ( opIdx ) 
		{
		case OP_SAMPLE:
			fileName.getFullSampleName( params->smpfilename, rVal[i] );
			TRACE1( params->smpfilename );
			break;
		case OP_LOKEY:
			getNoteNumberFromName( rVal[i], params->lokey );
			break;
		case OP_HIKEY:
			getNoteNumberFromName( rVal[i], params->hikey );
			break;
		case OP_KEY:
			getNoteNumberFromName( rVal[i], params->lokey );
			params->root_key = params->hikey = params->lokey;
			break;
		case OP_LOVEL:
			params->lovel=atoi( rVal[i] );
			break;
		case OP_HIVEL:
			params->hivel=atoi( rVal[i] );
			break;
		case OP_OFFSET:
			params->offset=atoi( rVal[i] );
			break;
		case OP_TUNE:
			params->tune=atoi( rVal[i] );
			break;
		case OP_PITCH_KEYTRACK:
			params->pitch_keytrack=atoi( rVal[i] );
			break;
		case OP_PITCH_KEYCENTER:
			getNoteNumberFromName( rVal[i], params->root_key );
			break;
		case OP_TRANSPOSE:
			params->transpose=atoi( rVal[i] );
			break;
		case OP_VOLUME:
			params->volume=(float)atof( rVal[i] );
			break;
		case OP_PAN:
			params->pan=(float)atof( rVal[i] );
			break;
		case OP_LOOP_START:
			params->loop_start=atoi( rVal[i] );
			break;
		case OP_LOOP_END:
			params->loop_end=atoi( rVal[i] );
			break;
		case OP_LOOP_MODE:
			// no loop
			if(strncmp( rVal[i] ,"no_loop",7)==0){
				params->loop_mode=0;
			// loop forward
			}else if(strncmp( rVal[i] ,"loop_continuous",15)==0){
				params->loop_mode=1;
			// loop bidirectional
			}else if(strncmp( rVal[i] ,"loop_bidirectional",18)==0){
				params->loop_mode=2;
			// loop backward
			}else if(strncmp( rVal[i] ,"loop_backward",13)==0){
				params->loop_mode=3;
			// loop forward sustained
			}else if(strncmp( rVal[i] ,"loop_sustain",12)==0){
				params->loop_mode=4;
			// invalid opcode
			}else{
				// TODO: Warning! Not a valid Loopmode!
			}
			break;
		case OP_LOCCN:
			if(opcode.ccNum>=0 && opcode.ccNum<128)
				params->locc[opcode.ccNum]=atoi( rVal[i] );
			break;
		case OP_HICCN:
			if(opcode.ccNum>=0 && opcode.ccNum<128)
				params->hicc[opcode.ccNum]=atoi( rVal[i] );
			break;
		case OP_LOBEND:
			params->lobend=atoi( rVal[i] );
			break;
		case OP_HIBEND:
			params->hibend=atoi( rVal[i] );
			break;
		case OP_LOCHANAFT:
			params->lochanaft=atoi( rVal[i] );
			break;
		case OP_HICHANAFT:
			params->hichanaft=atoi( rVal[i] );
			break;
		case OP_LOPOLYAFT:
			params->lopolyaft=atoi( rVal[i] );
			break;
		case OP_HIPOLYAFT:
			params->hipolyaft=atoi( rVal[i] );
			break;
		case OP_TRIGGER:
			if(strncmp( rVal[i] ,"release",7)==0)	// trigger release
				params->trigger=1;
			break;
		case OP_GROUP:
			params->group=atoi( rVal[i] );
			break;
		case OP_OFF_BY:
			params->off_by=atoi( rVal[i] );
			break;
		case OP_INVALID:
			{int i = 0;}
			//
			// Invalid opcode (not included in sfz 1.0 specs)
			//
			break;
		default:
			{int i = 0;}
			//
			// Valid opcode however unsupported 
			//
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool sbSfzParser::getNoteNumberFromName(char * input, int &noteNum) 
{
	// return values true: input accepted
	//               false: input rejected      

	int sign = 1;
	int temp_noteNum;

	noteNum = 0;

	// strip leading blanks
	while (strncmp (input," ",1) == 0) {
		input++;
	}

	// translate note name to note number

	if (strncmp (input,"C",1) == 0 || strncmp (input,"c",1) == 0)
		temp_noteNum = 0;
	else if (strncmp (input,"D",1) == 0 || strncmp (input,"d",1) == 0 )
		temp_noteNum = 2;
	else if (strncmp (input,"E",1) == 0 || strncmp (input,"e",1) == 0 )
		temp_noteNum = 4;
	else if (strncmp (input,"F",1) == 0 || strncmp (input,"f",1) == 0 )
		temp_noteNum = 5;
	else if (strncmp (input,"G",1) == 0 || strncmp (input,"g",1) == 0 )
		temp_noteNum = 7;
	else if (strncmp (input,"A",1) == 0 || strncmp (input,"a",1) == 0 )
		temp_noteNum = 9;
	else if (strncmp (input,"B",1) == 0 || strncmp (input,"b",1) == 0 )
		temp_noteNum = 11;
	else { // Input string is not a note name:
		temp_noteNum = (float)atof(input);
		// is it a number?
		if (temp_noteNum == 0 && !(strncmp (input, "0", 1) == 0)){
			return false;
		}
		// is the number inside  0 ... 127 ?
		if (temp_noteNum < 0 || temp_noteNum > 127){
			return false;
		}
		noteNum = temp_noteNum;
		return true;
	}

	input++;
	if (strncmp (input,"#",1) == 0 ) {
		temp_noteNum += 1;
		input++;
	}

	if (strncmp (input,"-",1) == 0 ) {
		sign = -1;
		input++;
	}
	
	if (strncmp (input,"0",1) == 0 )
		temp_noteNum += 24;
	else if (strncmp (input,"1",1) == 0 )
		temp_noteNum += 24 + (sign * 12);
	else if (strncmp (input,"2",1) == 0 )
		temp_noteNum += 24 + (sign * 24);
	else if (strncmp (input,"3",1) == 0 )
		temp_noteNum += 24 + (sign * 36);
	else if (strncmp (input,"4",1) == 0 )
		temp_noteNum += 24 + (sign * 48);
	else if (strncmp (input,"5",1) == 0 )
		temp_noteNum += 24 + (sign * 60);
	else if (strncmp (input,"6",1) == 0 )
		temp_noteNum += 24 + (sign * 72);
	else if (strncmp (input,"7",1) == 0 )
		temp_noteNum += 24 + (sign * 84);
	else if (strncmp (input,"8",1) == 0 )
		temp_noteNum += 24 + (sign * 96);
	else {
		return false;
	}
	// is the number inside  0 ... 127 ?
	if (temp_noteNum < 0 || temp_noteNum > 127){
		return false;
	}
	noteNum = temp_noteNum;

	input++;
	if (strncmp (input,".",1) == 0 ) {
		noteNum += atoi(input);
	}
	// MessageBox(GetForegroundWindow(),"still alive Pitch is noteName","import tuning table",0);
	return true;
}



