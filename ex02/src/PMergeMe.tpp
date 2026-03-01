/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PMergeMe.tpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/01 13:43:33 by sliziard          #+#    #+#             */
/*   Updated: 2026/03/01 22:03:40 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PMERGEME_TPP__
# define __PMERGEME_TPP__

# include <algorithm>
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
	if (seq.size() == 2)
	{
		Cont<T, Alloc>	cpy(seq);
		if (comp(cpy[1], cpy[0]))
			std::swap(cpy[1], cpy[0]);
		return cpy;
	}

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
		pos = std::lower_bound(main.begin(), main.end(), r.straggler, comp);
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
	return fordJohnsonSort<
		Cont,
		T,
		std::allocator<T>,
		std::less<T>
	>(seq);
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

	main.resize(sortedIdx.size());
	for (size_t i = 0; i < sortedIdx.size(); ++i)
	{
		main[i] = r.pairs[sortedIdx[i]].second;
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

	Cont<size_t, t_idxalloc>	t;
	t.push_back(1);

	size_t	k = 2;
	while (true)
	{
		size_t tk = (1ULL << k) - jacobsthal(k);
		if (tk > m)
			break;
		t.push_back(tk);
		++k;
	}

	size_t	prev = 0;
	for (size_t i = 0; i < t.size(); ++i)
	{
		size_t	upper = std::min(t[i], m);
		for (size_t j = upper; j > prev; --j)
			order.push_back(j - 1);
		prev = upper;
	}
	for (size_t j = m; j > prev; --j)
		order.push_back(j - 1);
	return order;
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
	Cont<size_t, t_idxalloc>	order(_genInsertOrder<Cont>(pairs.size()));
	Cont<size_t, t_idxalloc>	posOfPair;

	posOfPair.resize(sortedIdx.size());
	for (size_t i = 0; i < sortedIdx.size(); ++i)
		posOfPair[sortedIdx[i]] = i;

	for (size_t i = 0; i < order.size(); ++i)
	{
		size_t	pairIdx = order[i];
		const T	&lo = pairs[pairIdx].first;

		typedef typename Cont<T, Alloc>::iterator	t_contIt;
		size_t		bigPos = posOfPair[pairIdx];
		t_contIt	insertPos = std::lower_bound(
			mainChain.begin(), mainChain.begin() + bigPos, lo, comp
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