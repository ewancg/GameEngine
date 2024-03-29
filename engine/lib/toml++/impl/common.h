//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

#pragma once
#include "preprocessor.h"

//#====================================================================================================================
//# INCLUDES
//#====================================================================================================================

TOML_DISABLE_WARNINGS;
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cfloat>
#include <climits>
#include <cmath>
#include <iterator>
#include <limits>
#include <memory>
#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <iosfwd>
#include <new>
#if !TOML_HAS_CUSTOM_OPTIONAL_TYPE
	#include <optional>
#endif
#if TOML_EXCEPTIONS
	#include <stdexcept>
#endif
#if TOML_INT_CHARCONV || TOML_FLOAT_CHARCONV
	#include <charconv>
#endif
#if !TOML_INT_CHARCONV || !TOML_FLOAT_CHARCONV
	#include <sstream>
#endif
#if !TOML_INT_CHARCONV
	#include <iomanip>
#endif
TOML_ENABLE_WARNINGS;

#if defined(__cpp_lib_launder) && __cpp_lib_launder >= 201606
	#define TOML_LAUNDER(x) std::launder(x)
#else
	#define TOML_LAUNDER(x) x
#endif

#if defined(DOXYGEN)                                                                                                   \
	|| (defined(__cpp_char8_t) && __cpp_char8_t >= 201811 && defined(__cpp_lib_char8_t)                                \
		&& __cpp_lib_char8_t >= 201907)
	#define TOML_HAS_CHAR8 1
#else
	#define TOML_HAS_CHAR8 0
#endif

//#====================================================================================================================
//# ENVIRONMENT GROUND-TRUTHS
//#====================================================================================================================

/// \cond
#ifndef TOML_DISABLE_ENVIRONMENT_CHECKS
	#define TOML_ENV_MESSAGE                                                                                           \
		"If you're seeing this error it's because you're building toml++ for an environment that doesn't conform to "  \
		"one of the 'ground truths' assumed by the library. Essentially this just means that I don't have the "        \
		"resources to test on more platforms, but I wish I did! You can try disabling the checks by defining "         \
		"TOML_DISABLE_ENVIRONMENT_CHECKS, but your mileage may vary. Please consider filing an issue at "              \
		"https://github.com/marzer/tomlplusplus/issues to help me improve support for your target environment. "       \
		"Thanks!"

static_assert(CHAR_BIT == 8, TOML_ENV_MESSAGE);
static_assert(FLT_RADIX == 2, TOML_ENV_MESSAGE);
static_assert('A' == 65, TOML_ENV_MESSAGE);
static_assert(sizeof(double) == 8, TOML_ENV_MESSAGE);
static_assert(std::numeric_limits<double>::is_iec559, TOML_ENV_MESSAGE);
static_assert(std::numeric_limits<double>::digits == 53, TOML_ENV_MESSAGE);
static_assert(std::numeric_limits<double>::digits10 == 15, TOML_ENV_MESSAGE);

	#undef TOML_ENV_MESSAGE
#endif // !TOML_DISABLE_ENVIRONMENT_CHECKS
/// \endcond

//#====================================================================================================================
//# UNDOCUMENTED TYPEDEFS AND FORWARD DECLARATIONS
//#====================================================================================================================

/// \cond
// undocumented forward declarations are hidden from doxygen because they fuck it up =/

namespace toml // non-abi namespace; this is not an error
{
	using namespace std::string_literals;
	using namespace std::string_view_literals;
	using ::std::size_t;
	using ::std::intptr_t;
	using ::std::uintptr_t;
	using ::std::ptrdiff_t;
	using ::std::nullptr_t;
	using ::std::int8_t;
	using ::std::int16_t;
	using ::std::int32_t;
	using ::std::int64_t;
	using ::std::uint8_t;
	using ::std::uint16_t;
	using ::std::uint32_t;
	using ::std::uint64_t;
	using ::std::uint_least32_t;
	using ::std::uint_least64_t;

	// legacy typedefs
	using string_char = char;
	using string	  = std::string;
	using string_view = std::string_view;
}

TOML_NAMESPACE_START // abi namespace
{
	struct date;
	struct time;
	struct time_offset;

	TOML_ABI_NAMESPACE_BOOL(TOML_HAS_CUSTOM_OPTIONAL_TYPE, custopt, stdopt);
	struct date_time;
	TOML_ABI_NAMESPACE_END;

	class node;
	class array;
	class table;

	template <typename>
	class node_view;
	template <typename>
	class value;
	template <typename>
	class default_formatter;
	template <typename>
	class json_formatter;

	TOML_NODISCARD
	TOML_API
	bool operator==(const array& lhs, const array& rhs) noexcept;

	TOML_NODISCARD
	TOML_API
	bool operator!=(const array& lhs, const array& rhs) noexcept;

	TOML_NODISCARD
	TOML_API
	bool operator==(const table& lhs, const table& rhs) noexcept;

	TOML_NODISCARD
	TOML_API
	bool operator!=(const table& lhs, const table& rhs) noexcept;

	template <typename Char>
	std::basic_ostream<Char>& operator<<(std::basic_ostream<Char>&, const array&);

	template <typename Char, typename T>
	std::basic_ostream<Char>& operator<<(std::basic_ostream<Char>&, const value<T>&);

	template <typename Char>
	std::basic_ostream<Char>& operator<<(std::basic_ostream<Char>&, const table&);

	template <typename T, typename U>
	std::basic_ostream<T>& operator<<(std::basic_ostream<T>&, default_formatter<U>&);

	template <typename T, typename U>
	std::basic_ostream<T>& operator<<(std::basic_ostream<T>&, default_formatter<U>&&);

	template <typename T, typename U>
	std::basic_ostream<T>& operator<<(std::basic_ostream<T>&, json_formatter<U>&);

	template <typename T, typename U>
	std::basic_ostream<T>& operator<<(std::basic_ostream<T>&, json_formatter<U>&&);

	template <typename Char, typename T>
	inline std::basic_ostream<Char>& operator<<(std::basic_ostream<Char>&, const node_view<T>&);

	TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, ex, noex);
