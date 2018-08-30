/*
 * Util.h
 *
 *  Created on: 2017. 9. 16.
 *      Author: user
 */

#ifndef UTIL_H_
#define UTIL_H_

#include  "Types.h"

// 함수 선언
void WriteLine( int iX, int iY, const char* pcString );
BOOL Initialize64KernelMemory( void );
BOOL CheckSystemMemory( void );
void CopyKernel64ImageTo2Mbyte( void );


#endif /* UTIL_H_ */
