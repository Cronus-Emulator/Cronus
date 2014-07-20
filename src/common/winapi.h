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

//Define que a header seja incluída apenas uma única vez.
#pragma once

// Define diretivas de segurança para portabilidade
#define STRICT

//Exclui a inclusão de headers para Criptografia, DDE, RPC, Shell, and SocketsV1.
#define WIN32_LEAN_AND_MEAN

// Reduz as inclusões de funções quando Windows.h é declarada.
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOMCX
#define NOCLIPBOARD
#define NOCOLOR
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC

/*
io.h - Define as funções/constantes para criação/manipulação de arquivos.
Windows.h - Define as funções/constantes da WinAPI
Winsock2.h - Define as funções/constantes de Socket V2 para uso.
Ws2tcpip.h - Define as funções/constantes que declaram as especificações da TCP/IPv4-v6
Mswsock.h - Define as extensões para as funções de Socket V2
*/

#include <io.h>
#include <Windows.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Mswsock.h>

