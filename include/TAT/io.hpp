/**
 * \file io.hpp
 *
 * Copyright (C) 2019-2021 Hao Zhang<zh970205@mail.ustc.edu.cn>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#ifndef TAT_IO_HPP
#define TAT_IO_HPP

#include <iostream>
#include <limits>

#include "tensor.hpp"

namespace TAT {
   /**
    * \defgroup IO
    * @{
    */
   template<typename ScalarType>
   std::ostream& print_complex(std::ostream& out, const std::complex<ScalarType>& value) {
      if (value.real() != 0) {
         out << value.real();
         if (value.imag() != 0) {
            if (value.imag() > 0) {
               out << '+';
            }
            out << value.imag();
            out << 'i';
         }
      } else {
         if (value.imag() == 0) {
            out << '0';
         } else {
            out << value.imag();
            out << 'i';
         }
      }
      return out;
   }

   template<typename ScalarType>
   std::ostream&& print_complex(std::ostream&& out, const std::complex<ScalarType>& value) {
      print_complex(out, value);
      return std::move(out);
   }

   inline void ignore_until(std::istream& in, char end) {
      in.ignore(std::numeric_limits<std::streamsize>::max(), end);
   }

   template<typename ScalarType>
   std::istream& scan_complex(std::istream& in, std::complex<ScalarType>& value) {
      ScalarType part;
      in >> part;
      char maybe_i = in.peek();
      if (maybe_i == 'i') {
         in.get();
         // no real part
         value = std::complex<ScalarType>{0, part};
      } else {
         // have real part
         if (maybe_i == '+' || maybe_i == '-') {
            // have imag part
            ScalarType another_part;
            in >> another_part;
            value = std::complex<ScalarType>{part, another_part};
            if (in.get() != 'i') {
               in.setstate(std::ios::failbit);
            }
         } else {
            // no imag part
            value = std::complex<ScalarType>{part, 0};
         }
      }
      return in;
   }

   template<typename ScalarType>
   std::istream&& scan_complex(std::istream&& in, std::complex<ScalarType>& value) {
      scan_complex(in, value);
      return std::move(in);
   }

   template<typename T, typename = std::enable_if_t<std::is_trivially_destructible_v<T>>>
   std::ostream& operator<(std::ostream& out, const T& data) {
      out.write(reinterpret_cast<const char*>(&data), sizeof(T));
      return out;
   }
   template<typename T, typename = std::enable_if_t<std::is_trivially_destructible_v<T>>>
   std::istream& operator>(std::istream& in, T& data) {
      in.read(reinterpret_cast<char*>(&data), sizeof(T));
      return in;
   }

   // 如果Name = std::string则不能使用这个来输出
   // 而输入的话会重载std::string的输入问题不大
   // 对于二进制io在tensor处处理了问题也不大
   inline std::ostream& operator<<(std::ostream& out, const FastName& name) {
      return out << fastname_dataset.id_to_name[name.id];
   }

   inline bool valid_name_character(char c) {
      return ' ' < c && c < '\x7f' && c != ',' && c != '[' && c != ']';
      // 可打印字符去掉空格，逗号和方括号
   }

   // inline std::istream& operator>>(std::istream& in, std::string& name) {
   inline std::istream& scan_string_for_name(std::istream& in, std::string& name) {
      char buffer[256]; // max name length = 256
      Size length = 0;
      while (valid_name_character(in.peek())) {
         buffer[length++] = in.get();
      }
      buffer[length] = '\x00';
      name = (const char*)buffer;
      return in;
   }

   inline std::istream& scan_fastname_for_name(std::istream& in, FastName& name) {
      std::string string;
      scan_string_for_name(in, string);
      name = FastName(string);
      return in;
   }

   inline std::ostream& operator<(std::ostream& out, const std::string& string) {
      Size count = string.size();
      out < count;
      out.write(string.data(), sizeof(char) * count);
      return out;
   }
   inline std::istream& operator>(std::istream& in, std::string& string) {
      Size count;
      in > count;
      string.resize(count);
      in.read(string.data(), sizeof(char) * count);
      return in;
   }

