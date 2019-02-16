// copied from https://cukic.co/2019/01/22/projections-without-ranges/
#include <functional>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <utility>
#include <numeric>

template <typename Function, typename Projection>
class composed_fn {
public:
	composed_fn(Function function, Projection projection)
		: m_function{std::move(function)}
		, m_projection{std::move(projection)}
	{
	}

	template <size_t Total, size_t Current, typename Type>
	decltype(auto) project_impl(Type&& arg) const
	{
		if constexpr (Total == Current + 1) {
			return std::invoke(m_projection, std::forward<Type>(arg));
		}
		else {
			return std::forward<Type>(arg);
		}
	}

	template <typename Tuple, std::size_t... Idx>
	decltype(auto) call_operator_impl(Tuple&& args, std::index_sequence<Idx...>) const
	{
		// ((std::cout<<project_impl<sizeof...(Idx), Idx>(std::get<Idx>(std::forward<Tuple>(args)))<<std::endl),...);
		return std::invoke(
				m_function,
				project_impl<sizeof...(Idx), Idx>(std::get<Idx>(std::forward<Tuple>(args)))...
				);
	}

	template <typename... Args>
	decltype(auto) operator() (Args&&... args) const
	{
		return call_operator_impl(
				std::make_tuple(std::forward<Args>(args)...), std::index_sequence_for<Args...>()
				);
	}

private:
	Function m_function;
	Projection m_projection;
};


template <typename Function, typename Projection>
class projecting_fn {
public:
	projecting_fn(Function function, Projection projection)
		: m_function{std::move(function)}
		, m_projection{std::move(projection)}
	{
	}

	template <typename... Args>
	decltype(auto) operator() (Args&&... args) const
	{
		return std::invoke(
				m_function,
				std::invoke(m_projection, std::forward<decltype(args)>(args))...);
	}

private:
	Function m_function;
	Projection m_projection;
};