#if TOML_EXCEPTIONS
	using parse_result = table;
#else
	class parse_result;
#endif
	TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS

	namespace impl
	{
		template <typename T>
		using string_map = std::map<std::string, T, std::less<>>; // heterogeneous lookup

		template <typename T>
		using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

		template <typename T, typename... U>
		inline constexpr bool is_one_of = (false || ... || std::is_same_v<T, U>);

		template <typename T>
		inline constexpr bool is_cvref = std::is_reference_v<T> || std::is_const_v<T> || std::is_volatile_v<T>;

		template <typename T>
		inline constexpr bool is_wide_string =
			is_one_of<std::decay_t<T>, const wchar_t*, wchar_t*, std::wstring_view, std::wstring>;

		template <typename T>
		inline constexpr bool dependent_false = false;

#if TOML_WINDOWS_COMPAT

		TOML_NODISCARD
		TOML_API
		std::string narrow(std::wstring_view) noexcept;

		TOML_NODISCARD
		TOML_API
		std::wstring widen(std::string_view) noexcept;

	#if TOML_HAS_CHAR8
		TOML_NODISCARD
		TOML_API
		std::wstring widen(std::u8string_view) noexcept;
	#endif
#endif // TOML_WINDOWS_COMPAT

		TOML_ABI_NAMESPACE_BOOL(TOML_EXCEPTIONS, ex, noex);
		class parser;
		TOML_ABI_NAMESPACE_END; // TOML_EXCEPTIONS

#if TOML_ABI_NAMESPACES
	#if TOML_EXCEPTIONS
		#define TOML_PARSER_TYPENAME TOML_NAMESPACE::impl::ex::parser
	#else
		#define TOML_PARSER_TYPENAME TOML_NAMESPACE::impl::noex::parser
	#endif
#else
	#define TOML_PARSER_TYPENAME TOML_NAMESPACE::impl::parser
#endif
	}
}
TOML_NAMESPACE_END;

/// \endcond

//#====================================================================================================================
//# TYPEDEFS AND FORWARD DECLARATIONS
//#====================================================================================================================

/// \brief	The root namespace for all toml++ functions and types.
namespace toml
{
}

TOML_NAMESPACE_START // abi namespace
{
	/// \brief	Convenience literal operators for working with toml++.
	///
	/// \detail This namespace exists so you can safely hoist the toml++ literal operators into another scope
	/// 		 without dragging in everything from the toml namespace: \cpp
	///
	/// #include <toml++/toml.h>
	///	using namespace toml::literals;
	///
	///	int main()
	///	{
	///		toml::table tbl = "vals = [1, 2, 3]"_toml;
	///
	///		// ... do stuff with the table generated by the "_toml" literal ...
	///
	///		return 0;
	///	}
	/// \ecpp
	///
	inline namespace literals
	{
	}

#if TOML_HAS_CUSTOM_OPTIONAL_TYPE
	template <typename T>
	using optional = TOML_OPTIONAL_TYPE<T>;
#else
	/// \brief	The 'optional' type used throughout the library.
	///
	/// \remarks By default this will be an alias for std::optional, but you can change the optional type
	/// 		 used by the library by defining #TOML_OPTIONAL_TYPE.
	template <typename T>
	using optional = std::optional<T>;
#endif

	/// \brief	TOML node type identifiers.
	enum class node_type : uint8_t
	{
		none,			///< Not-a-node.
		table,			///< The node is a toml::table.
		array,			///< The node is a toml::array.
		string,			///< The node is a toml::value<std::string>.
		integer,		///< The node is a toml::value<int64_t>.
		floating_point, ///< The node is a toml::value<double>.
		boolean,		///< The node is a toml::value<bool>.
		date,			///< The node is a toml::value<date>.
		time,			///< The node is a toml::value<time>.
		date_time		///< The node is a toml::value<date_time>.
	};

	/// \brief	A pointer to a shared string resource containing a source path.
	using source_path_ptr = std::shared_ptr<const std::string>;

	/// \brief	Helper class for suppressing move-construction in single-argument array constructors.
	///
	/// \detail \cpp
	/// // desired result: [ [ 42 ] ]
	/// auto bad = toml::array{ toml::array{ 42 } }
	/// auto good = toml::array{ toml::inserter{ toml::array{ 42 } } }
	/// std::cout << "bad: " << bad << "\n";
	/// std::cout << "good:" << good << "\n";
	/// \ecpp
	///
	/// \out
	/// bad:  [ 42 ]
	/// good: [ [ 42 ] ]
	/// \eout
	///
	/// \see toml::array
	template <typename T>
	struct TOML_TRIVIAL_ABI inserter
	{
		T&& value;
	};
	template <typename T>
	inserter(T &&) -> inserter<T>;
}
TOML_NAMESPACE_END;

//#====================================================================================================================
//# VALUE AND NODE TRAITS
//#====================================================================================================================

