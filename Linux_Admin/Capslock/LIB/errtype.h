/*****************************************************
 * File: errtype.h
 * Author: Mohamed Gamal
 * Description: Error type definitions
 *****************************************************/

#ifndef ERRTYPE_H_
#define ERRTYPE_H_


/* Enumeration of all system error codes */
typedef enum {
    E_OK = 0,             /* No error */
    E_NOT_OK,             /* General error */
    E_NULL_PTR,           /* Null pointer passed */
    E_OUT_OF_RANGE,       /* Value out of allowed range */
    E_BUSY,               /* Resource is busy */
    E_TIMEOUT,            /* Operation timed out */
    E_INVALID_PARAM,      /* Invalid input parameter */
    E_COMM_ERROR,         /* Communication failure */
    E_MEM_ERROR,          /* Memory allocation or access error */
    E_NOT_CHAR,		  /* This error indicates that the input is not character*/
    E_HW_FAILURE,         /* Hardware malfunction */
    E_UNSUPPORTED,        /* Operation not supported */
    E_NOT_FOUND,
    E_UNKNOWN_ERROR       /* Undefined or unknown error */
} ErrType_t;

#endif /* ERRTYPE_H_ */

