/*----------------------------------------------------------------------
   rds.h -  Velocis Client definitions.

   Copyright (c) 1992-1996 Raima Corporation.  All Rights Reserved.
----------------------------------------------------------------------*/

#ifndef RDS_H
#define RDS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * major modules
 */

#ifndef NO_NESTED_H
#define NO_NESTED_H
#define NO_NESTED_H_DEFINED
#endif

#include "catalog.h"
#include "rdm.h"     /* RDM definitions and prototypes */
#include "rdmproto.h"
#include "adm.h"     /* Administrative function definitions and prototypes */
#include "admproto.h"
#include "saproto.h" /* Server Access Library prototypes */
#include <stddef.h>
#include "rmcomp.h"
#include "rdserrs.h"

#ifdef NO_NESTED_H_DEFINED
/* If NO_NESTED_H was defined in this header file, clean it up now. */
#undef NO_NESTED_H
#undef NO_NESTED_H_DEFINED
#endif

#ifdef __cplusplus
}
#endif

#endif

/*
$Header:   P:/PAT/INCLUDE/VCS/RDS.H_V   1.8   07 Feb 1996 10:07:06   JUNZ  $
*/