/// \cond
TOML_IMPL_NAMESPACE_START
{
	// general value traits
	// (as they relate to their equivalent native TOML type)
	template <typename T>
	struct value_traits
	{
		using native_type										  = void;
		static constexpr bool is_native							  = false;
		static constexpr bool is_losslessly_convertible_to_native = false;
		static constexpr bool can_represent_native				  = false;
		static constexpr bool can_partially_represent_native	  = false;
		static constexpr auto type								  = node_type::none;
	};
	template <typename T>
	struct value_traits<T&> : value_traits<T>
	{};
	template <typename T>
	struct value_traits<T&&> : value_traits<T>
	{};
	template <typename T>
	struct value_traits<T* const> : value_traits<T*>
	{};
	template <typename T>
	struct value_traits<T* volatile> : value_traits<T*>
	{};
	template <typename T>
	struct value_traits<T* const volatile> : value_traits<T*>
	{};

	// integer value traits
	template <typename T>
	struct integer_value_limits
	{
		static constexpr auto min = (std::numeric_limits<T>::min)();
		static constexpr auto max = (std::numeric_limits<T>::max)();
	};
	template <typename T>
	struct integer_value_traits_base : integer_value_limits<T>
	{
		using native_type				= int64_t;
		static constexpr bool is_native = std::is_same_v<T, native_type>;
		static constexpr bool is_signed = static_cast<T>(-1) < T{}; // for impls not specializing std::is_signed<T>
		static constexpr auto type		= node_type::integer;
		static constexpr bool can_partially_represent_native = true;
	};
	template <typename T>
	struct unsigned_integer_value_traits : integer_value_traits_base<T>
	{
		static constexpr bool is_losslessly_convertible_to_native =
			integer_value_limits<T>::max <= 9223372036854775807ULL;
		static constexpr bool can_represent_native = false;
	};
	template <typename T>
	struct signed_integer_value_traits : integer_value_traits_base<T>
	{
		using native_type = int64_t;
		static constexpr bool is_losslessly_convertible_to_native =
			integer_value_limits<T>::min >= (-9223372036854775807LL - 1LL)
			&& integer_value_limits<T>::max <= 9223372036854775807LL;
		static constexpr bool can_represent_native = integer_value_limits<T>::min <= (-9223372036854775807LL - 1LL)
												  && integer_value_limits<T>::max >= 9223372036854775807LL;
	};
	template <typename T, bool S = integer_value_traits_base<T>::is_signed>
	struct integer_value_traits : signed_integer_value_traits<T>
	{};
	template <typename T>
	struct integer_value_traits<T, false> : unsigned_integer_value_traits<T>
	{};
	template <>
	struct value_traits<signed char> : integer_value_traits<signed char>
	{};
	template <>
	struct value_traits<unsigned char> : integer_value_traits<unsigned char>
	{};
	template <>
	struct value_traits<signed short> : integer_value_traits<signed short>
	{};
	template <>
	struct value_traits<unsigned short> : integer_value_traits<unsigned short>
	{};
	template <>
	struct value_traits<signed int> : integer_value_traits<signed int>
	{};
	template <>
	struct value_traits<unsigned int> : integer_value_traits<unsigned int>
	{};
	template <>
	struct value_traits<signed long> : integer_value_traits<signed long>
	{};
	template <>
	struct value_traits<unsigned long> : integer_value_traits<unsigned long>
	{};
	template <>
	struct value_traits<signed long long> : integer_value_traits<signed long long>
	{};
	template <>
	struct value_traits<unsigned long long> : integer_value_traits<unsigned long long>
	{};
#ifdef TOML_INT128
	template <>
	struct integer_value_limits<TOML_INT128>
	{
		static constexpr TOML_INT128 max =
			static_cast<TOML_INT128>((TOML_UINT128{ 1u } << ((__SIZEOF_INT128__ * CHAR_BIT) - 1)) - 1);
		static constexpr TOML_INT128 min = -max - TOML_INT128{ 1 };
	};
	template <>
	struct integer_value_limits<TOML_UINT128>
	{
		static constexpr TOML_UINT128 min = TOML_UINT128{};
		static constexpr TOML_UINT128 max =
			(2u * static_cast<TOML_UINT128>(integer_value_limits<TOML_INT128>::max)) + 1u;
	};
	template <>
	struct value_traits<TOML_INT128> : integer_value_traits<TOML_INT128>
	{};
	template <>
	struct value_traits<TOML_UINT128> : integer_value_traits<TOML_UINT128>
	{};
#endif
#ifdef TOML_SMALL_INT_TYPE
	template <>
	struct value_traits<TOML_SMALL_INT_TYPE> : signed_integer_value_traits<TOML_SMALL_INT_TYPE>
	{};
#endif
	static_assert(value_traits<int64_t>::is_native);
	static_assert(value_traits<int64_t>::is_signed);
	static_assert(value_traits<int64_t>::is_losslessly_convertible_to_native);
	static_assert(value_traits<int64_t>::can_represent_native);
	static_assert(value_traits<int64_t>::can_partially_represent_native);

	// float value traits
	template <typename T>
	struct float_value_limits
	{
		static constexpr bool is_iec559 = std::numeric_limits<T>::is_iec559;
		static constexpr int digits		= std::numeric_limits<T>::digits;
		static constexpr int digits10	= std::numeric_limits<T>::digits10;
	};
	template <typename T>
	struct float_value_traits : float_value_limits<T>
	{
		using native_type										  = double;
		static constexpr bool is_native							  = std::is_same_v<T, native_type>;
		static constexpr bool is_signed							  = true;
		static constexpr bool is_losslessly_convertible_to_native = float_value_limits<T>::is_iec559
																 && float_value_limits<T>::digits <= 53
																 && float_value_limits<T>::digits10 <= 15;
		static constexpr bool can_represent_native = float_value_limits<T>::is_iec559
												  && float_value_limits<T>::digits >= 53	// DBL_MANT_DIG
												  && float_value_limits<T>::digits10 >= 15; // DBL_DIG
		static constexpr bool can_partially_represent_native								// 32-bit float values
			= float_value_limits<T>::is_iec559 && float_value_limits<T>::digits >= 24
		   && float_value_limits<T>::digits10 >= 6;
		static constexpr auto type = node_type::floating_point;
	};
	template <>
	struct value_traits<float> : float_value_traits<float>
	{};
	template <>
	struct value_traits<double> : float_value_traits<double>
	{};
	template <>
	struct value_traits<long double> : float_value_traits<long double>
	{};
	template <int mant_dig, int dig>
	struct extended_float_value_limits
	{
		static constexpr bool is_iec559 = true;
		static constexpr int digits		= mant_dig;
		static constexpr int digits10	= dig;
	};
#ifdef TOML_FP16
	template <>
	struct float_value_limits<TOML_FP16> : extended_float_value_limits<__FLT16_MANT_DIG__, __FLT16_DIG__>
	{};
	template <>
	struct value_traits<TOML_FP16> : float_value_traits<TOML_FP16>
	{};
#endif
#ifdef TOML_FLOAT16
	template <>
	struct float_value_limits<TOML_FLOAT16> : extended_float_value_limits<__FLT16_MANT_DIG__, __FLT16_DIG__>
	{};
	template <>
	struct value_traits<TOML_FLOAT16> : float_value_traits<TOML_FLOAT16>
	{};
#endif
#ifdef TOML_FLOAT128
	template <>
	struct float_value_limits<TOML_FLOAT128> : extended_float_value_limits<__FLT128_MANT_DIG__, __FLT128_DIG__>
	{};
	template <>
	struct value_traits<TOML_FLOAT128> : float_value_traits<TOML_FLOAT128>
	{};
#endif
#ifdef TOML_SMALL_FLOAT_TYPE
	template <>
	struct value_traits<TOML_SMALL_FLOAT_TYPE> : float_value_traits<TOML_SMALL_FLOAT_TYPE>
	{};
#endif
	static_assert(value_traits<double>::is_native);
	static_assert(value_traits<double>::is_losslessly_convertible_to_native);
	static_assert(value_traits<double>::can_represent_native);
	static_assert(value_traits<double>::can_partially_represent_native);

	// string value traits
	template <typename T>
	struct string_value_traits
	{
		using native_type										  = std::string;
		static constexpr bool is_native							  = std::is_same_v<T, native_type>;
		static constexpr bool is_losslessly_convertible_to_native = true;
		static constexpr bool can_represent_native =
			!std::is_array_v<T> && (!std::is_pointer_v<T> || std::is_const_v<std::remove_pointer_t<T>>);
		static constexpr bool can_partially_represent_native = can_represent_native;
		static constexpr auto type							 = node_type::string;
	};
	template <>
	struct value_traits<std::string> : string_value_traits<std::string>
	{};
	template <>
	struct value_traits<std::string_view> : string_value_traits<std::string_view>
	{};
	template <>
	struct value_traits<const char*> : string_value_traits<const char*>
	{};
	template <size_t N>
	struct value_traits<const char[N]> : string_value_traits<const char[N]>
	{};
	template <>
	struct value_traits<char*> : string_value_traits<char*>
	{};
	template <size_t N>
	struct value_traits<char[N]> : string_value_traits<char[N]>
	{};
#if TOML_HAS_CHAR8
	template <>
	struct value_traits<std::u8string> : string_value_traits<std::u8string>
	{};
	template <>
	struct value_traits<std::u8string_view> : string_value_traits<std::u8string_view>
	{};
	template <>
	struct value_traits<const char8_t*> : string_value_traits<const char8_t*>
	{};
	template <size_t N>
	struct value_traits<const char8_t[N]> : string_value_traits<const char8_t[N]>
	{};
	template <>
	struct value_traits<char8_t*> : string_value_traits<char8_t*>
	{};
	template <size_t N>
	struct value_traits<char8_t[N]> : string_value_traits<char8_t[N]>
	{};
#endif
#if TOML_WINDOWS_COMPAT
	template <typename T>
	struct wstring_value_traits
	{
		using native_type										  = std::string;
		static constexpr bool is_native							  = false;
		static constexpr bool is_losslessly_convertible_to_native = true;							 // narrow
		static constexpr bool can_represent_native				  = std::is_same_v<T, std::wstring>; // widen
		static constexpr bool can_partially_represent_native	  = can_represent_native;
		static constexpr auto type								  = node_type::string;
	};
	template <>
	struct value_traits<std::wstring> : wstring_value_traits<std::wstring>
	{};
	template <>
	struct value_traits<std::wstring_view> : wstring_value_traits<std::wstring_view>
	{};
	template <>
	struct value_traits<const wchar_t*> : wstring_value_traits<const wchar_t*>
	{};
	template <size_t N>
	struct value_traits<const wchar_t[N]> : wstring_value_traits<const wchar_t[N]>
	{};
	template <>
	struct value_traits<wchar_t*> : wstring_value_traits<wchar_t*>
	{};
	template <size_t N>
	struct value_traits<wchar_t[N]> : wstring_value_traits<wchar_t[N]>
	{};
#endif

	// other native value traits
	template <typename T, node_type NodeType>
	struct native_value_traits
	{
		using native_type										  = T;
		static constexpr bool is_native							  = true;
		static constexpr bool is_losslessly_convertible_to_native = true;
		static constexpr bool can_represent_native				  = true;
		static constexpr bool can_partially_represent_native	  = true;
		static constexpr auto type								  = NodeType;
	};
	template <>
	struct value_traits<bool> : native_value_traits<bool, node_type::boolean>
	{};
	template <>
	struct value_traits<date> : native_value_traits<date, node_type::date>
	{};
	template <>
	struct value_traits<time> : native_value_traits<time, node_type::time>
	{};
	template <>
	struct value_traits<date_time> : native_value_traits<date_time, node_type::date_time>
	{};

	// native value category queries
	template <typename T>
	using native_type_of = typename value_traits<T>::native_type;
	template <typename T>
	inline constexpr bool is_native = value_traits<T>::is_native;
	template <typename T>
	inline constexpr bool can_represent_native = value_traits<T>::can_represent_native;
	template <typename T>
	inline constexpr bool can_partially_represent_native = value_traits<T>::can_partially_represent_native;
	template <typename T>
	inline constexpr bool is_losslessly_convertible_to_native = value_traits<T>::is_losslessly_convertible_to_native;
	template <typename T, typename... U>
	inline constexpr bool is_natively_one_of = is_one_of<native_type_of<T>, U...>;

	// native <=> node conversions
	template <typename T>
	struct node_wrapper
	{
		using type = T;
	};
	template <>
	struct node_wrapper<std::string>
	{
		using type = value<std::string>;
	};
	template <>
	struct node_wrapper<int64_t>
	{
		using type = value<int64_t>;
	};
	template <>
	struct node_wrapper<double>
	{
		using type = value<double>;
	};
	template <>
	struct node_wrapper<bool>
	{
		using type = value<bool>;
	};
	template <>
	struct node_wrapper<date>
	{
		using type = value<date>;
	};
	template <>
	struct node_wrapper<time>
	{
		using type = value<time>;
	};
	template <>
	struct node_wrapper<date_time>
	{
		using type = value<date_time>;
	};
	template <typename T>
	using wrap_node = typename node_wrapper<T>::type;

	template <typename T>
	struct node_unwrapper
	{
		using type = T;
	};
	template <typename T>
	struct node_unwrapper<value<T>>
	{
		using type = T;
	};
	template <typename T>
	using unwrap_node = typename node_unwrapper<T>::type;

	template <typename T>
	struct node_type_getter
	{
		static constexpr auto value = value_traits<T>::type;
	};
	template <>
	struct node_type_getter<table>
	{
		static constexpr auto value = node_type::table;
	};
	template <>
	struct node_type_getter<array>
	{
		static constexpr auto value = node_type::array;
	};
	template <>
	struct node_type_getter<void>
	{
		static constexpr auto value = node_type::none;
	};
	template <typename T>
	inline constexpr node_type node_type_of = node_type_getter<unwrap_node<remove_cvref_t<T>>>::value;
}
TOML_IMPL_NAMESPACE_END;
/// \endcond

