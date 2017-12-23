/*
 *  \remark This file is part of VITA.
 *
 *  \copyright Copyright (C) 2017 EOS di Manlio Morini.
 *
 *  \license
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 *
 *  \see https://github.com/morinim/vita/wiki/polyomino_tutorial
 */

#include <iostream>
#include <string>

#include "kernel/vita.h"
#include "utility/matrix.h"

const std::size_t board_height = 8;
const std::size_t board_width  = 8;

using shape = vita::matrix<int>;

std::vector<std::vector<shape>> piece_masks;

shape put(const shape &piece, const shape &base, std::size_t y, std::size_t x)
{
  if (y + piece.rows() > base.rows())
    return base;

  if (x + piece.cols() > base.cols())
    return base;

  shape ret(base);
  for (std::size_t row(0); row < piece.rows(); ++row)
    for (std::size_t col(0); col < piece.cols(); ++col)
      ret(y + row, x + col) += piece(row, col);

  return ret;
}

std::size_t add_piece_variants(const shape &piece)
{
  std::set<shape> ms;

  const shape empty(board_height, board_width);  // filled with `0`s

  for (unsigned reflection(0); reflection <= 1; ++reflection)
    for (unsigned rotation(0); rotation <= 3; ++rotation)
      for (unsigned y(0); y < board_height; ++y)
        for (unsigned x(0); x < board_width; ++x)
        {
          shape flipped(reflection ? vita::fliplr(piece) : piece);
          shape piece1(vita::rot90(flipped, rotation));

          shape piece_on_board(put(piece1, empty, y, x));
          if (piece_on_board != empty)
            ms.insert(piece_on_board);
        }

  piece_masks.emplace_back(ms.begin(), ms.end());

  return ms.size();
}

void fill_piece_masks()
{
  std::cout << "Calculating variants...";

  add_piece_variants({ {'A', 'A', 'A'},
                       { 0 , 'A',  0 } });

  add_piece_variants({ {'B', 'B', 'B'},
                       {'B',  0 , 'B'} });

  add_piece_variants({ {'C', 'C'},
                       {'C', 'C'} });

  add_piece_variants({ {'D',  0 , 'D'},
                       {'D', 'D', 'D'},
                       {'D',  0 , 'D'} });

  add_piece_variants({ { 0 , 'E',  0 },
                       {'E', 'E', 'E'},
                       {'E',  0 ,  0 } });

  add_piece_variants({ { 0 , 'F', 'F'},
                       { 0 , 'F',  0 },
                       {'F', 'F',  0 } });

  add_piece_variants({ { 0 , 'G'},
                       {'G', 'G'},
                       {'G',  0 } });

  add_piece_variants({ { 0 ,  0 , 'H', 'H'},
                       {'H', 'H', 'H',  0 } });

  add_piece_variants({ { 0 , 'I'},
                       { 0 , 'I'},
                       { 0 , 'I'},
                       {'I', 'I'} });

  add_piece_variants({ {'J', 'J', 'J'},
                       { 0 ,  0 , 'J'},
                       { 0 ,  0 , 'J'} });

  add_piece_variants({ {'K', 'K', 'K'},
                       {'K',  0 ,  0 } });

  add_piece_variants({ {'L', 'L', 'L'},
                       {'L', 'L', 'L'} });

  add_piece_variants({ { 0 , 'M',  0 ,  0 },
                       {'M', 'M', 'M', 'M'} });


  std::size_t sum(0);
  double product(1);
  for (const auto &x : piece_masks)
  {
    sum += x.size();
    product *= x.size();
  }

  std::cout << "ok (" << piece_masks.size() << " pieces, " << sum
            << " variants, search space " << product << ")\n";
}

void print_board(const shape &board)
{
  std::size_t i(0);

  for (const auto &e : board)
  {
    if (std::isprint(e))
      std::cout << char(e);
    else if (e == 0)
      std::cout << '.';
    else
      std::cout << '+';

    ++i;

    if (i % board.cols() == 0)
      std::cout << '\n';
    else
      std::cout << ' ';
  }

  std::cout << '\n';
}

int main()
{
  using namespace vita;

  fill_piece_masks();

  problem prob;
  prob.env.individuals = 500;
  prob.env.generations = 1000;

  for (std::size_t i(0); i < piece_masks.size(); ++i)
    prob.sset.insert(ga::parameter<ga::integer>(i, {0, piece_masks[i].size()}));

  // The fitness function.
  auto f = [](const i_ga &ind) -> fitness_t
  {
    shape board(board_height, board_width);

    for (std::size_t i(0); i < ind.size(); ++i)
      board += piece_masks[i][ind[i].as<int>()];

    double filled(std::count_if(board.begin(), board.end(),
                                [](unsigned v) { return v != 0; }));

    return {filled};
  };

  ga_search<i_ga, std_es, decltype(f)> search(prob, f);
  auto result = search.run(10);

  std::cout << "\nBest result:\n";
  std::cout << result.best.solution;
  std::cout << "\n  fitness " << result.best.score.fitness << "\n";

  shape board(board_height, board_width);
  for (std::size_t i(0); i < result.best.solution.size(); ++i)
    board += piece_masks[i][result.best.solution[i].as<int>()];
  print_board(board);
}
