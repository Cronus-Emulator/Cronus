/*-------------------------------------------------------------------------|
| _________                                                                |
| \_   ___ \_______  ____   ____  __ __  ______                            |
| /    \  \/\_  __ \/    \ /    \|  |  \/  ___/                            |
| \     \____|  | \(  ( ) )   |  \  |  /\___ \                             |
|  \______  /|__|   \____/|___|  /____//____  >                            |
|         \/                   \/           \/                             |
|--------------------------------------------------------------------------|
| Copyright (C) <2014>  <Cronus - Emulator>                                |
|	                                                                       |
| Copyright Portions to eAthena, jAthena and Hercules Project              |
|                                                                          |
| This program is free software: you can redistribute it and/or modify     |
| it under the terms of the GNU General Public License as published by     |
| the Free Software Foundation, either version 3 of the License, or        |
| (at your option) any later version.                                      |
|                                                                          |
| This program is distributed in the hope that it will be useful,          |
| but WITHOUT ANY WARRANTY; without even the implied warranty of           |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
| GNU General Public License for more details.                             |
|                                                                          |
| You should have received a copy of the GNU General Public License        |
| along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
|                                                                          |
|----- Descrição: ---------------------------------------------------------| 
|                                                                          |
|--------------------------------------------------------------------------|
|                                                                          |
|----- ToDo: --------------------------------------------------------------| 
|                                                                          |
|-------------------------------------------------------------------------*/

#include "utils.h"

#include <math.h> // floor()
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> // cache purposes [Ind/Hercules]

#include "../common/cbasetypes.h"
#include "../common/core.h"
#include "../common/malloc.h"
#include "../common/mmo.h"
#include "../common/showmsg.h"
#include "../common/socket.h"

#ifdef WIN32
#	include "../common/winapi.h"
#	ifndef F_OK
#		define F_OK   0x0
#	endif  /* F_OK */
#   define MAX_DIR_PATH 2048
#else
#	include <dirent.h>
#	include <unistd.h>
#endif


struct HCache_interface HCache_s;

bool exists(const char* filename)
{
	return !access(filename, F_OK);
}

uint8 GetByte(uint32 val, int idx)
{
	switch( idx )
	{
	case 0: return (uint8)( (val & 0x000000FF)         );
	case 1: return (uint8)( (val & 0x0000FF00) >> 0x08 );
	case 2: return (uint8)( (val & 0x00FF0000) >> 0x10 );
	case 3: return (uint8)( (val & 0xFF000000) >> 0x18 );
	default:
#if defined(DEBUG)
		ShowDebug("GetByte: invalid index (idx=%d)\n", idx);
#endif
		return 0;
	}
}

uint16 GetWord(uint32 val, int idx)
{
	switch( idx )
	{
	case 0: return (uint16)( (val & 0x0000FFFF)         );
	case 1: return (uint16)( (val & 0xFFFF0000) >> 0x10 );
	default:
#if defined(DEBUG)
		ShowDebug("GetWord: invalid index (idx=%d)\n", idx);
#endif
		return 0;
	}
}
uint16 MakeWord(uint8 byte0, uint8 byte1)
{
	return byte0 | (byte1 << 0x08);
}

uint32 MakeDWord(uint16 word0, uint16 word1)
{
	return
		( (uint32)(word0        ) )|
		( (uint32)(word1 << 0x10) );
}

/*************************************
* Big-endian compatibility functions *
*************************************/

// Converts an int16 from current machine order to little-endian
int16 MakeShortLE(int16 val)
{
	unsigned char buf[2];
	buf[0] = (unsigned char)( (val & 0x00FF)         );
	buf[1] = (unsigned char)( (val & 0xFF00) >> 0x08 );
	return *((int16*)buf);
}

// Converts an int32 from current machine order to little-endian
int32 MakeLongLE(int32 val)
{
	unsigned char buf[4];
	buf[0] = (unsigned char)( (val & 0x000000FF)         );
	buf[1] = (unsigned char)( (val & 0x0000FF00) >> 0x08 );
	buf[2] = (unsigned char)( (val & 0x00FF0000) >> 0x10 );
	buf[3] = (unsigned char)( (val & 0xFF000000) >> 0x18 );
	return *((int32*)buf);
}

// Reads an uint16 in little-endian from the buffer
uint16 GetUShort(const unsigned char* buf)
{
	return	 ( ((uint16)(buf[0]))         )
			|( ((uint16)(buf[1])) << 0x08 );
}