TOML_NAMESPACE_START
{
	/// \brief	Metafunction for determining if a type is, or is a reference to, a toml::table.
	template <typename T>
	inline constexpr bool is_table = std::is_same_v<impl::remove_cvref_t<T>, table>;

	/// \brief	Metafunction for determining if a type is, or is a reference to, a toml::array.
	template <typename T>
	inline constexpr bool is_array = std::is_same_v<impl::remove_cvref_t<T>, array>;

	/// \brief	Metafunction for determining if a type satisfies either toml::is_table or toml::is_array.
	template <typename T>
	inline constexpr bool is_container = is_table<T> || is_array<T>;

	/// \brief	Metafunction for determining if a type is, or is a reference to, a std::string or toml::value<std::string>.
	template <typename T>
	inline constexpr bool is_string = std::is_same_v<impl::wrap_node<impl::remove_cvref_t<T>>, value<std::string>>;

	/// \brief	Metafunction for determining if a type is, or is a reference to, a int64_t or toml::value<int64_t>.
	template <typename T>
	inline constexpr bool is_integer = std::is_same_v<impl::wrap_node<impl::remove_cvref_t<T>>, value<int64_t>>;

	/// \brief	Metafunction for determining if a type is, or is a reference to, a double or toml::value<double>.
	template <typename T>
	inline constexpr bool is_floating_point = std::is_same_v<impl::wrap_node<impl::remove_cvref_t<T>>, value<double>>;

	/// \brief	Metafunction for determining if a type satisfies either toml::is_integer or toml::is_floating_point.
	template <typename T>
	inline constexpr bool is_number = is_integer<T> || is_floating_point<T>;

	/// \brief	Metafunction for determining if a type is, or is a reference to, a bool or toml::value<bool>.
	template <typename T>
	inline constexpr bool is_boolean = std::is_same_v<impl::wrap_node<impl::remove_cvref_t<T>>, value<bool>>;

	/// \brief	Metafunction for determining if a type is, or is a reference to, a toml::date or toml::value<date>.
	template <typename T>
	inline constexpr bool is_date = std::is_same_v<impl::wrap_node<impl::remove_cvref_t<T>>, value<date>>;

	/// \brief	Metafunction for determining if a type is, or is a reference to, a toml::time or toml::value<time>.
	template <typename T>
	inline constexpr bool is_time = std::is_same_v<impl::wrap_node<impl::remove_cvref_t<T>>, value<time>>;

	/// \brief	Metafunction for determining if a type is, or is a reference to, a toml::date_time or toml::value<date_time>.
	template <typename T>
	inline constexpr bool is_date_time = std::is_same_v<impl::wrap_node<impl::remove_cvref_t<T>>, value<date_time>>;

	/// \brief	Metafunction for determining if a type satisfies any of toml::is_date, toml::is_time or toml::is_date_time.
	template <typename T>
	inline constexpr bool is_chronological = is_date<T> || is_time<T> || is_date_time<T>;

	/// \brief	Metafunction for determining if a type is, or is a reference to, any of the toml value types. Excludes tables and arrays.
	template <typename T>
	inline constexpr bool is_value = is_string<T> || is_number<T> || is_boolean<T> || is_chronological<T>;

	/// \brief	Metafunction for determining if a type is, or is a reference to, a toml::node (or one of its subclasses).
	template <typename T>
	inline constexpr bool is_node =
		std::is_same_v<toml::node, impl::remove_cvref_t<T>> || std::is_base_of_v<toml::node, impl::remove_cvref_t<T>>;

	/// \brief	Metafunction for determining if a type is, or is a reference to, a toml::node_view.
	template <typename T>
	inline constexpr bool is_node_view =
		impl::is_one_of<impl::remove_cvref_t<T>, node_view<node>, node_view<const node>>;
}
TOML_NAMESPACE_END;

