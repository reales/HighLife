/*-
 * SFZ PARSER for DiscoDsp HighLife VST Sampler
 *   
 * file name: scSfzParser.h
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

#include <cstring>
#include ".\highlife.h"
#include "sbSfzOpcode.h"

#define HD_GROUP 2
#define HD_REGION 1
#define HD_UNSUPPORTED 0
#define HD_UNDEFINED -1 


class sbSfzFileName
{
public:
	sbSfzFileName(){
		memset( sfzName, 0, sizeof(sfzName) );
	};
	sbSfzFileName(char * sfzName){
		memset( sfzName, 0, sizeof(sfzName) );
		init(sfzName); 
	};
	void init(char * sfzName){
		strcpy(this->sfzName, sfzName); 
	};
	void getTitle(char* sfzTitle); 
	void getPath(char* itemPath, char* itemName);
	void getFullSampleName(char* fullName, char* sampleName);
private:
	char  sfzName[256];
};

class sbSfzZone
{
public:
	sbSfzZone(){ init(); };
	void init();
	
	char smpfilename[_MAX_PATH];
	int lokey;
	int hikey;
	int lovel;
	int hivel;
	int offset;
	int tune;
	int pitch_keytrack;
	int root_key;
	int transpose;
	float volume;
	float pan;
	int	loop_start;
	int	loop_end;
	int	loop_mode;
	int locc[128];
	int hicc[128];
	int lobend;
	int hibend;
	int lochanaft;
	int hichanaft;
	int lopolyaft;
	int hipolyaft;
	int trigger;
	int group;
	int off_by;
};

class sbSfzParser
{
public:
	sbSfzParser( hlf_Bank * pBank ) : headerState(HD_UNDEFINED), params(NULL), pBank(pBank), zoneComplete(false){};
	bool readFile( char* sfzFileName, HWND const hwnd, hlf_Program* pprg ); 

private:
	bool fetchLine( FILE* pFile );
	bool splitBlock(); 
	void getBlockOpcodes();
	bool getNoteNumberFromName(char * input, int &noteNum);

	bool newRegion();

	sbSfzFileName fileName;
	sbSfzZone * params;
	sbSfzZone group, region;
	sbSfzOpcode opcode;

	char line[1024];
	char * lVal[64];
	char * rVal[64];
	char * pBlock; 

	bool zoneComplete;

	hlf_Bank * pBank; // pointer to highlife Bank object
	bool loadHighlifeZone( HWND hwnd, hlf_Program* pprg );

	int opCount; // counter for number of opcodes that are present within a block/line.
	int headerState;// can have the values: GROUP (2), REGION (1) or UNDEFINED (-1)
};