// Reads an uint32 in little-endian from the buffer
uint32 GetULong(const unsigned char* buf)
{
	return	 ( ((uint32)(buf[0]))         )
			|( ((uint32)(buf[1])) << 0x08 )
			|( ((uint32)(buf[2])) << 0x10 )
			|( ((uint32)(buf[3])) << 0x18 );
}

// Reads an int32 in little-endian from the buffer
int32 GetLong(const unsigned char* buf)
{
	return (int32)GetULong(buf);
}

// Reads a float (32 bits) from the buffer
float GetFloat(const unsigned char* buf)
{
	uint32 val = GetULong(buf);
	return *((float*)(void*)&val);
}

/// calculates the value of A / B, in percent (rounded down)
unsigned int get_percentage(const unsigned int A, const unsigned int B)
{
	double result;

	if( B == 0 )
	{
		ShowError("get_percentage(): division by zero! (A=%u,B=%u)\n", A, B);
		return ~0U;
	}

	result = 100 * ((double)A / (double)B);

	if( result > UINT_MAX )
	{
		ShowError("get_percentage(): result percentage too high! (A=%u,B=%u,result=%g)\n", A, B, result);
		return UINT_MAX;
	}

	return (unsigned int)floor(result);
}

//-----------------------------------------------------
// custom timestamp formatting (from eApp)
//-----------------------------------------------------
const char* timestamp2string(char* str, size_t size, time_t timestamp, const char* format)
{
	size_t len = strftime(str, size, format, localtime(&timestamp));
	memset(str + len, '\0', size - len);
	return str;
}


/* [Ind/Hercules] Caching */
bool HCache_check(const char *file) {
	struct stat bufa, bufb;
	FILE *first, *second;
	char s_path[255], dT[1];
	time_t rtime;

	if( !(first = fopen(file,"rb")) )
	   return false;

	if( file[0] == '.' && file[1] == '/' )
	   file += 2;
	else if( file[0] == '.' )
	   file++;
	   
	snprintf(s_path, 255, "./cache/%s", file);
	   
	if( !(second = fopen(s_path,"rb")) ) {
		fclose(first);
		return false;
	}

	if( fread(dT,sizeof(dT),1,second) != 1 || fread(&rtime,sizeof(rtime),1,second) != 1 || dT[0] != HCACHE_KEY || HCache->recompile_time > rtime ) {
		fclose(first);
		fclose(second);
		return false;
	}
		
	fstat(fileno(first), &bufa);
	fstat(fileno(second), &bufb);
	
	fclose(first);
	fclose(second);
	   
	if( bufa.st_mtime > bufb.st_mtime )
		return false;
	
	return true;
}

FILE *HCache_open(const char *file, const char *opt) {
	FILE *first;
	char s_path[255];
		
	if( file[0] == '.' && file[1] == '/' )
		file += 2;
	else if( file[0] == '.' )
		file++;
	
	snprintf(s_path, 255, "./cache/%s", file);
	
	if( !(first = fopen(s_path,opt)) ) {
		return NULL;
	}
	
	if( opt[0] != 'r' ) {
		char dT[1];/* 1-byte key to ensure our method is the latest, we can modify to ensure the method matches */
		dT[0] = HCACHE_KEY;
		hwrite(dT,sizeof(dT),1,first);
		hwrite(&HCache->recompile_time,sizeof(HCache->recompile_time),1,first);
	}
	fseek(first, 20, SEEK_SET);/* skip first 20, might wanna store something else later */
	
	return first;
}
void HCache_init(void) {
	FILE *server;
	
	if( (server = fopen(SERVER_NAME,"rb")) ) {
		struct stat buf;
		
		fstat(fileno(server), &buf);
		HCache->recompile_time = buf.st_mtime;
		fclose(server);
		
		HCache->enabled = true;
	} else
		ShowWarning("Unable to open '%s', caching capabilities have been disabled!\n",SERVER_NAME);
}
/* transit to fread, shields vs warn_unused_result */
size_t hread(void * ptr, size_t size, size_t count, FILE * stream) {
	return fread(ptr, size, count, stream);
}
/* transit to fwrite, shields vs warn_unused_result */
size_t hwrite(const void * ptr, size_t size, size_t count, FILE * stream) {
	return fwrite(ptr, size, count, stream);
}

void HCache_defaults(void) {
	
	HCache = &HCache_s;
	HCache->init = HCache_init;
	HCache->check = HCache_check;
	HCache->open = HCache_open;
	HCache->recompile_time = 0;
	HCache->enabled = false;
}
