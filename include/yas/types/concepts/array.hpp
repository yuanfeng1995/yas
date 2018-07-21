
// Copyright (c) 2010-2018 niXman (i dot nixman dog gmail dot com). All
// rights reserved.
//
// This file is part of YAS(https://github.com/niXman/yas) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef __yas__types__concepts__array_hpp
#define __yas__types__concepts__array_hpp

namespace yas {
namespace detail {
namespace concepts {
namespace array {

/***************************************************************************/

template<std::size_t F, typename Archive, typename C>
Archive& save(Archive &ar, const C &c) {
    if ( F & yas::json ) {
        ar.write("[", 1);

        if ( !c.empty() ) {
            auto it = c.begin();
            ar & (*it);
            for ( ++it; it != c.end(); ++it ) {
                ar.write(",", 1);
                ar & (*it);
            }
        }

        ar.write("]", 1);
    } else {
        const auto size = c.size();
        ar.write_seq_size(size);
        if ( size ) {
            if ( can_be_processed_as_byte_array<F, typename C::value_type>::value ) {
                ar.write(&c[0], sizeof(typename C::value_type) * size);
            } else {
                for ( const auto &it: c ) {
                    ar & it;
                }
            }
        }
    }

    return ar;
}

/***************************************************************************/

template<std::size_t F, typename Archive, typename C>
Archive& load(Archive &ar, C &c) {
    if ( F & yas::json ) {
        __YAS_CONSTEXPR_IF ( !(F & yas::compacted) ) {
            json_skipws(ar);
        }

        __YAS_THROW_IF_BAD_JSON_CHARS(ar, "[");

        __YAS_CONSTEXPR_IF ( !(F & yas::compacted) ) {
            json_skipws(ar);
        }

        const char ch = ar.peekch();
        if ( ch == ']' ) {
            ar.getch();

            return ar;
        }

        __YAS_CONSTEXPR_IF ( !(F & yas::compacted) ) {
            json_skipws(ar);
        }

        while ( true ) {
            typename C::value_type v = typename C::value_type{};
            ar & v;
            c.push_back(std::move(v));

            __YAS_CONSTEXPR_IF ( !(F & yas::compacted) ) {
                json_skipws(ar);
            }

            const char ch2 = ar.peekch();
            if ( ch2 == ']' ) {
                break;
            } else {
                __YAS_THROW_IF_BAD_JSON_CHARS(ar, ",");
            }

            __YAS_CONSTEXPR_IF ( !(F & yas::compacted) ) {
                json_skipws(ar);
            }
        }

        json_skipws(ar);
        __YAS_THROW_IF_BAD_JSON_CHARS(ar, "]");
    } else {
        const auto size = ar.read_seq_size();
        if ( size ) {
            c.resize(size);
            if ( can_be_processed_as_byte_array<F, typename C::value_type>::value ) {
                ar.read(&c[0], sizeof(typename C::value_type) * size);
            } else {
                for ( auto &it: c ) {
                    ar & it;
                }
            }
        }
    }

    return ar;
}

/***************************************************************************/

} // ns array
} // ns concepts
} // ns detail
} // ns yas

#endif // __yas__types__concepts__array_hpp
