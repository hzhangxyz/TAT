/**
 * \file propagate_const.hpp
 *
 * Copyright (C) 2021 Hao Zhang<zh970205@mail.ustc.edu.cn>
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
#ifndef TAT_PROPAGATE_CONST_HPP
#define TAT_PROPAGATE_CONST_HPP

#include <memory>

namespace TAT {
   // 实现类似<experimental/propagate_const>的东西
   // 为了简单, propagate_const_shared_ptr<T> 相当于 propagate_const<shared_ptr<T>>
   template<typename T>
   struct propagate_const_shared_ptr : std::shared_ptr<T> {
      using std::shared_ptr<T>::shared_ptr;

      propagate_const_shared_ptr(const std::shared_ptr<T>& p) noexcept : std::shared_ptr<T>(p) {}
      propagate_const_shared_ptr(std::shared_ptr<T>&& p) noexcept : std::shared_ptr<T>(std::move(p)) {}

      const T& operator*() const noexcept {
         return *this->get();
      }
      T& operator*() noexcept {
         return *this->get();
      }

      const T* operator->() const noexcept {
         return this->get();
      }
      T* operator->() noexcept {
         return this->get();
      }
   };
} // namespace TAT
#endif
