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

#include ".\ddspguifiledialog.h"
#include "stdio.h"

CDDSPGuiFileDialog::CDDSPGuiFileDialog(void)
{
	num_opened_files=0;
}

CDDSPGuiFileDialog::~CDDSPGuiFileDialog(void)
{
}

int CDDSPGuiFileDialog::SaveFileDialog(HWND const hwnd,char* title,char* filename,char* filter,char* default_extension,char* initial_dir)
{
	// filename struct holder
	OPENFILENAME OFN;
	memset(&OFN,0,sizeof(OPENFILENAME));

	// setup structure
	OFN.lpstrTitle=title;
	OFN.lStructSize=sizeof(OPENFILENAME);
	OFN.hwndOwner=hwnd;
	OFN.lpstrFilter=filter;
	OFN.lpstrFile=filename;
	OFN.nMaxFile=MAX_PATH;
	OFN.Flags=OFN_EXPLORER;
	OFN.lpstrInitialDir=initial_dir;
	OFN.lpstrDefExt=default_extension;

	// save program accepted
	if(GetSaveFileName(&OFN)!=0)
		return 1;

	return 0;
}

int CDDSPGuiFileDialog::OpenFileDialog(HWND const hwnd,char* title,char* filename,char* filter,char* default_extension,char* initial_dir,bool const allow_multi)
{
	// filename struct holder
	OPENFILENAME OFN;
	memset(&OFN,0,sizeof(OPENFILENAME));

	// file names holder
	char* lpstrFile=new char[MAX_FILES*MAX_PATH];
	sprintf(lpstrFile,filename);

	// setup structure
	OFN.lpstrTitle=title;
	OFN.lStructSize=sizeof(OPENFILENAME);
	OFN.hwndOwner=hwnd;
	OFN.lpstrFilter=filter;
	OFN.lpstrFile=lpstrFile;
	OFN.nFilterIndex=0;
	OFN.nMaxFile=MAX_FILES*MAX_PATH;
	OFN.lpstrInitialDir=initial_dir;
	OFN.Flags=OFN_EXPLORER;
	
	// check multiselectin flag
	if(allow_multi)
	{
		// allow multi
		OFN.Flags|=OFN_ALLOWMULTISELECT;
	}
	else
	{
		// single sel
		if(GetOpenFileName(&OFN)!=0)
		{
			// copy first name
			sprintf(open_filenames[0],lpstrFile);
			num_opened_files=1;
			delete[] lpstrFile;
			return 1;
		}
		else
		{
			// cancelled , delete char data and return failed
			num_opened_files=0;
			delete[] lpstrFile;
			return 0;
		}
	}

	// reset index
	num_opened_files=0;

	// check for file(s)
	if(GetOpenFileName(&OFN)!=0)
	{
		// pointer
		char* p=lpstrFile;

		// copy first name
		sprintf(open_filenames[0],p);

		// check for double zero
		if(p[strlen(p)+1]==0)
		{
			// single select
			num_opened_files=1;
		}
		else
		{
			char prefix[_MAX_PATH];
			sprintf(prefix,open_filenames[0]);

			// add the '\'
			if(prefix[strlen(prefix)-1]!='\\')
			{
				int const strl=int(strlen(prefix));
				prefix[strl]='\\';
				prefix[strl+1]=NULL;
			}

			// no double zero, get more files (multiselect)
			for(;;)
			{
				p=p+strlen(p)+1;

				// exit if double 0
				if(*p==0)
					break;

				// check for bound limit
				if(num_opened_files<MAX_FILES)
				{
					// copy next name
					sprintf(open_filenames[num_opened_files],"%s%s",prefix,p);

					// increment file counter
					num_opened_files++;	
				}
			}
		}

		// delete char data and return success
		delete[] lpstrFile;
		return 1;
	}

	// dialog cancelled, delete char data return 0
	delete[] lpstrFile;
	return 0;
}

int CDDSPGuiFileDialog::GetOpenFileCount()
{
	return num_opened_files;
}

char* CDDSPGuiFileDialog::GetFileName(int const index)
{
	return open_filenames[index];
}
