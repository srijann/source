#ifndef __STD_UTILITY__
#define __STD_UTILITY__

/***************************************************************************
 *
 * utility - Declarations for the Standard Library utility classes
 *
 * $Id: utility,v 1.15 1995/08/15 00:07:49 smithey Exp $
 *
 ***************************************************************************
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 ***************************************************************************
 *
 * (c) Copyright 1994, 1995 Rogue Wave Software, Inc.
 * ALL RIGHTS RESERVED
 *
 * The software and information contained herein are proprietary to, and
 * comprise valuable trade secrets of, Rogue Wave Software, Inc., which
 * intends to preserve as trade secrets such software and information.
 * This software is furnished pursuant to a written license agreement and
 * may be used, copied, transmitted, and stored only in accordance with
 * the terms of such license and with the inclusion of the above copyright
 * notice.  This software and information or any other copies thereof may
 * not be provided or otherwise made available to any other person.
 *
 * Notwithstanding any other lease or license that may pertain to, or
 * accompany the delivery of, this computer software and information, the
 * rights of the Government regarding its use, reproduction and disclosure
 * are as set forth in Section 52.227-19 of the FARS Computer
 * Software-Restricted Rights clause.
 * 
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 * Contractor/Manufacturer is Rogue Wave Software, Inc.,
 * P.O. Box 2328, Corvallis, Oregon 97339.
 *
 * This computer software and information is distributed with "restricted
 * rights."  Use, duplication or disclosure is subject to restrictions as
 * set forth in NASA FAR SUP 18-52.227-79 (April 1985) "Commercial
 * Computer Software-Restricted Rights (April 1985)."  If the Clause at
 * 18-52.227-74 "Rights in Data General" is specified in the contract,
 * then the "Alternate III" clause applies.
 *
 **************************************************************************/

#include <stdcomp.h>

#ifndef RWSTD_NO_NAMESPACE
namespace std {
#endif

//
// Operators
//

template <class T>
inline bool operator!=(const T& x, const T& y)
{
    return !(x == y);
}

template <class T>
inline bool operator>(const T& x, const T& y)
{
    return y < x;
}

template <class T>
inline bool operator<=(const T& x, const T& y)
{
    return !(y < x);
}

template <class T>
inline bool operator>=(const T& x, const T& y)
{
    return !(x < y);
}

//
// Pairs.
//

template <class T1, class T2>
struct pair
{
    T1 first;
    T2 second;
    pair (const T1& a, const T2& b) : first(a), second(b) {}
#ifdef RWSTD_NO_MEMBER_WO_DEF_CTOR
    pair () 
#ifndef RWSTD_NO_BUILT_IN_CTOR
: first(T1()), second(T2()) 
#endif
    { ; }
#endif
    ~pair () {;} // To avoid xlC warnings when destroy is called.
};

template <class T1, class T2>
inline bool operator==(const pair<T1, T2>& x, const pair<T1, T2>& y)
{ 
    return x.first == y.first && x.second == y.second; 
}

template <class T1, class T2>
inline bool operator<(const pair<T1, T2>& x, const pair<T1, T2>& y)
{ 
    return x.first < y.first || (!(y.first < x.first) && x.second < y.second); 
}

template <class T1, class T2>
inline pair<T1, T2> make_pair(const T1& x, const T2& y)
{
    return pair<T1, T2>(x, y);
}

#ifndef RWSTD_NO_NAMESPACE
}
#endif

#endif /*__STD_UTILITY__*/
