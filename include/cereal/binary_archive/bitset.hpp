/*
  Copyright (c) 2013, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of cereal nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_BINARY_ARCHIVE_BITSET_HPP_
#define CEREAL_BINARY_ARCHIVE_BITSET_HPP_

#include <cereal/cereal.hpp>
#include <bitset>

namespace cereal
{
  namespace bitset_detail
  {
    enum class type : uint8_t
    {
      ulong,
      ullong,
      string
    };

    template <class Archive> inline
    CEREAL_ARCHIVE_RESTRICT_SERIALIZE(BinaryInputArchive, BinaryOutputArchive)
    serialize( Archive & ar, type & t )
    {
      ar( reinterpret_cast<uint8_t &>( t ) );
    }
  }

  //! Serializing (save) for std::bitset to binary
  template <size_t N> inline
  void save( BinaryOutputArchive & ar, std::bitset<N> const & bits )
  {
    try
    {
      auto const b = bits.to_ulong();
      ar( bitset_detail::type::ulong );
      ar( b );
    }
    catch( std::overflow_error const & e )
    {
      try
      {
        auto const b = bits.to_ullong();
        ar( bitset_detail::type::ullong );
        ar( b );
      }
      catch( std::overflow_error const & e )
      {
        ar( bitset_detail::type::string );
        ar( bits.to_string() );
      }
    }
  }

  //! Serializing (load) for std::bitset to binary
  template <size_t N> inline
  void load( BinaryInputArchive & ar, std::bitset<N> & bits )
  {
    bitset_detail::type t;
    ar( t );

    switch( t )
    {
      case bitset_detail::type::ulong:
      {
        unsigned long b;
        ar( b );
        bits = std::bitset<N>( b );
        break;
      }
      case bitset_detail::type::ullong:
      {
        unsigned long long b;
        ar( b );
        bits = std::bitset<N>( b );
        break;
      }
      case bitset_detail::type::string:
      {
        std::string b;
        ar( b );
        bits = std::bitset<N>( b );
        break;
      }
      default:
        throw Exception("Invalid bitset data representation");
    }
  }
} // namespace cereal

#endif // CEREAL_BINARY_ARCHIVE_BITSET_HPP_