//#====================================================================================================================
//# INTERNAL HELPERS
//#====================================================================================================================

/// \cond
TOML_IMPL_NAMESPACE_START
{
	template <typename T>
	TOML_NODISCARD
	TOML_ATTR(const)
	TOML_ALWAYS_INLINE
	constexpr std::underlying_type_t<T> unwrap_enum(T val) noexcept
	{
		return static_cast<std::underlying_type_t<T>>(val);
	}

	// Q: "why not use the built-in fpclassify?"
	// A: Because it gets broken by -ffast-math and friends
	enum class fp_class : unsigned
	{
		ok,
		neg_inf,
		pos_inf,
		nan
	};
	TOML_NODISCARD
	TOML_ATTR(pure)
	inline fp_class fpclassify(const double& val) noexcept
	{
		static_assert(sizeof(uint64_t) == sizeof(double));

		static constexpr uint64_t sign	   = 0b1000000000000000000000000000000000000000000000000000000000000000ull;
		static constexpr uint64_t exponent = 0b0111111111110000000000000000000000000000000000000000000000000000ull;
		static constexpr uint64_t mantissa = 0b0000000000001111111111111111111111111111111111111111111111111111ull;

		uint64_t val_bits;
		memcpy(&val_bits, &val, sizeof(val));
		if ((val_bits & exponent) != exponent)
			return fp_class::ok;
		if ((val_bits & mantissa))
			return fp_class::nan;
		return (val_bits & sign) ? fp_class::neg_inf : fp_class::pos_inf;
	}

	// Q: "why not use std::find??"
	// A: Because <algorithm> is _huge_ and std::find would be the only thing I used from it.
	//    I don't want to impose such a heavy compile-time burden on users.
	template <typename T>
	TOML_NODISCARD
	inline const T* find(const std::vector<T>& haystack, const T& needle) noexcept
	{
		for (size_t i = 0, e = haystack.size(); i < e; i++)
			if (haystack[i] == needle)
				return haystack.data() + i;
		return nullptr;
	}

	// clang-format off

	inline constexpr std::string_view low_character_escape_table[] =
	{
		"\\u0000"sv,
		"\\u0001"sv,
		"\\u0002"sv,
		"\\u0003"sv,
		"\\u0004"sv,
		"\\u0005"sv,
		"\\u0006"sv,
		"\\u0007"sv,
		"\\b"sv,
		"\\t"sv,
		"\\n"sv,
		"\\u000B"sv,
		"\\f"sv,
		"\\r"sv,
		"\\u000E"sv,
		"\\u000F"sv,
		"\\u0010"sv,
		"\\u0011"sv,
		"\\u0012"sv,
		"\\u0013"sv,
		"\\u0014"sv,
		"\\u0015"sv,
		"\\u0016"sv,
		"\\u0017"sv,
		"\\u0018"sv,
		"\\u0019"sv,
		"\\u001A"sv,
		"\\u001B"sv,
		"\\u001C"sv,
		"\\u001D"sv,
		"\\u001E"sv,
		"\\u001F"sv,
	};

	inline constexpr std::string_view node_type_friendly_names[] =
	{
		"none"sv,
		"table"sv,
		"array"sv,
		"string"sv,
		"integer"sv,
		"floating-point"sv,
		"boolean"sv,
		"date"sv,
		"time"sv,
		"date-time"sv
	};

	// clang-format on

	template <typename T>
	TOML_NODISCARD
	TOML_ATTR(returns_nonnull)
	auto* make_node_specialized(T && val) noexcept
	{
		using type = unwrap_node<remove_cvref_t<T>>;
		static_assert(!std::is_same_v<type, node>);
		static_assert(!is_node_view<type>);

		if constexpr (is_one_of<type, array, table>)
		{
			return new type{ static_cast<T&&>(val) };
		}
		else if constexpr (is_native<type> && !std::is_same_v<remove_cvref_t<T>, type>)
		{
			return new value<type>{ static_cast<T&&>(val) };
		}
		else
		{
			static_assert(!is_wide_string<T> || TOML_WINDOWS_COMPAT,
						  "Instantiating values from wide-character strings is only "
						  "supported on Windows with TOML_WINDOWS_COMPAT enabled.");
			static_assert(is_native<type> || is_losslessly_convertible_to_native<type>,
						  "Value initializers must be (or be promotable to) one of the TOML value types");

			using value_type = native_type_of<remove_cvref_t<T>>;
			if constexpr (is_wide_string<T>)
			{
#if TOML_WINDOWS_COMPAT
				return new value<value_type>{ narrow(static_cast<T&&>(val)) };
#else
				static_assert(dependent_false<T>, "Evaluated unreachable branch!");
#endif
			}
			else
				return new value<value_type>{ static_cast<T&&>(val) };
		}
	}

	template <typename T>
	TOML_NODISCARD
	auto* make_node(T && val) noexcept
	{
		using type = unwrap_node<remove_cvref_t<T>>;
		if constexpr (std::is_same_v<type, node> || is_node_view<type>)
		{
			if constexpr (is_node_view<type>)
			{
				if (!val)
					return static_cast<toml::node*>(nullptr);
			}

			return static_cast<T&&>(val).visit(
				[](auto&& concrete) noexcept {
					return static_cast<toml::node*>(make_node_specialized(static_cast<decltype(concrete)&&>(concrete)));
				});
		}
		else
			return make_node_specialized(static_cast<T&&>(val));
	}

	template <typename T>
	TOML_NODISCARD
	auto* make_node(inserter<T> && val) noexcept
	{
		return make_node(static_cast<T&&>(val.value));
	}

	template <typename T, bool = (is_node<T> || is_node_view<T> || is_value<T> || can_partially_represent_native<T>)>
	struct inserted_type_of_
	{
		using type = std::remove_pointer_t<decltype(make_node(std::declval<T>()))>;
	};
	template <typename T>
	struct inserted_type_of_<inserter<T>, false>
	{
		using type = typename inserted_type_of_<T>::type;
	};
	template <typename T>
	struct inserted_type_of_<T, false>
	{
		using type = void;
	};
}
TOML_IMPL_NAMESPACE_END;
/// \endcond

