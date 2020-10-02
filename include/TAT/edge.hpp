/**
 * \file edge.hpp
 *
 * Copyright (C) 2019-2020 Hao Zhang<zh970205@mail.ustc.edu.cn>
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
#ifndef TAT_EDGE_HPP
#define TAT_EDGE_HPP

#include <map>
#include <set>

#include "basic_type.hpp"
#include "symmetry.hpp"

namespace TAT {
   struct no_edge_map {
      NoSymmetry first;
      Size second;
      no_edge_map() : second(0) {}
      no_edge_map(const std::initializer_list<std::pair<const NoSymmetry, Size>>& map) : second(map.begin()->second) {}
      no_edge_map(const std::map<NoSymmetry, Size>& map) : second(map.begin()->second) {}
      Size& at(const NoSymmetry&) {
         return second;
      }
      const Size& at(const NoSymmetry&) const {
         return second;
      }
      Size& operator[](const NoSymmetry&) {
         return second;
      }
      no_edge_map* begin() {
         return this;
      }
      const no_edge_map* begin() const {
         return this;
      }
      no_edge_map* end() {
         return this + 1;
      }
      const no_edge_map* end() const {
         return this + 1;
      }
      no_edge_map* find(const NoSymmetry&) {
         return this;
      }
      const no_edge_map* find(const NoSymmetry&) const {
         return this;
      }
      no_edge_map* erase(const NoSymmetry&) {
         return this;
      }
      std::pair<no_edge_map*, bool> insert(const std::tuple<NoSymmetry, Size>& pair) {
         second = std::get<1>(pair);
         return {this, true};
      }
      using iterator = no_edge_map*;
      using const_iterator = const no_edge_map*;
   };
   bool operator==(const no_edge_map& map_1, const no_edge_map& map_2) {
      return map_1.second == map_2.second;
   }

   /**
    * \see Edge
    */
   template<class Symmetry, bool is_pointer = false>
   struct BoseEdge {
      using symmetry_type = Symmetry;
#ifdef TAT_USE_SIMPLE_NOSYMMETRY_EDGE
      using edge_map = std::conditional_t<std::is_same_v<Symmetry, NoSymmetry>, no_edge_map, std::map<Symmetry, Size>>;
#else
      using edge_map = std::map<Symmetry, Size>;
#endif
      using map_type = std::conditional_t<is_pointer, const edge_map&, edge_map>;

      map_type map;

      BoseEdge() = default;
      BoseEdge(const BoseEdge&) = default;
      BoseEdge(BoseEdge&&) = default;
      BoseEdge& operator=(const BoseEdge&) = default;
      BoseEdge& operator=(BoseEdge&&) = default;
      ~BoseEdge() = default;

      /**
       * \brief 由对称性到维度的映射表直接构造
       */
      BoseEdge(edge_map&& map) : map(std::move(map)) {}
      BoseEdge(const edge_map& map) : map(map) {}
      BoseEdge(const std::initializer_list<std::pair<const Symmetry, Size>>& map) : map(map) {}

      /**
       * \brief 由一些对称性的集合构造, 意味着每一个对称性对应的维度都为1
       */
      BoseEdge(const std::set<Symmetry>& symmetries) {
         for (const auto& symmetry : symmetries) {
            map[symmetry] = 1;
         }
      }
      BoseEdge(const std::initializer_list<Symmetry>& symmetries) : BoseEdge(std::set<Symmetry>(symmetries)) {}

      /**
       * \brief 构造一个平凡的边, 仅含一个对称性
       */
      BoseEdge(const Size dimension) : map({{Symmetry(), dimension}}) {}
   };
   template<class Symmetry, bool is_pointer>
   bool operator==(const BoseEdge<Symmetry, is_pointer>& edge_1, const BoseEdge<Symmetry, is_pointer>& edge_2) {
      return edge_1.map == edge_2.map;
   }

   /**
    * \see Edge
    */
   template<class Symmetry, bool is_pointer = false>
   struct FermiEdge {
      using symmetry_type = Symmetry;
      using edge_map = std::map<Symmetry, Size>;
      using map_type = std::conditional_t<is_pointer, const edge_map&, edge_map>;

      /**
       * \brief 费米箭头方向
       * \note 当map中只含fermi=0的对称性值时, arrow无法定义,
       * 这在possible_reverse中得到体现
       * \see arrow_valid
       */
      Arrow arrow = false;
      map_type map = {};

      FermiEdge() = default;
      FermiEdge(const FermiEdge&) = default;
      FermiEdge(FermiEdge&&) = default;
      FermiEdge& operator=(const FermiEdge&) = default;
      FermiEdge& operator=(FermiEdge&&) = default;
      ~FermiEdge() = default;

      /**
       * \brief 由对称性到维度的映射表直接构造
       */
      FermiEdge(edge_map&& map) : map(std::move(map)) {}
      FermiEdge(const edge_map& map) : map(map) {}
      FermiEdge(const std::initializer_list<std::pair<const Symmetry, Size>>& map) : map(map) {}

      /**
       * \brief 由一些对称性的集合构造, 意味着每一个对称性对应的维度都为1
       */
      FermiEdge(const std::set<Symmetry>& symmetries) {
         for (const auto& symmetry : symmetries) {
            map[symmetry] = 1;
         }
      }
      FermiEdge(const std::initializer_list<Symmetry>& symmetries) : FermiEdge(std::set<Symmetry>(symmetries)) {}

      /**
       * \brief 构造一个平凡的边, 仅含一个对称性
       */
      FermiEdge(const Size dimension) : map({{Symmetry(), dimension}}) {}

      /**
       * \brief 由费米箭头方向和对称性到维度的映射表直接构造
       */
      FermiEdge(const Arrow arrow, edge_map&& map) : arrow(arrow), map(std::move(map)) {}
      FermiEdge(const Arrow arrow, const edge_map& map) : arrow(arrow), map(map) {}
      FermiEdge(const Arrow arrow, const std::initializer_list<std::pair<const Symmetry, Size>>& map) : arrow(arrow), map(map) {}

      /**
       * \brief 由费米子数自动构造箭头方向, 虽然这个不一定需要一致
       */
      void possible_reverse() {
         for (const auto& [symmetry, size] : map) {
            if (symmetry.fermi < 0) {
               arrow ^= true;
               return;
            }
         }
      }

      /**
       * \brief 检查箭头是否有效, 即含有非零的费米子数
       */
      [[nodiscard]] bool arrow_valid() const {
         for (const auto& [symmetry, size] : map) {
            if (symmetry.fermi != 0) {
               return true;
            }
         }
         return false;
      }
   };
   template<class Symmetry, bool is_pointer>
   bool operator==(const FermiEdge<Symmetry, is_pointer>& edge_1, const FermiEdge<Symmetry, is_pointer>& edge_2) {
      return edge_1.map == edge_2.map && edge_1.arrow == edge_2.arrow;
   }

   template<class Symmetry, bool is_pointer>
   using EdgeBase = std::conditional_t<is_fermi_symmetry_v<Symmetry>, FermiEdge<Symmetry, is_pointer>, BoseEdge<Symmetry, is_pointer>>;
   /**
    * \brief 张量的边的形状的类型, 是一个Symmetry到Size的映射表, 如果是费米对称性, 还会含有一个箭头方向
    * \tparam Symmetry 张量所拥有的对称性
    * \tparam is_pointer map是否为引用而非真是存储着数据的伪边
    * \see BoseEdge, FermiEdge
    */
   template<class Symmetry, bool is_pointer = false>
   struct Edge : EdgeBase<Symmetry, is_pointer> {
      using symmetry_valid = std::enable_if_t<is_symmetry_v<Symmetry>>;

      using EdgeBase<Symmetry, is_pointer>::EdgeBase;
   };

   /**
    * \brief 中间处理中常用到的数据类型, 类似Edge但是其中对称性值到子边长的映射表为指针
    * \see Edge
    */
   template<class Symmetry>
   using EdgePointer = Edge<Symmetry, true>;

   /**
    * \brief 对一个边的形状列表进行枚举分块, 并做一些其他操作
    * \tparam T 应是vector<Edge>或者vector<EdgePointer>的iterator
    * \param edges 即将要枚举的边列表的开头指针
    * \param rank 即将要枚举的边列表的大小
    * \param rank0 如果边列表为空，则调用rank0后返回
    * \param dims0 如果边列表中存在零维的边，则调用dims0后返回
    * \param operate 对枚举的每一个情况做操作
    * \note operate输入两个参数, 一个是每个边所在的位置列表, 一个是需要更新信息的位置开头, 并返回操作后需要更新的位置开头
    * \see initialize_block_symmetries_with_check, get_merged_edge
    */
   template<class T, class F1, class F2, class F3>
   void loop_edge(const T* edges, const Rank rank, F1&& rank0, F2&& dims0, F3&& operate) {
      if (rank == 0) {
         rank0();
         return;
      }
      using Symmetry = typename T::symmetry_type;
      using MapIteratorList = std::vector<typename T::edge_map::const_iterator>;
      auto symmetry_iterator_list = MapIteratorList();
      symmetry_iterator_list.reserve(rank);
      for (auto i = 0; i != rank; ++i) {
         const auto& map = edges[i].map;
         auto symmetry_iterator = map.begin();
         if (symmetry_iterator == map.end()) {
            dims0();
            return;
         }
         symmetry_iterator_list.push_back(symmetry_iterator);
      }
      Rank minimum_changed = 0;
      while (true) {
         minimum_changed = operate(symmetry_iterator_list, minimum_changed);
         auto edge_position = rank - 1;
         ++symmetry_iterator_list[edge_position];
         while (symmetry_iterator_list[edge_position] == edges[edge_position].map.end()) {
            if (edge_position == 0) {
               return;
            }
            symmetry_iterator_list[edge_position] = edges[edge_position].map.begin();
            --edge_position;
            ++symmetry_iterator_list[edge_position];
         }
         minimum_changed = minimum_changed < edge_position ? minimum_changed : edge_position;
      }
   }

   /**
    * \brief 根据边的形状的列表, 得到所有满足对称性条件的张量分块
    * \return 分块信息, 为一个vector, 元素为两个类型的tuple, 分别是子块的各个子边对称性值和子块的总大小
    * \tparam T 为vector<Edge>或者vector<EdgePointer>
    * \see loop_edge
    */
   template<class T>
   [[nodiscard]] auto initialize_block_symmetries_with_check(const T& edges) {
      using Symmetry = typename T::value_type::symmetry_type;
      using MapIteratorList = std::vector<typename T::value_type::edge_map::const_iterator>;
      Rank rank = edges.size();
      auto result = std::vector<std::tuple<std::vector<Symmetry>, Size>>();
      auto symmetries = std::vector<Symmetry>(rank);
      auto sizes = std::vector<Size>(rank);
      loop_edge(
            edges.data(),
            rank,
            [&result]() {
               result.push_back({std::vector<Symmetry>{}, 1});
            },
            []() {},
            [&](const MapIteratorList& symmetry_iterator_list, Rank minimum_changed) {
               auto symmetry_summary = Symmetry();
               for (const auto& symmetry_iterator : symmetry_iterator_list) {
                  symmetry_summary += symmetry_iterator->first;
               }
               if (symmetry_summary == Symmetry()) {
                  for (auto i = minimum_changed; i < rank; i++) {
                     symmetries[i] = symmetry_iterator_list[i]->first;
                     sizes[i] = symmetry_iterator_list[i]->second * (i ? sizes[i - 1] : 1);
                  }
                  result.push_back({symmetries, sizes.back()});
                  return rank;
               }
               return minimum_changed;
            });
      return result;
   }

   /**
    * \brief 判断一个类型是否为Edge类型, 这里不认为map为引用的Edge类型为Edge
    * \tparam T 如果T是Edge类型, 则value为true
    */
   template<class T>
   struct is_edge : std::bool_constant<false> {};
   template<class T>
   struct is_edge<Edge<T>> : std::bool_constant<true> {};
   template<class T>
   constexpr bool is_edge_v = is_edge<T>::value;
} // namespace TAT
#endif