#ifndef TAT_DOXYGEN_SHOULD_SKIP_THIS
   template<>
   struct NameTraits<FastName> {
      static constexpr name_out_operator_t<FastName> write = operator<;
      static constexpr name_in_operator_t<FastName> read = operator>;
      static constexpr name_out_operator_t<FastName> print = operator<<;
      static constexpr name_in_operator_t<FastName> scan = scan_fastname_for_name;
   };
   template<>
   struct NameTraits<std::string> {
      static constexpr name_out_operator_t<std::string> write = operator<;
      static constexpr name_in_operator_t<std::string> read = operator>;
      static constexpr name_out_operator_t<std::string> print = std::operator<<;
      static constexpr name_in_operator_t<std::string> scan = scan_string_for_name;
   };

   template<typename T>
   struct is_symmetry_vector : std::bool_constant<false> {};
   template<typename T>
   struct is_symmetry_vector<std::vector<T>> : is_symmetry<T> {};
   template<typename T>
   constexpr bool is_symmetry_vector_v = is_symmetry_vector<T>::value;
#endif

   template<typename T, typename A>
   std::ostream& operator<(std::ostream& out, const std::vector<T, A>& list) {
      Size count = list.size();
      out < count;
      if constexpr (std::is_trivially_destructible_v<T>) {
         out.write(reinterpret_cast<const char*>(list.data()), sizeof(T) * count);
      } else {
         for (const auto& i : list) {
            if constexpr (is_name_v<T>) {
               NameTraits<T>::write(out, i);
            } else {
               out < i;
            }
         }
      }
      return out;
   }
   template<typename T, typename A>
   std::istream& operator>(std::istream& in, std::vector<T, A>& list) {
      list.clear();
      Size count;
      in > count;
      if constexpr (std::is_trivially_destructible_v<T>) {
         list.resize(count);
         in.read(reinterpret_cast<char*>(list.data()), sizeof(T) * count);
      } else {
         for (Size i = 0; i < count; i++) {
            auto& item = list.emplace_back();
            if constexpr (is_name_v<T>) {
               NameTraits<T>::read(in, item);
            } else {
               in > item;
            }
         }
      }
      return in;
   }

   template<typename Key, typename Value, typename = std::enable_if_t<is_symmetry_v<Key> || is_symmetry_vector_v<Key>>>
   std::ostream& operator<(std::ostream& out, const std::map<Key, Value>& map) {
      Size size = map.size();
      out < size;
      for (const auto& [key, value] : map) {
         out < key < value;
      }
      return out;
   }

   template<typename Key, typename Value, typename = std::enable_if_t<is_symmetry_v<Key> || is_symmetry_vector_v<Key>>>
   std::istream& operator>(std::istream& in, std::map<Key, Value>& map) {
      map.clear();
      Size size;
      in > size;
      for (Size i = 0; i < size; i++) {
         Key key;
         in > key;
         in > map[std::move(key)];
      }
      return in;
   }

   template<typename T, typename A, typename = std::enable_if_t<is_scalar_v<T> || is_edge_v<T> || is_symmetry_v<T> || is_name_v<T>>>
   std::ostream& operator<<(std::ostream& out, const std::vector<T, A>& list) {
      out << '[';
      auto not_first = false;
      for (const auto& i : list) {
         if (not_first) {
            out << ',';
         }
         not_first = true;
         if constexpr (is_name_v<T>) {
            NameTraits<T>::print(out, i);
         } else if constexpr (std::is_same_v<T, std::complex<real_base_t<T>>>) {
            print_complex(out, i);
         } else {
            out << i;
         }
      }
      out << ']';
      return out;
   }

   template<typename T, typename A, typename = std::enable_if_t<is_scalar_v<T> || is_edge_v<T> || is_symmetry_v<T> || is_name_v<T>>>
   std::istream& operator>>(std::istream& in, std::vector<T, A>& list) {
      list.clear();
      ignore_until(in, '[');
      if (in.peek() == ']') {
         // empty list
         in.get(); // 获取']'
      } else {
         // not empty
         while (true) {
            // 此时没有space
            auto& i = list.emplace_back();
            if constexpr (is_name_v<T>) {
               NameTraits<T>::scan(in, i);
            } else if constexpr (std::is_same_v<T, std::complex<real_base_t<T>>>) {
               scan_complex(in, i);
            } else {
               in >> i;
            }
            char next = in.get();
            if (next == ']') {
               break;
            }
         }
      }
      return in;
   }

   template<typename Symmetry>
   std::ostream& operator<<(std::ostream& out, const Edge<Symmetry>& edge) {
      if constexpr (std::is_same_v<Symmetry, NoSymmetry>) {
         out << edge.map.at(NoSymmetry());
      } else {
         if constexpr (is_fermi_symmetry_v<Symmetry>) {
            out << '{' << "arrow" << ':';
            out << edge.arrow;
            out << ',' << "map" << ':';
         }
         out << '{';
         auto not_first = false;
         for (const auto& [symmetry, dimension] : edge.map) {
            if (not_first) {
               out << ',';
            }
            not_first = true;
            out << symmetry << ':' << dimension;
         }
         out << '}';
         if constexpr (is_fermi_symmetry_v<Symmetry>) {
            out << '}';
         }
      }
      return out;
   }
   template<typename Symmetry>
   std::istream& operator>>(std::istream& in, Edge<Symmetry>& edge) {
      if constexpr (std::is_same_v<Symmetry, NoSymmetry>) {
         in >> edge.map[NoSymmetry()];
      } else {
         if constexpr (is_fermi_symmetry_v<Symmetry>) {
            ignore_until(in, ':');
            in >> edge.arrow;
         }
         edge.map.clear();
         ignore_until(in, '{');
         if (in.peek() != '}') {
            // not empty
            do {
               Symmetry symmetry;
               in >> symmetry;
               ignore_until(in, ':');
               in >> edge.map[symmetry];
            } while (in.get() == ','); // 读了map最后的'}'
         } else {
            in.get(); // 读了map最后的'}'
         }
         if constexpr (is_fermi_symmetry_v<Symmetry>) {
            ignore_until(in, '}');
         }
      }
      return in;
   }

   template<typename Symmetry>
   std::ostream& operator<(std::ostream& out, const Edge<Symmetry>& edge) {
      if constexpr (std::is_same_v<Symmetry, NoSymmetry>) {
         out < edge.map.begin()->second;
      } else {
         if constexpr (is_fermi_symmetry_v<Symmetry>) {
            out < edge.arrow;
         }
         out < edge.map;
      }
      return out;
   }
   template<typename Symmetry>
   std::istream& operator>(std::istream& in, Edge<Symmetry>& edge) {
      if constexpr (std::is_same_v<Symmetry, NoSymmetry>) {
         // edge.map.clear(); 不需要
         in > edge.map[NoSymmetry()];
      } else {
         if constexpr (is_fermi_symmetry_v<Symmetry>) {
            in > edge.arrow;
         }
         in > edge.map;
      }
      return in;
   }

   template<typename Symmetry, std::size_t... Is>
   void print_symmetry_sequence(std::ostream& out, const Symmetry& symmetry, std::index_sequence<Is...>) {
      (((Is == 0 ? out : out << ',') << std::get<Is>(symmetry)), ...);
   }
   template<typename... T>
   std::ostream& operator<<(std::ostream& out, const symmetry_t<T...>& symmetry) {
      using Symmetry = symmetry_t<T...>;
      if constexpr (Symmetry::length != 0) {
         if constexpr (Symmetry::length == 1) {
            out << std::get<0>(symmetry);
         } else {
            out << '(';
            print_symmetry_sequence(out, symmetry, typename Symmetry::index_sequence());
            out << ')';
         }
      }
      return out;
   }
   template<typename Symmetry, std::size_t... Is>
   void scan_symmetry_sequence(std::istream& in, Symmetry& symmetry, std::index_sequence<Is...>) {
      (((Is == 0 ? in : (ignore_until(in, ','), in)) >> std::get<Is>(symmetry)), ...);
   }
   template<typename... T>
   std::istream& operator>>(std::istream& in, symmetry_t<T...>& symmetry) {
      using Symmetry = symmetry_t<T...>;
      if constexpr (Symmetry::length != 0) {
         if constexpr (Symmetry::length == 1) {
            in >> std::get<0>(symmetry);
         } else {
            ignore_until(in, '(');
            scan_symmetry_sequence(in, symmetry, typename Symmetry::index_sequence());
            ignore_until(in, ')');
         }
      }
      return in;
   }

   /**
    * 一个控制屏幕字体色彩的简单类型
    */
   struct UnixColorCode {
      std::string color_code;
      UnixColorCode(const char* code) noexcept : color_code(code) {}
   };
   inline const UnixColorCode console_red = "\x1B[31m";
   inline const UnixColorCode console_green = "\x1B[32m";
   inline const UnixColorCode console_yellow = "\x1B[33m";
   inline const UnixColorCode console_blue = "\x1B[34m";
   inline const UnixColorCode console_origin = "\x1B[0m";
   inline std::ostream& operator<<(std::ostream& out, const UnixColorCode& value) {
      out << value.color_code;
      return out;
   }

   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   std::ostream& operator<<(std::ostream& out, const TensorShape<ScalarType, Symmetry, Name, Allocator>& shape) {
      const auto& tensor = *shape.owner;
      out << '{' << console_green << "names" << console_origin << ':';
      out << tensor.names << ',';
      out << console_green << "edges" << console_origin << ':';
      out << tensor.core->edges << '}';
      return out;
   }
   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   std::ostream& operator<<(std::ostream& out, const Tensor<ScalarType, Symmetry, Name, Allocator>& tensor) {
      out << '{' << console_green << "names" << console_origin << ':';
      out << tensor.names << ',';
      out << console_green << "edges" << console_origin << ':';
      out << tensor.core->edges << ',';
      out << console_green << "blocks" << console_origin << ':';
      if constexpr (std::is_same_v<Symmetry, NoSymmetry>) {
         out << tensor.core->blocks.begin()->second;
      } else {
         out << '{';
         auto not_first = false;
         for (const auto& [symmetries, block] : tensor.core->blocks) {
            if (not_first) {
               out << ',';
            }
            not_first = true;
            out << console_yellow << symmetries << console_origin << ':' << block;
         }
         out << '}';
      }
      out << '}';
      return out;
   }
   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   std::istream& operator>>(std::istream& in, Tensor<ScalarType, Symmetry, Name, Allocator>& tensor) {
      ignore_until(in, ':');
      in >> tensor.names;
      tensor.name_to_index = construct_name_to_index<std::map<Name, Rank>>(tensor.names);
      ignore_until(in, ':');
      pmr::vector<Edge<Symmetry>> edges;
      in >> edges;
      tensor.core = std::make_shared<Core<ScalarType, Symmetry>>(std::move(edges));
      check_valid_name(tensor.names, tensor.core->edges.size());
      ignore_until(in, ':');
      if constexpr (std::is_same_v<Symmetry, NoSymmetry>) {
         // change begin();
         auto& block = tensor.core->blocks[std::vector<Symmetry>(tensor.names.size(), NoSymmetry())];
         block.clear(); // clear了vector，但是容量应该没有变
         in >> block;
      } else {
         // core是刚刚创建的所以不需要clear blocks
         ignore_until(in, '{');
         if (in.peek() != '}') {
            do {
               std::vector<Symmetry> symmetries;
               in >> symmetries;
               ignore_until(in, ':');
               auto& block = tensor.core->blocks[std::move(symmetries)];
               block.clear();
               in >> block;
            } while (in.get() == ','); // 读了map最后的'}'
         } else {
            in.get(); // 读了map最后的'}'
         }
      }
      ignore_until(in, '}');
      return in;
   }

   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   std::ostream& operator<<(std::ostream& out, const Singular<ScalarType, Symmetry, Name, Allocator>& singular) {
      const auto& value = singular.value; // std::map<Symmetry, vector<real_base_t<ScalarType>>>
      if constexpr (std::is_same_v<Symmetry, NoSymmetry>) {
         out << value.begin()->second;
      } else {
         out << '{';
         bool not_first = false;
         for (const auto& [key, value] : value) {
            if (not_first) {
               out << ',';
            } else {
               not_first = true;
            }
            out << console_yellow << key << console_origin << ':' << value;
         }
         out << '}';
      }
      return out;
   }

   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   std::istream& operator>>(std::istream& in, Singular<ScalarType, Symmetry, Name, Allocator>& singular) {
      auto& value = singular.value;
      if constexpr (std::is_same_v<Symmetry, NoSymmetry>) {
         in >> value[NoSymmetry()];
      } else {
         value.clear();
         ignore_until(in, '{');
         if (in.peek() != '}') {
            do {
               Symmetry symmetry;
               in >> symmetry;
               ignore_until(in, ':');
               in >> value[symmetry];
            } while (in.get() == ','); // 读了map最后的'}'
         } else {
            in.get(); // 读了map最后的'}'
         }
      }
      return in;
   }

   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   std::string Tensor<ScalarType, Symmetry, Name, Allocator>::show() const {
      std::ostringstream out;
      out << *this;
      return out.str();
   }

   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   std::string Singular<ScalarType, Symmetry, Name, Allocator>::show() const {
      std::ostringstream out;
      out << *this;
      return out.str();
   }

   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   const Tensor<ScalarType, Symmetry, Name, Allocator>& Tensor<ScalarType, Symmetry, Name, Allocator>::meta_put(std::ostream& out) const {
      out < names;
      out < core->edges;
      return *this;
   }

   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   const Tensor<ScalarType, Symmetry, Name, Allocator>& Tensor<ScalarType, Symmetry, Name, Allocator>::data_put(std::ostream& out) const {
      out < core->storage;
      return *this;
   }

   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   std::ostream& operator<(std::ostream& out, const Tensor<ScalarType, Symmetry, Name, Allocator>& tensor) {
      tensor.meta_put(out).data_put(out);
      return out;
   }

   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   std::string Tensor<ScalarType, Symmetry, Name, Allocator>::dump() const {
      std::ostringstream out;
      out < *this;
      return out.str();
   }

   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   std::ostream& operator<(std::ostream& out, const Singular<ScalarType, Symmetry, Name, Allocator>& singular) {
      if constexpr (std::is_same_v<Symmetry, NoSymmetry>) {
         out < singular.value.begin()->second;
      } else {
         out < singular.value;
      }
      return out;
   }

   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   std::string Singular<ScalarType, Symmetry, Name, Allocator>::dump() const {
      std::ostringstream out;
      out < *this;
      return out.str();
   }

   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   Tensor<ScalarType, Symmetry, Name, Allocator>& Tensor<ScalarType, Symmetry, Name, Allocator>::meta_get(std::istream& in) {
      in > names;
      name_to_index = construct_name_to_index<std::map<Name, Rank>>(names);
      pmr::vector<Edge<Symmetry>> edges;
      in > edges;
      core = std::make_shared<Core<ScalarType, Symmetry>>(std::move(edges));
      check_valid_name(names, core->edges.size());
      return *this;
   }

   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   Tensor<ScalarType, Symmetry, Name, Allocator>& Tensor<ScalarType, Symmetry, Name, Allocator>::data_get(std::istream& in) {
      in > core->storage;
      return *this;
   }

   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   std::istream& operator>(std::istream& in, Tensor<ScalarType, Symmetry, Name, Allocator>& tensor) {
      tensor.meta_get(in).data_get(in);
      return in;
   }

   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   Tensor<ScalarType, Symmetry, Name, Allocator>& Tensor<ScalarType, Symmetry, Name, Allocator>::load(const std::string& input) & {
      std::istringstream in(input);
      in > *this;
      return *this;
   }

   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   std::istream& operator>(std::istream& in, Singular<ScalarType, Symmetry, Name, Allocator>& singular) {
      if constexpr (std::is_same_v<Symmetry, NoSymmetry>) {
         // singular.value.clear(); 不需要
         in > singular.value[NoSymmetry()];
      } else {
         in > singular.value;
      }
      return in;
   }

   template<typename ScalarType, typename Symmetry, typename Name, template<typename> class Allocator>
   Singular<ScalarType, Symmetry, Name, Allocator>& Singular<ScalarType, Symmetry, Name, Allocator>::load(const std::string& input) & {
      std::istringstream in(input);
      in > *this;
      return *this;
   }

   inline std::ostream& operator<(std::ostream& out, const fastname_dataset_t& dataset) {
      return out < dataset.id_to_name;
   }
   inline std::istream& operator>(std::istream& in, fastname_dataset_t& dataset) {
      in > dataset.id_to_name;
      dataset.fastname_number = dataset.id_to_name.size();
      dataset.name_to_id.clear();
      for (auto i = 0; i < dataset.fastname_number; i++) {
         dataset.name_to_id[dataset.id_to_name[i]] = i;
      }
      return in;
   }
   inline void load_fastname_dataset(const std::string& input) {
      std::istringstream in(input);
      in > fastname_dataset;
   }
   inline std::string dump_fastname_dataset() {
      std::ostringstream out;
      out < fastname_dataset;
      return out.str();
   }

   template<typename T>
   std::istream&& operator>(std::istream&& in, T& v) {
      in > v;
      return std::move(in);
   }
   template<typename T>
   std::ostream&& operator<(std::ostream&& out, const T& v) {
      out < v;
      return std::move(out);
   }
   /**@}*/
} // namespace TAT
#endif
