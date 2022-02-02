#include <iostream>
#include <vector>
#include <array>
#include <math.h>
#include <optional>
#include <algorithm>
#include <string>
#include <sstream>

auto& cout = std::cout;
const auto endl = "\n";

const auto& empty = '\0';
const auto& top = '_';
const auto& left = '|';

struct Square
{
    size_t number;
    uint8_t borders;
    char owner;

    Square(size_t number,uint8_t borders, char owner): 
    number(number), borders(borders), owner(owner) {}
};


struct Position
{
    uint8_t row, col;
};

const auto num_sides = 5;
const auto num_side_dots = num_sides + 1;
const long total_squares = pow(num_sides, 2);
const long total_dots = pow(num_side_dots, 2);
const auto total_sides = 2 * num_side_dots * (num_side_dots - 1);
std::vector<Square> squares;
std::vector<bool> sides;
std::vector<char> players {'X', 'O'};

Position normalize_position(const Position& position, const bool vertical) {
    const auto last_col = num_sides + (vertical ? 0 : 1);
    auto col = position.col;
    if (!vertical && col == last_col) {
        col -= 1;
    }
    return {.row= position.row, .col= col};
}

std::optional<Position> get_bare_position_for_side(size_t side_number, bool vertical) {
    if (vertical) {
        const auto last_border_row_start = total_sides - num_sides + 1;
        if (side_number >= last_border_row_start) {
            return Position{.row = num_sides, .col = (uint8_t)(num_sides - (total_sides - side_number))};
        } else if (side_number <= num_sides) {
            return Position{.row = 1, .col = (uint8_t)side_number};
        }
    }

    const auto last_col = num_sides + (vertical ? 0 : 1);

    const auto den = 2 * num_sides + 1;
    const auto z = vertical ? 0 : num_sides;
    const int m = round((side_number - z) / (float)den);
    const auto col = side_number - z - m*den;
    if (m >= num_sides || col > last_col || col <= 0) {
        return {};
    }
    const auto r = m + 1;
    return Position{.row = (uint8_t)r, .col = (uint8_t)col};
}

std::optional<Position> get_position_for_side(size_t number, bool vertical) {
    const auto position = get_bare_position_for_side(number, vertical);
    if (position) {
        return normalize_position(*position, vertical);
    } else {
        return position;
    }
}

size_t get_square_number(const Position& position) {
    return (position.row - 1) * num_sides + position.col;
}

std::optional<Position> get_position(size_t number) {
    if (number > total_squares) {
        return {};
    }
    const uint8_t row = floor(number / (float)num_sides) + 1;
    const uint8_t col = number % num_sides;
    return Position{.row = (uint8_t)(col == 0 ? (row - 1) : row), 
                    .col = (uint8_t)(col == 0 ? num_sides : col)};
}

std::array<size_t, 4> get_sides_for_pos(const Position& position) {
    const size_t top = num_sides * (position.row-1) + num_side_dots * (position.row - 1) + position.col;
    const size_t down = num_sides * (position.row) + num_side_dots * (position.row) + position.col;
    const size_t left = top + num_sides;
    const size_t right = left + 1;
    return {top, right, down, left};
}

std::vector<size_t> get_squares_for_side(size_t side_number) {
    std::vector<size_t> result;
    result.reserve(2);

    auto v = true;
    for (size_t i = 0; i < 2; i++)
    {
        const auto first_pos = get_bare_position_for_side(side_number, v);
        if (first_pos) {
            const auto first = get_square_number(*first_pos);
            if (v) {
                result.push_back(first);
                if (!(first <= num_sides || first > (total_squares - num_sides))) {
                    const auto second = first - num_sides;
                    result.push_back(second);
                }
            } else {
                const auto last_col = num_sides + 1;
                if (first_pos->col == last_col) {
                    const auto pos = normalize_position(*first_pos, v);
                    const auto pos_num = get_square_number(pos);
                    result.push_back(pos_num);
                } else {
                    result.push_back(first);
                    if ((first -1) % num_sides != 0) {
                        const auto second = first - 1;
                        result.push_back(second);
                    }
                } 
            }
            break;
        }
        v = !v;
    }
    std::sort(result.begin(), result.end());
    return result;
}

std::optional<uint8_t> draw_side(size_t number, char player) {
    const auto side = sides[number -1];
    if (!side) {
        sides[number - 1] = true;
        const auto boxes = get_squares_for_side(number);
        for (auto &number : boxes) {
            auto& square = squares[number-1];
            square.borders += 1;
            if (square.borders >= 4) {
                square.owner = player;
            }
        }
        return (uint8_t)boxes.size();
    }
    return {};
}

std::string represent_board() {
    std::stringstream ss;
    size_t start = 1;
    for (size_t i = start; i <= num_sides*2+1; i++) {
        const auto is_vertical = i % 2 == 1;
        const auto end = start + num_sides;
        for (size_t b = start; b < end; b++) {
            const auto side = sides[b -1];
            if (is_vertical) {
                ss << ".";
                if (side) {
                    ss << top;
                } else {
                    ss << " ";
                }
            } else {
                if (side) {
                    ss << left;
                } else {
                    ss << " ";
                }
                const auto boxes = get_squares_for_side(b);
                const auto last = boxes.rbegin();
                const auto last_box = squares[*last -1];
                if (last_box.owner != empty) {
                    ss << last_box.owner;
                } else {
                    ss << " ";
                }
            }
        }
        if (is_vertical) {
            ss << ".";
        } else {
            const auto side = sides[end -1];
            if (side) {
                ss << left;
            } else {
                ss << " ";
            }
        }
        ss << endl;
        start = end + (is_vertical ? 0 : 1);
    }
    return ss.str();
}

int main(const int argc, const char** argv) {
    squares.reserve(total_squares);
    for (size_t i = 1; i <= total_squares; i++)
    {
        squares.emplace_back(i, (uint8_t)0, *const_cast<char*>(&empty));
    }
    
    sides.assign(total_sides, false);

    draw_side(1, 'X');
    draw_side(6, 'X');
    draw_side(7, 'X');
    draw_side(12, 'X');
    draw_side(11, 'X');
    draw_side(20, 'X');
    cout << represent_board();

    return 0;
}