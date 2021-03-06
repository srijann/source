#ifndef __STD_FUNCTIONAL__
#define __STD_FUNCTIONAL__

/***************************************************************************
 *
 * functional - global template functions
 *
 * $Id: functional,v 1.22 1995/09/27 19:03:47 lijewski Exp $
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
// The bases of many of the function objects here.
//

template <class Arg, class Result>
struct unary_function
{
    typedef Arg argument_type;
    typedef Result result_type;
};

template <class Arg1, class Arg2, class Result>
struct binary_function
{
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Result result_type;
};

//
// Arithmetic operators.
//

template <class T>
struct plus : public binary_function<T, T, T>
{
    T operator() (const T& x, const T& y) const { return x + y; }
};

template <class T>
struct minus : public binary_function<T, T, T>
{
    T operator() (const T& x, const T& y) const { return x - y; }
};

template <class T>
struct times : public binary_function<T, T, T>
{
    T operator() (const T& x, const T& y) const { return x * y; }
};

template <class T>
struct divides : public binary_function<T, T, T>
{
    T operator() (const T& x, const T& y) const { return x / y; }
};

template <class T>
struct modulus : public binary_function<T, T, T>
{
    T operator() (const T& x, const T& y) const { return x % y; }
};

template <class T>
struct negate : public unary_function<T, T>
{
    T operator() (const T& x) const { return -x; }
};

//
// Comparisons.
//

template <class T>
struct equal_to : public binary_function<T, T, bool>
{
    bool operator() (const T& x, const T& y) const { return x == y; }
};

template <class T>
struct not_equal_to : public binary_function<T, T, bool>
{
    bool operator() (const T& x, const T& y) const { return x != y; }
};

template <class T>
struct greater : public binary_function<T, T, bool>
{
    bool operator() (const T& x, const T& y) const { return x > y; }
};

template <class T>
struct less : public binary_function<T, T, bool>
{
    bool operator() (const T& x, const T& y) const { return x < y; }
};

template <class T>
struct greater_equal : public binary_function<T, T, bool>
{
    bool operator() (const T& x, const T& y) const { return x >= y; }
};

template <class T>
struct less_equal : public binary_function<T, T, bool>
{
    bool operator() (const T& x, const T& y) const { return x <= y; }
};

//
// Logical operations.
//

template <class T>
struct logical_and : public binary_function<T, T, bool>
{
    bool operator() (const T& x, const T& y) const { return x && y; }
};

template <class T>
struct logical_or : public binary_function<T, T, bool>
{
    bool operator() (const T& x, const T& y) const { return x || y; }
};

template <class T>
struct logical_not : public unary_function<T, bool>
{
    bool operator() (const T& x) const { return !x; }
};

//
// Negators.
//

template <class Predicate>
class unary_negate : public unary_function<typename Predicate::argument_type,
                                           bool>
{
  protected:
    Predicate pred;
  public:
    explicit unary_negate (const Predicate& x) : pred(x) {}
    bool operator() (const argument_type& x) const { return !pred(x); }
};

template <class Predicate>
unary_negate<Predicate> not1(const Predicate& pred)
{
    return unary_negate<Predicate>(pred);
}

template <class Predicate>
class binary_negate
    : public binary_function<typename Predicate::first_argument_type,
                             typename Predicate::second_argument_type, bool>
{
  protected:
    Predicate pred;
  public:
    explicit binary_negate (const Predicate& x) : pred(x) {}
    bool operator() (const first_argument_type& x,
                     const second_argument_type& y) const
    {
        return !pred(x, y);
    }
};

template <class Predicate>
binary_negate<Predicate> not2(const Predicate& pred)
{
    return binary_negate<Predicate>(pred);
}

//
// Binders.
//

template <class Operation>
class binder1st :public unary_function<typename Operation::second_argument_type,
                                       typename Operation::result_type>
{
  protected:
    Operation op;
    typename Operation::first_argument_type value;
  public:
    binder1st (const Operation& x, const typename
                                   Operation::first_argument_type& y)
        : op(x), value(y) {}
    result_type operator() (const argument_type& x) const
    {
        return op(value, x);
    }
};

template <class Operation, class T>
binder1st<Operation> bind1st (const Operation& op, const T& x)
{
    return binder1st<Operation>(op, Operation::first_argument_type(x));
}

template <class Operation>
class binder2nd : public unary_function<typename Operation::first_argument_type,
                                        typename Operation::result_type>
{
  protected:
    Operation op;
    typename Operation::second_argument_type value;
  public:
    binder2nd (const Operation& x, const typename
                                   Operation::second_argument_type& y)
        : op(x), value(y) {}
    result_type operator() (const argument_type& x) const
    {
        return op(x, value);
    }
};

template <class Operation, class T>
binder2nd<Operation> bind2nd (const Operation& op, const T& x)
{
   return binder2nd<Operation>(op, Operation::second_argument_type(x));
}

template <class Operation1, class Operation2>
class unary_compose : public unary_function<typename Operation2::argument_type,
                                            typename Operation1::result_type>
{
  protected:
    Operation1 op1;
    Operation2 op2;
  public:
    unary_compose (const Operation1& x, const Operation2& y) : op1(x), op2(y) {}
    result_type operator() (const argument_type& x) const
    {
        return op1(op2(x));
    }
};

//
// Adaptors.
//

template <class Arg, class Result>
class pointer_to_unary_function : public unary_function<Arg, Result>
{
  protected:
    Result (*ptr)(Arg);
  public:
    explicit pointer_to_unary_function (Result (*x)(Arg)) : ptr(x) {}
    Result operator() (const Arg& x) const { return ptr(x); }
};

template <class Arg, class Result>
pointer_to_unary_function<Arg, Result> ptr_fun(Result (*x)(Arg))
{
    return pointer_to_unary_function<Arg, Result>(x);
}

template <class Arg1, class Arg2, class Result>
class pointer_to_binary_function : public binary_function<Arg1, Arg2, Result>
{
  protected:
    Result (*ptr)(Arg1, Arg2);
  public:
    explicit pointer_to_binary_function (Result (*x)(Arg1, Arg2)) : ptr(x) {}
    Result operator() (const Arg1& x, const Arg2& y) const
    {
        return ptr(x, y);
    }
};

template <class Arg1, class Arg2, class Result>
pointer_to_binary_function<Arg1, Arg2, Result>
ptr_fun(Result (*x)(Arg1, Arg2))
{
    return pointer_to_binary_function<Arg1, Arg2, Result>(x);
}

#ifndef RWSTD_NO_NAMESPACE
}
#endif

#endif /*__STD_FUNCTIONAL__*/

