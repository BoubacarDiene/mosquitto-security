//////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                              //
//          Copyright © 2020 Boubacar DIENE                                                     //
//                                                                                              //
//          This file is part of mosquitto-security project.                                    //
//                                                                                              //
//          mosquitto-security is free software: you can redistribute it and/or modify          //
//          it under the terms of the GNU General Public License as published by                //
//          the Free Software Foundation, either version 2 of the License, or                   //
//          (at your option) any later version.                                                 //
//                                                                                              //
//          mosquitto-security is distributed in the hope that it will be useful,               //
//          but WITHOUT ANY WARRANTY; without even the implied warranty of                      //
//          MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                       //
//          GNU General Public License for more details.                                        //
//                                                                                              //
//          You should have received a copy of the GNU General Public License                   //
//          along with mosquitto-security. If not, see <http://www.gnu.org/licenses/>           //
//          or write to the Free Software Foundation, Inc., 51 Franklin Street,                 //
//          51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.                       //
//                                                                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////

/*!
* \file Log.h
* \author Boubacar DIENE
*/

#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------------------------- */
/* ////////////////////////////////////////// HEADERS ///////////////////////////////////////// */
/* -------------------------------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>

/* -------------------------------------------------------------------------------------------- */
/* ////////////////////////////////////////// MACROS ////////////////////////////////////////// */
/* -------------------------------------------------------------------------------------------- */

// Unsafe logging method
#define BLUE    "\033[0;34m"
#define YELLOW  "\033[1;33m"
#define RED     "\033[5;31m"
#define GREEN   "\033[0;32m"

#define END     "\033[0m"

#define __file__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define printColor(color) \
   printf("%s%s : %s:%d (%s) %s", color, TAG, __file__, __LINE__, __func__, END)

#define Logd(...)(printColor(BLUE),printf(__VA_ARGS__),printf("\n"))
#define Logi(...)(printColor(GREEN),printf(__VA_ARGS__),printf("\n"))
#define Logw(...)(printColor(YELLOW),printf(__VA_ARGS__),printf("\n"))
#define Loge(...)(printColor(RED),printf(__VA_ARGS__),printf("\n"))

// Async-signal-safe logging method
// Note: -Wno-unused-result has been added to cflags because
//       gcc warn even if (void) is prepended to write
#define Logd_safe(...)(write(STDOUT_FILENO,__VA_ARGS__))
#define Loge_safe(...)(write(STDERR_FILENO,__VA_ARGS__))

#ifdef __cplusplus
}
#endif

#endif //__LOG_H__
