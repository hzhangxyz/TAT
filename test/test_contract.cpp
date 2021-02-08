/**
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

#include <TAT/TAT.hpp>

#include "run_test.hpp"

void run_test() {
   auto a = TAT::Tensor<double, TAT::NoSymmetry>{{"A", "B"}, {2, 2}}.test();
   auto b = TAT::Tensor<double, TAT::NoSymmetry>{{"C", "D"}, {2, 2}}.test();
   std::cout << a << "\n";
   std::cout << b << "\n";
   std::cout << TAT::Tensor<double, TAT::NoSymmetry>::contract(a, b, {{"A", "C"}}) << "\n";
   std::cout << TAT::Tensor<double, TAT::NoSymmetry>::contract(a, b, {{"A", "D"}}) << "\n";
   std::cout << TAT::Tensor<double, TAT::NoSymmetry>::contract(a, b, {{"B", "C"}}) << "\n";
   std::cout << TAT::Tensor<double, TAT::NoSymmetry>::contract(a, b, {{"B", "D"}}) << "\n";
   std::cout << TAT::Tensor<double, TAT::NoSymmetry>::contract(
                      TAT::Tensor<double, TAT::NoSymmetry>{{"A", "B", "C", "D"}, {1, 2, 3, 4}}.test(),
                      TAT::Tensor<double, TAT::NoSymmetry>{{"E", "F", "G", "H"}, {3, 1, 2, 4}}.test(),
                      {{"B", "G"}, {"D", "H"}})
             << "\n";
   auto c =
         TAT::Tensor<double, TAT::FermiSymmetry>{
               {"A", "B", "C", "D"}, {{{-1, 1}, {0, 1}, {-2, 1}}, {{0, 1}, {1, 2}}, {{0, 2}, {1, 2}}, {{-2, 2}, {-1, 1}, {0, 2}}}, true}
               .test();
   auto d =
         TAT::Tensor<double, TAT::FermiSymmetry>{
               {"E", "F", "G", "H"}, {{{0, 2}, {1, 1}}, {{-2, 1}, {-1, 1}, {0, 2}}, {{0, 1}, {-1, 2}}, {{0, 2}, {1, 1}, {2, 2}}}, true}
               .test();
   std::cout << c << "\n";
   std::cout << d << "\n";
   std::cout << TAT::Tensor<double, TAT::FermiSymmetry>::contract(c, d, {{"B", "G"}, {"D", "H"}}) << "\n";
   std::cout << TAT::Tensor<double, TAT::FermiSymmetry>::contract(
                      c.transpose({"A", "C", "B", "D"}), d.transpose({"G", "H", "E", "F"}), {{"B", "G"}, {"D", "H"}})
             << "\n";
}
