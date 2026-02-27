/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 14:47:48 by sliziard          #+#    #+#             */
/*   Updated: 2026/02/27 19:37:23 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PMERGEME_HPP__
# define __PMERGEME_HPP__

# include <algorithm>
# include <ctime>
# include <functional>
# include <memory>
# include <utility>
# include <stdint.h>


// ============================================================================
// Declarations
// ============================================================================

template<
	template<class, class> class Cont,
	typename T,
	typename Alloc,
	typename Compare
>
Cont<T, Alloc>	fordJohnsonSort(const Cont<T, Alloc> &seq);

// ============================================================================
// 1- Make Pairs
// ============================================================================

template<template<class, class> class Cont, typename T >
struct PairsResult
{
	typedef std::pair<T, T>					t_pair;
	typedef std::allocator<t_pair>			t_alloc;
	typedef Cont<t_pair, t_alloc>			t_pairCont;
	typedef typename t_pairCont::size_type	t_size;

	t_pairCont	pairs;
	bool		hasStraggler;
	T			straggler;

	PairsResult(): pairs(), hasStraggler(false), straggler() {}

	void	push_back(T min, T max)
	{
		pairs.push_back(std::make_pair(min, max));
	}
};

template<
	template<class, class> class Cont,
	typename T,
	typename Alloc,
	typename Compare
>
PairsResult<Cont, T>	makePairs(const Cont<T, Alloc> &c)
{
	PairsResult<Cont, T>				r;
	Compare								comp;
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
// 2- Sort Pairs on big value
// ============================================================================

template <typename T>
struct ComparePairByBig
{
	bool operator()(const std::pair<T, T> &l, const std::pair<T, T> &r) const
	{
		return l.second < r.second;
	}
};

template<
	template<class, class> class Cont,
	typename T,
	typename Alloc
>
void	sortPairsOnBig(Cont<std::pair<T, T>, Alloc> &pairs)
{
	typedef Cont<std::pair<T,T>, Alloc>	PairCont;
	typename PairCont::size_type	n = pairs.size();

	if (n <= 1)
		return;
	if (n == 2)
	{
		if (pairs[0].second > pairs[1].second)
			std::swap(pairs[0], pairs[1]);
		return;
	}
	std::sort(pairs.begin(), pairs.end(), ComparePairByBig<T>());
	// ? Sadly can't do that cause compile recursion is not possible
	// pairs = fordJohnsonSort<
	// 			Cont,
	// 			std::pair<T, T>,
	// 			Alloc,
	// 			ComparePairByBig<T>
	// 		>(pairs);
}

// ============================================================================
// 3- Insert pends chain
// ============================================================================

template<
	template<class, class> class Cont,
	typename T,
	typename Alloc,
	typename Compare
>
void	insertPendsChain(
	Cont<T, Alloc> &mainChain,
	const PairsResult<Cont, T> &r
)
{
	Compare	comp;

	for (typename PairsResult<Cont, T>::t_size i = 0; i < r.pairs.size(); ++i)
	{
		const T	&small = r.pairs[i].first;
		const T &big = r.pairs[i].second;

		typename Cont<T, Alloc>::iterator	bound = std::lower_bound(
															mainChain.begin(),
															mainChain.end(),
															big, comp);
		typename Cont<T, Alloc>::iterator	pos = std::lower_bound(
															mainChain.begin(),
															bound,
															small, comp);
		mainChain.insert(pos, small);
	}
	if (r.hasStraggler)
	{
		typename Cont<T, Alloc>::iterator	pos =
			std::lower_bound(mainChain.begin(), mainChain.end(), r.straggler, comp);
		mainChain.insert(pos, r.straggler);
	}
}

// ============================================================================
// Main sort function
// ============================================================================

template<
	template<class, class> class Cont,
	typename T,
	typename Alloc,
	typename Compare
>
Cont<T, Alloc>	fordJohnsonSort(const Cont<T, Alloc> &seq)
{
	PairsResult<Cont, T>	r = makePairs<Cont, T, Alloc, Compare>(seq);
	sortPairsOnBig<
			Cont,
			T,
			typename PairsResult<Cont, T>::t_alloc
		>(r.pairs);
	Cont<T, Alloc>			main;

	for (typename PairsResult<Cont, T>::t_size i = 0; i < r.pairs.size(); ++i)
		main.push_back(r.pairs[i].second);

	insertPendsChain<Cont, T, Alloc, Compare>(main, r);
	return main;
}

template<
	template<class, class> class Cont,
	typename T
>
Cont<T, std::allocator<T> > fordJohnsonSort(const Cont<T, std::allocator<T> > &seq)
{
	return fordJohnsonSort<
		Cont,
		T,
		std::allocator<T>,
		std::less<T>
	>(seq);
}

// ============================================================================
// Time utils
// ============================================================================

template<
	template<class, class> class Cont,
	typename T
>
double	timedFJSort(
			const Cont<T, std::allocator<T> > &src,
			Cont<T, std::allocator<T> > &dest)
{
	std::clock_t	start = std::clock();
	dest = fordJohnsonSort<Cont, T>(src);
	std::clock_t	end = std::clock();

	return static_cast<double>(end - start) / CLOCKS_PER_SEC * 1e6;
}

#endif /* __PMERGEME_HPP__ */
