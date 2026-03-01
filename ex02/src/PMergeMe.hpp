/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PMergeMe.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sliziard <sliziard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 19:36:33 by sliziard          #+#    #+#             */
/*   Updated: 2026/03/01 16:18:09 by sliziard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __PMERGEME_HPP__
# define __PMERGEME_HPP__

# include <cstddef>
# include <ctime>
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

template<
	template<class, class> class Cont,
	typename T,
	typename Alloc,
	typename Compare
>
Cont<T, Alloc>	createMainChain(
	const PairsResult<Cont, T> &r,
	const Compare &comp
);

// ============================================================================
// Insertion
// ============================================================================

typedef std::allocator<size_t>	t_idxalloc;

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
			const Compare &comp
		);

size_t	jacobsthal(size_t n);

// ============================================================================
// Functions implementations
// ============================================================================

# ifndef __PMERGEME_TPP__
#  include "PMergeMe.tpp"
# endif

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

#endif /* __PMERGEME_HPP__ */