TOML_NAMESPACE_START
{
	/// \brief	Metafunction for determining which node type would be constructed
	///			if an object of this type was inserted into a toml::table or toml::array.
	///
	/// \note	This will return toml::node for nodes and node_views, even though a more specific node subclass
	///			would actually be inserted. There is no way around this in a compile-time metafunction.
	template <typename T>
	using inserted_type_of = typename impl::inserted_type_of_<impl::remove_cvref_t<T>>::type;

	TOML_NODISCARD
	TOML_ATTR(const)
	TOML_ALWAYS_INLINE
	TOML_CONSTEVAL
	size_t operator"" _sz(unsigned long long n) noexcept
	{
		return static_cast<size_t>(n);
	}
}
TOML_NAMESPACE_END;

//#====================================================================================================================
//# SOURCE POSITIONS & REGIONS
//#====================================================================================================================

TOML_NAMESPACE_START
{
	TOML_ABI_NAMESPACE_BOOL(TOML_LARGE_FILES, lf, sf);

#if TOML_LARGE_FILES
	using source_index = uint32_t;
#else
	/// \brief	The integer type used to tally line numbers and columns.
	/// \remarks This will be an alias for uint32_t if #TOML_LARGE_FILES is enabled.
	using source_index = uint16_t;
#endif

	/// \brief	A source document line-and-column pair.
	///
	/// \detail \cpp
	/// auto table = toml::parse_file("config.toml"sv);
	/// std::cout << "The node 'description' was defined at "sv
	///		<< table.get("description")->source().begin()
	///		<< "\n";
	///
	/// \ecpp
	///
	/// \out
	///	The value 'description' was defined at line 7, column 15
	/// \eout
	///
	/// \remarks toml++'s parser is unicode-aware insofar as it knows how to handle
	/// 		 various non-conventional whitespace and newline characters, but it doesn't give
	/// 		 much thought to combining marks, grapheme clusters vs. characters, et cetera.
	/// 		 If a TOML document contains lots of codepoints outside of the ASCII range
	/// 		 you may find that your source_positions don't match those given by a text editor
	/// 		 (typically the line numbers will be accurate but column numbers will be too high).
	/// 		 <strong>This is not an error.</strong> I've chosen this behaviour as a deliberate trade-off
	/// 		 between parser complexity and correctness.
	struct TOML_TRIVIAL_ABI source_position
	{
		/// \brief The line number.
		/// \remarks Valid line numbers start at 1.
		source_index line;

		/// \brief The column number.
		/// \remarks Valid column numbers start at 1.
		source_index column;

		/// \brief	Returns true if both line and column numbers are non-zero.
		TOML_NODISCARD
		explicit constexpr operator bool() const noexcept
		{
			return line > source_index{} && column > source_index{};
		}

		/// \brief	Returns true if two source_positions represent the same line and column.
		TOML_NODISCARD
		friend constexpr bool operator==(const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line == rhs.line && lhs.column == rhs.column;
		}

		/// \brief	Returns true if two source_positions do not represent the same line and column.
		TOML_NODISCARD
		friend constexpr bool operator!=(const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line != rhs.line || lhs.column != rhs.column;
		}

		/// \brief	Returns true if the LHS position is before the RHS position.
		TOML_NODISCARD
		friend constexpr bool operator<(const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line < rhs.line || (lhs.line == rhs.line && lhs.column < rhs.column);
		}

		/// \brief	Returns true if the LHS position is before the RHS position or equal to it.
		TOML_NODISCARD
		friend constexpr bool operator<=(const source_position& lhs, const source_position& rhs) noexcept
		{
			return lhs.line < rhs.line || (lhs.line == rhs.line && lhs.column <= rhs.column);
		}
	};

	/// \brief	A source document region.
	///
	/// \detail \cpp
	/// auto tbl = toml::parse_file("config.toml"sv);
	/// if (auto server = tbl.get("server"))
	/// {
	///		std::cout << "begin: "sv << server->source().begin << "\n";
	///		std::cout << "end: "sv << server->source().end << "\n";
	///		std::cout << "path: "sv << *server->source().path << "\n";
	///	}
	///
	/// \ecpp
	///
	/// \out
	///	begin: line 3, column 1
	///	end: line 3, column 22
	///	path: config.toml
	/// \eout
	///
	/// \remarks toml++'s parser is unicode-aware insofar as it knows how to handle
	/// 		 various non-conventional whitespace and newline characters, but it doesn't give
	/// 		 much thought to combining marks, grapheme clusters vs. characters, et cetera.
	/// 		 If a TOML document contains lots of codepoints outside of the ASCII range
	/// 		 you may find that your source_positions don't match those given by a text editor
	/// 		 (typically the line numbers will be accurate but column numbers will be too high).
	/// 		 <strong>This is not an error.</strong> I've chosen this behaviour as a deliberate trade-off
	/// 		 between parser complexity and correctness.
	struct source_region
	{
		/// \brief The beginning of the region (inclusive).
		source_position begin;

		/// \brief The end of the region (exclusive).
		source_position end;

		/// \brief	The path to the corresponding source document.
		///
		/// \remarks This will be `nullptr` if no path was provided to toml::parse().
		source_path_ptr path;

#if TOML_WINDOWS_COMPAT

		/// \brief	The path to the corresponding source document as a wide-string.
		///
		/// \availability This function is only available when #TOML_WINDOWS_COMPAT is enabled.
		///
		/// \remarks This will return an empty optional if no path was provided to toml::parse().
		TOML_NODISCARD
		optional<std::wstring> wide_path() const noexcept
		{
			if (!path || path->empty())
				return {};
			return { impl::widen(*path) };
		}

#endif
	};

	TOML_ABI_NAMESPACE_END; // TOML_LARGE_FILES
}
TOML_NAMESPACE_END;

