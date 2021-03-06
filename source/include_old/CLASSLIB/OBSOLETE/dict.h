/*------------------------------------------------------------------------*/
/*                                                                        */
/*  DICT.H                                                                */
/*                                                                        */
/*  Copyright Borland International 1991, 1993                            */
/*  All Rights Reserved                                                   */
/*                                                                        */
/*------------------------------------------------------------------------*/

#if !defined( __DICT_H )
#define __DICT_H

#define BI_OLDNAMES

#if !defined( __STDLIB_H )
#include <stdlib.h>
#endif

#if !defined( __CLSTYPES_H )
#include "classlib\obsolete\ClsTypes.h"
#endif  // __CLSTYPES_H

#if !defined( __OBJECT_H )
#include "classlib\obsolete\Object.h"
#endif  // __OBJECT_H

#if !defined( __SET_H )
#include "classlib\obsolete\Set.h"
#endif  // __SET_H

#pragma option -Vo-
#if defined( __BCOPT__ ) && !defined( __FLAT__ ) && !defined( _ALLOW_po )
#pragma option -po-
#endif

_CLASSDEF(Association)
_CLASSDEF(Dictionary)

class _CLASSTYPE Dictionary : public Set
{

public:

    Dictionary( unsigned sz = DEFAULT_HASH_TABLE_SIZE ) :
        Set(sz)
        {
        }

    virtual void add( Object _FAR & );
    Association _FAR & lookup( const Object _FAR & ) const;

    virtual classType isA() const
        {
        return dictionaryClass;
        }

    virtual char _FAR *nameOf() const
        {
        return "Dictionary";
        }

};

#if defined( __BCOPT__ ) && !defined( __FLAT__ ) && !defined( _ALLOW_po )
#pragma option -po.
#endif
#pragma option -Vo.

#endif  // __DICT_H

