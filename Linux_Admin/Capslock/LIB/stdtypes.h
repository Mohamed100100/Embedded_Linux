/*****************************************************
 * File: stdtypes.h
 * Author: Mohamed Gamal
 * Description: Standard data type definitions
 *****************************************************/

#ifndef STDTYPES_H_
#define STDTYPES_H_

/* Unsigned integer types */
typedef unsigned char       	uint8;
typedef unsigned short int      uint16;
typedef unsigned int     	uint32;
typedef unsigned long long  	uint64;

/* Signed integer types */
typedef signed char         	sint8;
typedef signed short int        sint16;
typedef signed int              sint32;
typedef signed long long    	sint64;

/* Floating point types */
typedef float               float32;
typedef double              float64;

/* Boolean type */
typedef enum {
    FALSE = 0,
    TRUE  = 1
} bool_t;

/* Null pointer definition */
#ifndef NULL
#define NULL ((void*)0)
#endif

#endif /* STDTYPES_H_ */

