/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PMergeMe.tpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 13:43:33 by sliziard          #+#    #+#             */
/*   Updated: 2026/03/03 14:09:52 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PMERGEME_TPP__
# define __PMERGEME_TPP__

# include <cassert>
# include <cstddef>
# include <functional>
# include <iterator>

# ifndef __PMERGEME_HPP__
#  error __FILE__ " sould only be included from PMergeMe.hpp"
#  include "PMergeMe.hpp" // ? for LSP purpose
# endif

// ============================================================================
// Main sort functions
// ============================================================================

template<
	template<class, class> class Cont,
	typename T,
	typename Alloc,
	typename Compare
>
Cont<T, Alloc>	fordJohnsonSort(const Cont<T, Alloc> &seq, const Compare &comp)
{
	if (seq.size() <= 1)
		return seq;

	PairsResult<Cont, T>		r(
		makePairs<Cont, T, Alloc, Compare>(seq, comp)
	);
	Cont<size_t, t_idxalloc>	sortedIdx;
	Cont<T, Alloc>				main(
		createMainChain<Cont, T, Alloc, Compare>(r, sortedIdx, comp)
	);

	insertPendsChain<Cont, T, Alloc, Compare>(main, r.pairs, sortedIdx, comp);
	if (r.hasStraggler)
	{
		typename Cont<T, Alloc>::iterator	pos;
		pos = binarySearch(main.begin(), main.end(), r.straggler, comp);
		main.insert(pos, r.straggler);
	}
	return main;
}

template<
	template<class, class> class Cont,
	typename T
>
Cont<T, std::allocator<T> >	fordJohnsonSort(
								const Cont<T, std::allocator<T> > &seq
							)
{
	size_t comparisonCount = 0;
	CountingCompare<T, std::less<T> >
		countingComp(std::less<T>(), comparisonCount);

	Cont<T, std::allocator<T> >	res = fordJohnsonSort<
		Cont,
		T,
		std::allocator<T>,
										CountingCompare<T, std::less<T> >
									>(seq, countingComp);
	
	std::cout << "Comparisons: " << comparisonCount << std::endl;
	return res;
}

// ============================================================================
// Pairing
// ============================================================================

template<
	template<class, class> class Cont,
	typename T,
	typename Alloc,
	typename Compare
>
PairsResult<Cont, T>	makePairs(
							const Cont<T, Alloc> &c,
							const Compare &comp
						)
{
	PairsResult<Cont, T>				r;
	typename Cont<T, Alloc>::size_type	size = c.size();

	for (typename Cont<T, Alloc>::size_type i = 0; i + 1 < size; i += 2)
	{
		T	a = c[i];
		T	b = c[i + 1];
		if (comp(a, b))
			r.push_back(a, b);
		else
			r.push_back(b, a);
	}

	if (size % 2)
	{
		r.hasStraggler = true;
		r.straggler = c[size - 1];
	}

	return r;
}

// ============================================================================
// Main Chain creation
// ============================================================================

template<template<class, class> class Cont>
static inline Cont<size_t, t_idxalloc>	_genIndexSeq(size_t n)
{
	Cont<size_t, t_idxalloc>		seq;

	seq.resize(n);
	for (size_t i = 0; i < n; ++i)
		seq[i] = i;
	return seq;
}

template<
	template<class, class> class Cont,
	typename T,
	typename Alloc,
	typename Compare
>
Cont<T, Alloc>	createMainChain(
	const PairsResult<Cont, T> &r,
	Cont<size_t, t_idxalloc> &sortedIdx,
	const Compare &comp
)
{
	typedef std::allocator<size_t>	t_idxalloc;
	Cont<size_t, t_idxalloc>		index(_genIndexSeq<Cont>(r.pairs.size()));
	Cont<T, Alloc>					main;

	CompareIndex<Cont, T>			idxComp(r.pairs, &cmp_thunk<T, Compare>, &comp);
	
	sortedIdx = fordJohnsonSort<
				Cont,
				size_t,
				t_idxalloc,
				CompareIndex<Cont, T>
			>(index, idxComp);
	main.resize(sortedIdx.size() + 1);

	main[0] = r.pairs[sortedIdx[0]].first;
	for (size_t i = 0; i < sortedIdx.size(); ++i)
	{
		main[i + 1] = r.pairs[sortedIdx[i]].second;
	}
	return main;
}

// ============================================================================
// Insertion
// ============================================================================

template<template<class, class> class Cont>
static Cont<size_t, t_idxalloc >	_genInsertOrder(size_t m)
{
	Cont<size_t, t_idxalloc>	order;

	if (m == 0)
		return order;

	Cont<size_t, t_idxalloc>	jacob;
	size_t	k = 3;
	while (true)
	{
		size_t j = jacobsthal(k);
		if (j > m)
			break;
		jacob.push_back(j);
		++k;
	}

	size_t	prev = 0;
	for (size_t i = 0; i < jacob.size(); ++i)
	{
		size_t	upper = jacob[i];
		for (size_t x = upper; x > prev; --x)
			order.push_back(x - 1);
		prev = upper;
	}
	for (size_t x = m; x > prev; --x)
		order.push_back(x - 1);

	// std::cout << "Insert order: ";
	// for (size_t i = 0; i < order.size(); ++i)
	// 	std::cout << order[i] << " ";
	// std::cout << std::endl;
	
	return order;
}

template<typename Iterator, typename T, typename Compare>
Iterator	binarySearch(
	Iterator first, Iterator last,
	const T& value,
	const Compare &comp
)
{
	while (first < last)
	{
		Iterator mid = first + (last - first) / 2;

		if (comp(*mid, value))
			first = mid + 1;
		else
			last = mid;
	}
	return first;
}

template<
	template<class, class> class Cont,
	typename T,
	typename Alloc,
	typename Compare
>
void	insertPendsChain(
			Cont<T, Alloc> &mainChain,
			const Cont<
				std::pair<T, T>, std::allocator<std::pair<T, T> >
			> &pairs,
			const Cont<size_t, t_idxalloc> &sortedIdx,
			const Compare &comp
		)
{
	Cont<size_t, t_idxalloc>	order(_genInsertOrder<Cont>(pairs.size() - 1));
	Cont<size_t, t_idxalloc>	posOfPair;

	posOfPair.resize(sortedIdx.size());
	for (size_t i = 0; i < sortedIdx.size(); ++i)
		posOfPair[sortedIdx[i]] = i + 1;

	for (size_t i = 0; i < order.size(); ++i)
	{
		size_t	pairIdx = sortedIdx[order[i] + 1];
		const T	&lo = pairs[pairIdx].first;
		size_t	bigPos = posOfPair[pairIdx];

		typedef typename Cont<T, Alloc>::iterator	t_contIt;
		t_contIt	insertPos = binarySearch(
			mainChain.begin(), mainChain.begin() + bigPos - 1, lo, comp
		);
		size_t		insertIdx = static_cast<size_t>(
			std::distance(mainChain.begin(), insertPos)
		);

		mainChain.insert(insertPos, lo);
		
		for (size_t j = 0; j < posOfPair.size(); ++j)
		{
			if (posOfPair[j] >= insertIdx)
				++posOfPair[j];
		}
	}
}

#endif /* __PMERGEME_TPP__ */