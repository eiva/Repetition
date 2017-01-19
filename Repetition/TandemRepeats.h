#pragma once
/* ===========================================================================
*
*                            PUBLIC DOMAIN NOTICE
*               National Center for Biotechnology Information
*
*  This software/database is a "United States Government Work" under the
*  terms of the United States Copyright Act.  It was written as part of
*  the author's official duties as a United States Government employee and
*  thus cannot be copyrighted.  This software/database is freely available
*  to the public for use. The National Library of Medicine and the U.S.
*  Government have not placed any restriction on its use or reproduction.
*
*  Although all reasonable efforts have been taken to ensure the accuracy
*  and reliability of the software and data, the NLM and the U.S.
*  Government do not and cannot warrant the performance or results that
*  may be obtained by using this software or data. The NLM and the U.S.
*  Government disclaim all warranties, express or implied, including
*  warranties of performance, merchantability or fitness for any particular
*  purpose.
*
*  Please cite the author in any work or product based on this material.
*
* ===========================================================================
*/

#include <string>
#include <list>

/// A primitive tandem repeat found in a string S
///
/// A tandem repeat is a string that can be written as BBB..B
/// num_repeats times for some string non-empty string B. If B is not
/// itself a tandem repeat, then it is a primitive tandem repeat.
///
/// A maximal tandem repeat is one which lies in another string and for
/// which there are no additional complete copies of B before or after.
///
/// Example:
///
/// The string "na na na na na na na na hey hey hey goodbye"
///
/// has a tandem repeat "na na na na na na na na ". If you choose "na
/// na " as the unit then the repeat is not primitive because "na na
/// " is itself a tandem repeat. However with repeating unit "na ", we
/// define a primitive tandem repeat. The maximal primitive tandem
/// repeats are {0, "na ", 8} and {24, "hey ", 3}
struct STandemRepeat
{
	STandemRepeat(std::string::size_type s, size_t p, size_t e)
		: start(s)
		, repeat_len(p)
		, num_repeats(e)
	{}

	/// The total extent of the string covered by the repeats.
	std::string::size_type span() const { return repeat_len * num_repeats; }

	/// For sorting. Needed to store in a set.
	bool operator <(const STandemRepeat& o) const
	{
		return std::make_tuple(start, span()) <
			std::make_tuple(o.start, o.span());
	}

	/// For comparison. Needed in unit tests.
	bool operator ==(const STandemRepeat& o) const
	{
		return std::make_tuple(start, repeat_len, num_repeats) ==
			std::make_tuple(o.start, o.repeat_len, o.num_repeats);
	}

	/// The (0-based) index in S of the first character in the
	/// repeated string. Must be less than length of S
	std::string::size_type start;

	/// The primitive repeated sequence B - cannot be empty
	///
	/// Though the data-structure does not validate this, B must not
	/// itself be a tandem repeat.
	size_t repeat_len;

	/// The number of times B repeats - must be 2 or more
	size_t num_repeats;
};

/// Return all maximal primitive repeats in \a s
///
/// The current implementation uses Crochemore's algorithm to find all
/// the maximal primitive tandem repeats in \a s. Asymptotic
/// worst-case time complexity is O(n log n) where n is the length of
/// \a s, which is optimal.
///
/// This returns all STRs for one precise definition of STR.
///
/// M. Crochemore: An optimal algorithm for computing the repetitions
/// in a word, Information Processing Letters, Vol 12 Num 5, 13 October
/// 1981, pp. 244-250
///
/// Example:
///
/// g_MaximalPrimitiveTandemRepeats("mississippi") will return the set
/// {{1,"iss",2}, {2,"s",2), {2,"ssi",2}, {5,"s",2}, {8,"p",2}}
std::list<STandemRepeat> g_MaximalPrimitiveTandemRepeats(const std::string& s);


