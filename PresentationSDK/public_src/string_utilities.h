//---------------------------------------------------------------------------
#ifndef string_utilitiesH
#define string_utilitiesH
//---------------------------------------------------------------------------
#pragma once
#include "types.h"
#include <string>
#include <algorithm>
#include <utility>
#include <iostream>
#include <stdlib.h>
//---------------------------------------------------------------------------

namespace public_nbs {

std::wstring ascii_to_wstring( const std::string& ascii_string );

template <class T, class Char_type>
T string_to_number( const Char_type *nptr, Char_type **endptr )
{
    nptr->blah;
}

template <>
inline double string_to_number<double,char>( const char *nptr, char **endptr )
{
    return ::strtod( nptr, endptr );
}

template <>
inline double string_to_number<double,wchar_t>( const wchar_t *nptr, wchar_t **endptr )
{
    return ::wcstod( nptr, endptr );
}

template <>
inline unsigned long string_to_number<unsigned long,char>( const char *nptr, char **endptr )
{
    return ::strtoul( nptr, endptr, 0 );
}

template <>
inline unsigned long string_to_number<unsigned long,wchar_t>( const wchar_t *nptr, wchar_t **endptr )
{
    return ::wcstoul( nptr, endptr, 0 );
}

template <>
inline long string_to_number<long,char>( const char* nptr, char **endptr )
{
    return ::strtol( nptr, endptr, 0 );
}

template <>
inline long string_to_number<long,wchar_t>( const wchar_t* nptr, wchar_t **endptr )
{
    return ::wcstol( nptr, endptr, 0 );
}

template <class T, class Char_type>
T to_number_def( const Char_type* s, T def )
{
    Char_type* endptr;
    T result = string_to_number<T,Char_type>( s, &endptr );
    if ( (!*s) || (*endptr) ) result = def;

    return result;
}

/// \brief Converts an integer string into an integer
///
/// \param s Integer string, may contain leading whitespace but
/// may not contain trailing whitespace
///
/// \param def Value that is returned if string is not valid
///
/// \return Integer value of the string or 'def' 
template <class Char_type>
long to_int_def( const Char_type* s, long def )
{
    return to_number_def<long>( s, def );
}

inline long to_int_def( const wchar_t* s, long def )
{
    return to_int_def<wchar_t>( s, def );
}

/// \brief Converts an unsigned integer string into an integer
///
/// \param s Unsigned integer string, may contain leading whitespace but
/// may not contain trailing whitespace
///
/// \param def Value that is returned if string is not valid
///
/// \return Integer value of the string or 'def' 
template <class Char_type>
unsigned long to_ul_def( const Char_type* s, unsigned long def )
{
    return to_number_def<unsigned long>( s, def );
}

inline unsigned long to_ul_def( const wchar_t* s, unsigned long def )
{
    return to_ul_def<wchar_t>( s, def );
}

/// \brief Converts a double string into an double
///
/// \param s Double string, may contain leading whitespace but
/// may not contain trailing whitespace
///
/// \param def Value that is returned if string is not valid
///
/// \return Double value of the string or 'def' 
template <class Char_type>
double to_double_def( const Char_type* s, double def )
{
    return to_number_def<double>( s, def );
}

inline double to_double_def( const wchar_t* s, double def )
{
    return to_double_def<wchar_t>( s, def );
}

template <class T, class Char_type>
bool is_number( const Char_type* s )
{
    Char_type* endptr;
    string_to_number<T,Char_type>( s, &endptr );
    return *s && !*endptr;
}

/// \brief Does the string contain a representation of a signed integer
///
/// The string may contain leading whitespace, but not trailing whitespace
template <class Char_type>
bool is_int( const Char_type* s )
{
    return is_number<long>( s );
}

inline bool is_int( const wchar_t* s )
{
    return is_int<wchar_t>( s );
}

/// \brief Does the string contain a representation of an unsigned integer
///
/// The string may contain leading whitespace, but not trailing whitespace
template <class Char_type>
bool is_ul( const Char_type* s )
{
    return is_number<unsigned long>( s );
}

inline bool is_ul( const wchar_t* s )
{
    return is_ul<wchar_t>( s );
}

/// \brief Does the string contain a representation of a double
///
/// The string may contain leading whitespace, but not trailing whitespace
template <class Char_type>
bool is_double( const Char_type* s )
{
    return is_number<double>( s );
}

inline bool is_double( const wchar_t* s )
{
    return is_double<wchar_t>( s );
}

template <class Char_type>
int isspace( Char_type c )
{
    return ::iswspace( c );
}

template <>
inline int isspace<char>( char c )
{
    return ::isspace( c );
}

template <>
inline int isspace<unsigned char>( unsigned char c )
{
    return ::isspace( c );
}

/*
template <>
inline int isspace<wchar_t>( wchar_t c )
{
    return ::iswspace( c );
}
*/

/// \brief Removes leading and trailing whitespace
template <class String_type>
String_type trim( const String_type& s )
{
    String_type result;

    typename String_type::const_iterator i = s.begin();
    typename String_type::const_iterator e = s.end();
    while ( (i != e) && (isspace<String_type::value_type>( *i )) ) ++i;
    if (i != e) 
    {
        --e;
        while ( (e != i) && (isspace<String_type::value_type>( *e )) ) --e;   
        result = s.substr( i - s.begin(), e - i + 1 );
    }

    return result;
}

template <class Char_type>
std::basic_string<Char_type> trim( const Char_type* s )
{
    return trim<std::basic_string<Char_type> >( s );
}

/// \brief Converts a string to lower case
///
/// \param s String to be changed
///
/// \return Reference to the string argument
///
/// This function simply replaces A-Z with a-z
template <class String_class>
String_class& to_lower( String_class& s )
{
    typename String_class::iterator i = s.begin();
    typename String_class::iterator e = s.end();
    for (; i != e; ++i)
    {
        if ( (*i >= 'A') && (*i <= 'Z') )
        {
            *i = 'a' + (*i - 'A');
        }
    }
    return s;
}

/// \brief Converts a string to lower case
///
/// \param s Original string
///
/// \return New string
///
/// This function simply replaces A-Z with a-z
template <class String_class>
String_class get_lower( const String_class& s )
{
    String_class temp( s );
    return to_lower( temp );
}

template <class Char_type>
std::basic_string<Char_type> get_lower( const Char_type* s )
{
    return get_lower<std::basic_string<Char_type> >( s );
}

/// \brief Converts a string to upper case
///
/// \param s String to be changed
///
/// \return Reference to the string argument
///
/// This function simply replaces a-z with A-Z
template <class String_class>
String_class& to_upper( String_class& s )
{
    typename String_class::iterator i = s.begin();
    typename String_class::iterator e = s.end();
    for (; i != e; ++i)
    {
        if ( (*i >= 'a') && (*i <= 'z') )
        {
            *i = 'A' + (*i - 'a');
        }
    }
    return s;
}

/// \brief Converts a string to upper case
///
/// \param s Original string
///
/// \return New string
///
/// This function simply replaces a-z with A-Z
template <class String_class>
String_class get_upper( const String_class& s )
{
    String_class temp( s );
    return to_upper( temp );
}

template <class Char_type>
std::basic_string<Char_type> get_upper( const Char_type* s )
{
    return get_upper<std::basic_string<Char_type> >( s );
}

/// \brief replacement for std::getline
///
/// \note This is defined because std::getline does not work
/// in Borland.
///
/// reads data from stream and appends it to str until
/// either a new line character '\n' or the end of file
/// the newline character is removed from the stream but not placed in str
template <class CharType>
std::basic_istream<CharType>& get_line( 
    std::basic_istream<CharType>& stream,
    std::basic_string<CharType>& str )
{
    const unsigned int BUFFER_SIZE = 1024;
    CharType buffer[BUFFER_SIZE + 1];
    CharType* ptr = buffer;
    CharType* end = buffer + BUFFER_SIZE;
    while (stream.get( *ptr ))
    {
        if (*ptr != '\n')
        {
            ++ptr;
            if (ptr == end)
            {
                *ptr = 0;
                str += buffer;
                ptr = buffer;
            }
        }
        else
        {
            break;
        }
    }
    *ptr = 0;
    str += buffer;

    return stream;
}

/// \brief Replaces "\\n" with "\n" and "\\t" with "\t"
template <class String_type>
String_type replace_escape( const String_type& s )
{
    String_type rval;
    rval.reserve( 2 * s.size() );
    std::back_insert_iterator<String_type> ins( std::back_inserter( rval ) );
    typename String_type::const_iterator i = s.begin();
    typename String_type::const_iterator e = s.end();
    while (i != e)
    {
        if (*i == L'\\')
        {
            ++i;
            if (i != e)
            {
                if (*i == L't') *ins = L'\t';
                else if (*i == L'n') *ins = L'\n';
                else *ins = *i;
                ++i;
            }
        }
        else
        {
            *ins = *i;
            ++i;
        }
    }

    return rval;
}         

/// \brief converts an int to a string
std::string to_str( int i );

/// \brief converts an int to a string
std::wstring to_wstr( int i );

/// \brief converts an unsigned int to a string
std::string to_str( unsigned int i );

/// \brief converts an unsigned int to a string
std::wstring to_wstr( unsigned int i );

/// \brief converts an unsigned long int to a string
std::string to_str( unsigned long int i );

/// \brief converts an unsigned long int to a string
std::wstring to_wstr( unsigned long int i );

/// \brief converts a sint64 to a string
std::string to_str( sint64 i );

/// \brief converts a sint64 to a string
std::wstring to_wstr( sint64 i );

/// \brief converts a double to a string
std::string to_str( double d );

/// \brief converts a double to a string
std::wstring to_wstr( double d );

/// \brief converts a uint64 to a string
std::string to_str( uint64 d );

/// \brief converts a uint64 to a string
std::wstring to_wstr( uint64 d );

/// \brief Returns hex string representation with at least width characters
std::wstring hex_wstr( uint32 i, unsigned int width = 0 );

/// \brief Copies characters from a std::wstring to a char string
///
/// \param source The original string
///
/// \param dest The new string, it will be resized
///
/// \return true if no data was lost
bool trunc_str( const std::wstring& source, std::string& dest );

/// \brief Number of characters in a zero terminated string
template <class Char_type>
unsigned int string_length( const Char_type* string )
{
    unsigned int rval = 0;
    while (*string)
    {
        ++rval;
        ++string;
    }
    return rval;
}

/// \brief Replaces one substring with another
///
/// \param str original string
///
/// \param oldstr substring to be replaced
///
/// \param newstr substring to replace with
///
/// \param result string into which the result is placed. If it's not
/// empty, it will be cleared first.
template <class String_type>
void replace( const String_type& str, 
    const typename String_type::value_type* oldstr, 
    const typename String_type::value_type* newstr, 
    String_type& result )
{
    result.clear();

    typename String_type::size_type oldlen = string_length( oldstr );
    typename String_type::size_type last_pos = 0;

    typename String_type::size_type pos = str.find( oldstr );
    while (pos != String_type::npos) 
    {
        result.append( str.c_str(), last_pos, pos - last_pos );
        result.append( newstr );
        last_pos = pos + oldlen;
        pos = str.find( oldstr, last_pos );
    }
    result.append( str.c_str(), last_pos, -1 );
}

/// \brief Replaces one substring with another
///
/// \param str string to be transformed
///
/// \param oldstr substring to be replaced
///
/// \param newstr substring to replace with
template <class String_type>
void replace( String_type& str, 
    const typename String_type::value_type* oldstr, 
    const typename String_type::value_type* newstr )
{
    String_type temp;
    replace( str, oldstr, newstr, temp );
    str.swap( temp );
}

/// \brief Find a subsequence within a larger sequence
///
/// \param first iterator to start of large sequence
///
/// \param last iterator to end of large sequence
///
/// \param first2 iterator to start of subsequence
///
/// \param last2 iterator to end of subsequence
///
/// \return pair of iterators identifying found subsequence
/// inside the large sequence, (last,last) if subsequence
/// is not fount, (last,last) if first2 == last2
template <class T1,class T2>
std::pair<T1,T1> find_section( T1 first, T1 last, T2 first2, T2 last2 ) 
{ 
   std::pair<T1,T1> result( last, last ); 
 
   if (first2 != last2)
   { 
      first = std::find( first, last, *first2 );
      while (first != last)
      {
         T1 temp = first;
         ++temp;
         T2 temp2 = first2;
         ++temp2;
         while ( (temp != last) && (temp2 != last2) && (*temp == *temp2) )
         { 
            temp++;
            temp2++;
         } 
         if (temp2 == last2)
         { 
            result.first = first;
            result.second = temp;
            break;
         }    
         ++first;
        first = std::find( first, last, *first2 );
      } 
   } 

   return result;
}

/// \brief Find a subsequence within a larger sequence
///
/// \param first iterator to start of large sequence
///
/// \param last iterator to end of large sequence
///
/// \param d container that contains the subsequence
///
/// \return pair of iterators identifying found subsequence
/// inside the large sequence, (last,last) if subsequence
/// is not fount, (last,last) if d is empty
template <class T1, class T2>
std::pair<T1,T1> find_section (T1 first, T1 last, T2 const& d)
{
    return find_section( first, last, d.begin(), d.end() );
}

/// \brief Find a subsequence within a larger sequence of characters
///
/// \param first iterator to start of large sequence
///
/// \param last iterator to end of large sequence
///
/// \param d pointer to a zero terminated character string
///
/// \return pair of iterators identifying found subsequence
/// inside the large sequence, (last,last) if subsequence
/// is not fount, (last,last) if d is empty
template <class T, class Char_type>
std::pair<T,T> find_section ( T first, T last, const Char_type* d )
{
    const Char_type* end = d + string_length( d );
    return find_section(first, last, d, end );
}

/// \brief Splits a sequence into sections
///
/// \param first iterator to start of sequence
///
/// \param last iterator to end of sequene
///
/// \param d delimiter value
///
/// \param c container into which section are placed
///
/// T objects should point to something comparable to a D,
/// and C objects should be containers containing objects that
/// contain sequences
template <class T, class D, class C>
void split( T first, T last, D const& d, C& c )
{
    T nxt = std::find(first, last, d);
    while (nxt != last)
   {
        c.push_back( C::value_type( first, nxt ) );
        first = ++nxt; 
        nxt = std::find(first, last, d);
    }    
   c.push_back(C::value_type(first, last));
}

/// \brief Splits a sequence into sections
///
/// \param first iterator to start of sequence
///
/// \param last iterator to end of sequene
///
/// \param d object containing delimiter sequence
///
/// \param c container into which section are placed
///
/// D type should support begin() and end() to define 
/// subsequence to look for
template <class T, class D, class C>
void split_big( T first, T last, D const& d, C& c )
{
    std::pair<T,T> nxt = find_section(first, last, d);
    while (nxt.first != last)
   {
        c.push_back(C::value_type(first, nxt.first));
        first = nxt.second;  
        nxt = find_section(first, last, d);
    }
   c.push_back(C::value_type(first, last));
}

/// \brief Splits a character sequence into sections
///
/// \param first iterator to start of sequence
///
/// \param last iterator to end of sequene
///
/// \param d pointer to zero terminated character string
///
/// \param c container into which section are placed
template <class T, class C, class Char_type>
void split_big( T first, T last, const Char_type* d, C& c )
{
    std::basic_string<Char_type> d2( d );
   split_big( first, last, d2, c );
}

}; // end namespace public_nbs

//---------------------------------------------------------------------------
#endif
