#ifndef CURRENT_DIR_HPP
#define CURRENT_DIR_HPP 

#include <iostream>
#include "projections.hpp"
#include <immer/flex_vector.hpp>
#include <experimental/filesystem>
#include "optional_ref.hpp"
#include "file.hpp"
#include <variant>
#include <optional>
#include <range/v3/view.hpp>
#include <range/v3/action/sort.hpp>

namespace fs = std::experimental::filesystem;

class current_dir {
private:
	struct data {
		fs::path dir_path;
		immer::flex_vector<file> dirs;
		immer::flex_vector<file> regular_files;
	};


	std::variant<data, std::string> m_data;

	current_dir(const std::string& dir_path, immer::flex_vector<file> dirs, immer::flex_vector<file> regular_files);
public:

	current_dir() {}
	current_dir(const std::string& dir_path, bool error_flag = false);

	// we need to wrap this in optional_ref (optional reference), because if there is an error we cannot do antything with reference
	optional_ref<const fs::path> path() const;
	fs::path path(const file& f) const;
	fs::path path(const std::string& file_name) const;

	std::size_t num_of_files() const;
	std::size_t num_of_regular_files() const;
	std::size_t num_of_dirs() const;

	// get a file from an imaginary vector dirs+regular_files
	optional_ref<const file> file_by_index(unsigned i) const;
	// get a file from regular_files
	optional_ref<const file> regular_file_by_index(unsigned i) const;
	// get a file from dirs
	optional_ref<const file> dir_by_index(unsigned i) const;

	// get an index from an imaginary vector dirs+regular_files
	std::size_t file_index(const std::string& file_name) const;
	// get an index from regular_files
	std::size_t regular_file_index(const std::string& file_name) const;
	// get an index from dirs
	std::size_t dir_index(const std::string& file_name) const;

	current_dir cd(const fs::path& dir_path) const;
	current_dir rename(const file& f, const std::string& new_file_name) const;
	current_dir insert_file(const file& f) const;
	current_dir delete_file(const file& f) const;

	bool is_error_dir() const;

	// offset from all files
	immer::flex_vector<file>::iterator dirs(std::size_t offset = 0) const;
	immer::flex_vector<file>::iterator dirs_end() const;
	// offset from all files
	immer::flex_vector<file>::iterator regs(std::size_t offset = 0) const;
	immer::flex_vector<file>::iterator regs_end() const;

	template <typename Pred>
	friend auto transform(const current_dir& curdir, Pred&& pred);
};




namespace impl_detail {
    #define unused(x) ((void)x)

	template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
	template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

	template <class T>
	struct remove_cvref {
		typedef std::remove_cv_t<std::remove_reference_t<T>> type;
	};

	template <class T>
	using remove_cvref_t = typename remove_cvref<T>::type;

	// NOTE: std::remove_cvref since C++20

	template <
	  typename Variant,
	  typename Func,
	  typename T = std::variant_alternative_t<0, remove_cvref_t<Variant>>,
	  typename Error = std::variant_alternative_t<1, remove_cvref_t<Variant>>,
	  typename Result = std::invoke_result_t<Func, T>
	>
	std::variant<Result, Error> fmapv(Variant&& var, Func&& f)
	{
		return std::visit(overloaded {
				[&f](const T& data) { // Will f be correctly forwarded inside lambda?
					return std::variant<Result, Error>(std::forward<Func>(f)(data));
				},
				[](const Error& message) {
					return std::variant<Result, Error>(message);
				}
		}, std::forward<Variant>(var));
	}

	template <
		typename Variant,
		typename... Args,
		typename T = std::variant_alternative_t<0, remove_cvref_t<Variant>>,
		typename Error = std::variant_alternative_t<1, remove_cvref_t<Variant>>
	>
	T cast(Variant&& var, Args&&... args)
	{
		return std::visit(overloaded {
				[](const T& result) {
					return result;
				},
				[&args...](const Error& e) {
					unused(e);
					return T{std::forward<Args>(args)...};
				}
		}, std::forward<Variant>(var));
	}
}; // end of impl_detail

template <typename Pred>
auto transform(const current_dir& curdir, Pred&& pred)
{
	auto f = [&](const current_dir::data& data) {
		return  ranges::v3::view::concat(data.dirs, data.regular_files)
				| ranges::v3::view::transform(std::forward<Pred>(pred))
				| ranges::v3::to_vector;
	};

	return impl_detail::cast(impl_detail::fmapv(curdir.m_data, f));
}

#endif /* CURRENT_DIR_HPP */
