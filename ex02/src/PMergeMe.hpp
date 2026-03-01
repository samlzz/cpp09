/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PMergeMe.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 19:36:33 by sliziard          #+#    #+#             */
/*   Updated: 2026/03/03 14:09:44 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PMERGEME_HPP__
# define __PMERGEME_HPP__

# include <cstddef>
# include <ctime>
# include <iterator>
# include <memory>
# include <stdint.h>

// ============================================================================
// Main sort functions
// ============================================================================

// More generic one
template<
	template<class, class> class Cont,
	typename T,
	typename Alloc,
	typename Compare
>
Cont<T, Alloc>				fordJohnsonSort(
								const Cont<T, Alloc> &seq,
								const Compare &comp = Compare());

template<
	template<class, class> class Cont,
	typename T
>
Cont<T, std::allocator<T> >	fordJohnsonSort(
								const Cont<T, std::allocator<T> > &seq
							);

// ============================================================================
// Pairing
// ============================================================================

template<template<class, class> class Cont, typename T>
struct PairsResult
{
	typedef std::pair<T, T>					t_pair;

	Cont<t_pair, std::allocator<t_pair> >	pairs;
	bool									hasStraggler;
	T										straggler;

	PairsResult(): hasStraggler(false) {}

	void	push_back(T little, T big)
	{
		pairs.push_back(std::make_pair(little, big));
	}
};

template<
	template<class, class> class Cont,
	typename T,
	typename Alloc,
	typename Compare
>
PairsResult<Cont, T>	makePairs(const Cont<T, Alloc> &c, const Compare &comp);

// ============================================================================
// Main Chain creation
// ============================================================================

template<template<class, class> class Cont, typename T>
struct CompareIndex
{
	typedef std::pair<T, T>			t_pair;
	typedef std::allocator<t_pair>	t_alloc;

	// ? Type erasing
	typedef bool	(*t_cmp_fn)(const T&, const T&, const void *);

	CompareIndex( const Cont<t_pair, t_alloc> &pairs, t_cmp_fn fn, const void *ctx)
		: _pairs(pairs), _fn(fn), _ctx(ctx)
	{}

	bool	operator()(size_t l, size_t r) const
	{
		return _fn(_pairs[l].second, _pairs[r].second, _ctx);
	}

private:
	const Cont<t_pair, t_alloc>	&_pairs;
	t_cmp_fn					_fn;
	const void					*_ctx;
};

template<typename T, typename Compare>
static bool		cmp_thunk(const T &a, const T &b, const void *ctx)
{
	return (*static_cast<const Compare*>(ctx))(a, b);
}

typedef std::allocator<size_t>	t_idxalloc;

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
);

// ============================================================================
// Insertion
// ============================================================================

template<typename Iterator, typename T, typename Compare>
Iterator	binarySearch(
						Iterator first,
						Iterator last,
						const T& value,
						const Compare &comp
					);

template<
	template<class, class> class Cont,
	typename T,
	typename Alloc,
	typename Compare
>
void	insertPendsChain(
			Cont<T, Alloc> &mainChain,
			const Cont<
				std::pair<T, T>,
				std::allocator<std::pair<T, T> >
			> &pairs,
			const Cont<size_t, t_idxalloc> &sortedIdx,
			const Compare &comp
		);

size_t	jacobsthal(size_t n);

// ============================================================================
// Helpers
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
	dest = fordJohnsonSort(src);
	std::clock_t	end = std::clock();

	return static_cast<double>(end - start) / CLOCKS_PER_SEC * 1e6;
}

// ---- Check sorting ----
template <typename Iterator>
bool	isSorted(Iterator first, Iterator last)
{
	if (std::distance(first, last) <= 1)
		return true;
	
	Iterator next = first + 1;
	while (next != last)
	{
		if (*next < *first)
			return false;
		++first;
		++next;
	}
	return true;
}

template <typename Sequence>
bool	isSorted(const Sequence &seq)
{
	return isSorted(seq.begin(), seq.end());
}

// ---- Comparaison count ----

template<typename T, typename Compare>
struct CountingCompare
{
	CountingCompare(const Compare& c, size_t& counter)
		: comp(c), count(counter) {}

	bool operator()(const T& a, const T& b) const
	{
		++count;
		return comp(a, b);
	}

	Compare	comp;
	size_t	&count;
};

// ============================================================================
// Functions implementations
// ============================================================================

# ifndef __PMERGEME_TPP__
#  include "PMergeMe.tpp"
# endif

#endif /* __PMERGEME_HPP__ */