//#====================================================================================================================
//# OTHER
//#====================================================================================================================

TOML_NAMESPACE_START
{
	/// \brief Metadata associated with TOML values.
	enum class value_flags : uint8_t
	{
		/// \brief None.
		none,

		/// \brief Format integer values as binary.
		format_as_binary = 1,

		/// \brief Format integer values as octal.
		format_as_octal = 2,

		/// \brief Format integer values as hexadecimal.
		format_as_hexadecimal = 3,
	};
	TOML_MAKE_FLAGS(value_flags);

	/// \brief	Format flags for modifying how TOML data is printed to streams.
	enum class format_flags : uint8_t
	{
		/// \brief None.
		none,

		/// \brief Dates and times will be emitted as quoted strings.
		quote_dates_and_times = 1,

		/// \brief Strings will be emitted as single-quoted literal strings where possible.
		allow_literal_strings = 2,

		/// \brief Strings containing newlines will be emitted as triple-quoted 'multi-line' strings where possible.
		allow_multi_line_strings = 4,

		/// \brief Values with special format flags will be formatted accordingly.
		allow_value_format_flags = 8,
	};
	TOML_MAKE_FLAGS(format_flags);

	/// \brief	Pretty-prints the value of a node_type to a stream.
	///
	/// \detail \cpp
	/// auto arr = toml::array{ 1, 2.0, "3", false };
	/// for (size_t i = 0; i < arr.size() i++)
	/// 	std::cout << "Element ["sv << i << "] is: "sv << arr[i].type() << "\n";
	///
	/// \ecpp
	///
	/// \out
	/// Element [0] is: integer
	/// Element [1] is: floating-point
	/// Element [2] is: string
	/// Element [3] is: boolean
	/// \eout
	template <typename Char>
	inline std::basic_ostream<Char>& operator<<(std::basic_ostream<Char>& lhs, node_type rhs)
	{
		using underlying_t = std::underlying_type_t<node_type>;
		const auto str	   = impl::node_type_friendly_names[static_cast<underlying_t>(rhs)];
		if constexpr (std::is_same_v<Char, char>)
			return lhs << str;
		else
		{
			if constexpr (sizeof(Char) == 1)
				return lhs << std::basic_string_view<Char>{ reinterpret_cast<const Char*>(str.data()), str.length() };
			else
				return lhs << str.data();
		}
	}

	/// \cond
	namespace impl
	{
#define TOML_P2S_DECL(Type)                                                                                            \
	template <typename Char>                                                                                           \
	inline void print_to_stream(Type, std::basic_ostream<Char>&, value_flags = {})
		TOML_P2S_DECL(int8_t);
		TOML_P2S_DECL(int16_t);
		TOML_P2S_DECL(int32_t);
		TOML_P2S_DECL(int64_t);
		TOML_P2S_DECL(uint8_t);
		TOML_P2S_DECL(uint16_t);
		TOML_P2S_DECL(uint32_t);
		TOML_P2S_DECL(uint64_t);
#undef TOML_P2S_DECL

#define TOML_P2S_DECL(Type)                                                                                            \
	template <typename Char>                                                                                           \
	inline void print_to_stream(Type, std::basic_ostream<Char>&)
		TOML_P2S_DECL(double);
		TOML_P2S_DECL(const date&);
		TOML_P2S_DECL(const time&);
		TOML_P2S_DECL(time_offset);
		TOML_P2S_DECL(const date_time&);
#undef TOML_P2S_DECL
	}

#if !TOML_HEADER_ONLY
	extern template TOML_API
	std::ostream& operator<<(std::ostream&, node_type);
#endif // !TOML_HEADER_ONLY

	/// \endcond
}
TOML_NAMESPACE_END;
