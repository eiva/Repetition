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

#include "stdafx.h"
#include "TandemRepeats.h"

using namespace std;

namespace
{
	/// Search all maximal repeating units in given set of equality classes 
	/// for a given period (size of repeating unit).
	/// \example
	/// We have {{1,2,3,5,6}} in e
	/// for p = 1 we have (1,2,3) as a first repeating unit and (5,6) as second.
	/// for p = 2 we have (3,5) repeating unit.
	list<STandemRepeat> s_SelectRepeats(int p, const vector<list<int>>& e_class)
	{
		list<STandemRepeat> result;
		for (const auto& eClass : e_class) {
			if (eClass.size() <= 1) continue;

			auto start = eClass.begin(); // Start of probe
			auto current = start; // Current probing item.
			++current;
			int e = 1; // Number of repeats.
			while (current != eClass.end()) {
				if (*start + p*e != *current) {
					// Start of a new probe.
					if (e > 1) {
						result.emplace_back(*start, p, e);
					}
					e = 1;
					start = current;
					++current;
				}
				else {
					++current;
					++e;
				}
			}

			if (e > 1) { // Don't forget about tail.
				result.emplace_back(*start, p, e);
			}
		}
		return result;
	}

	/// Returns list of equality classes for initial period (=1).
	/// Each list is a set of equal characters.
	/// + for each character position in which class it located.
	tuple<vector<list<int>>, vector<int>> s_Initial(const string& str)
	{
		array<int, 256> pos; // Tracks first occurance of charecters. (for o(1) lookup)
		pos.fill(-1);
		map<int, list<int>> eClass;
		int index = 0;
		// Collect data.
		for (int i = 0; i <= (int)str.size(); ++i) { // \0 is also incuded.
			const auto xi = i == (int)str.size() ? 0 : str[i];
			if (pos[xi] < 0) {
				pos[xi] = index;
				++index;
			}
			eClass[pos[xi]].emplace_back(i);
		}
		// Format results.
		vector<list<int>> result;
		vector<int> indexes(str.size() + 1, -1);
		int i = 0;
		for (const auto& item : eClass) {
			result.push_back(item.second);
			for (const auto it : item.second) {
				// And building index.
				indexes[it] = i;
			}
			++i;
		}

		return make_tuple(result, indexes);
	}

	/// Refine equality classes.
	/// Rule: 
	/// For each item in equality classes we place it in a new subclasses based on where item+1 is located.
	/// \example
	/// Input: {{0, 1 5, 6}, {2, 3, 7, 8}, {4, 9}, {10}} // this is p=1 for "aabbcaabbc"
	/// We have following output:
	/// {{0, 5}, {1, 6}, {2, 7}, {3, 8}, {4}, {9}}
	vector<list<int>> s_Refine(const vector<list<int>>& e, vector<int>& index)
	{
		vector<list<int>> result;

		for (const auto& eClass : e) {
			if (eClass.size() <= 1) {
				// Classes with less than 2 elements are dropped on current level and not going to next level.
				continue;
			}
			vector<list<int>> splits(e.size(), list<int>()); // new set of subclasses.
			for (const auto eItem : eClass) {
				const auto next = index[eItem + 1];
				splits[next].push_back(eItem);
			}
			for (const auto& split : splits) {
				if (split.empty()) continue;
				result.push_back(split);
			}
		}

		// Rebuild index.
		for (int cls = 0; cls < (int)result.size(); ++cls) {
			for (const auto item : result[cls]) {
				index[item] = cls;
			}
		}
		return result;
	}
}

list<STandemRepeat> g_MaximalPrimitiveTandemRepeats(const string& str)
{
	if (str.size() <= 1) return{};

	list<STandemRepeat> result;

	vector<list<int>> eClass;
	vector<int> index; // Index are used for o(1) access to the class by item.
	tie(eClass, index) = s_Initial(str);

	int p = 1;
	while (!eClass.empty() && p <= (int)str.size() / 2) {
		auto p_res = s_SelectRepeats(p, eClass);
		result.splice(result.end(), p_res);
		eClass = s_Refine(eClass, index);
		++p;
	}
	return result;
}
