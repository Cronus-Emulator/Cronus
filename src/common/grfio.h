//====================================================================\\
//                   _____                                            ||
//                  /  __ \                                           ||
//                  | /  \/_ __ ___  _ __  _   _ ___                  ||
//                  | |   | '__/ _ \| '_ \| | | / __|                 ||
//                  | \__/\ | | (_) | | | | |_| \__ \                 ||
//                   \____/_|  \___/|_| |_|\__,_|___/                 ||
//                        Source - 2016                               ||
//====================================================================||
// = Arquivo:                                                         ||
// - grfio.h                                                          ||
//====================================================================||
// = Código Base:                                                     ||
// - eAthena/Hercules/Cronus                                          ||
//====================================================================||
// = Sobre:                                                           ||
// Este software é livre: você pode redistribuí-lo e/ou modificá-lo   ||
// sob os termos da GNU General Public License conforme publicada     ||
// pela Free Software Foundation, tanto a versão 3 da licença, ou     ||
// (a seu critério) qualquer versão posterior.                        ||
//                                                                    ||
// Este programa é distribuído na esperança de que possa ser útil,    ||
// mas SEM QUALQUER GARANTIA; mesmo sem a garantia implícita de       ||
// COMERCIALIZAÇÃO ou ADEQUAÇÃO A UM DETERMINADO FIM. Veja a          ||
// GNU General Public License para mais detalhes.                     ||
//                                                                    ||
// Você deve ter recebido uma cópia da Licença Pública Geral GNU      ||
// juntamente com este programa. Se não, veja:                        ||
// <http://www.gnu.org/licenses/>.                                    ||
//====================================================================//

#ifndef COMMON_GRFIO_H
#define COMMON_GRFIO_H

#ifdef CRONUS_CORE
void grfio_init(const char* fname);
void grfio_final(void);
void* grfio_reads(const char* fname, int* size);
char* grfio_find_file(const char* fname);
#define grfio_read(fn) grfio_reads((fn), NULL)

unsigned long grfio_crc32(const unsigned char *buf, unsigned int len);
int decode_zip(void* dest, unsigned long* destLen, const void* source, unsigned long sourceLen);
int encode_zip(void* dest, unsigned long* destLen, const void* source, unsigned long sourceLen);
#endif // CRONUS_CORE

#endif /* COMMON_GRFIO_H */
