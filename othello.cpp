#include <iostream>
#include <unistd.h>
#include <utility>
#include <vector>

class Board {
public:
  Board() {
    m_board[3][3] = m_board[4][4] = -1; // White
    m_board[3][4] = m_board[4][3] = 1;  // Black
  }

  std::vector<std::pair<int, int>> get_puttable_positions(int player) const {
    std::vector<std::pair<int, int>> puttable_positions;
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        if (m_board[row][col] != 0) {
          continue;
        }
        if (!has_adjacent_stone(row, col)) {
          continue;
        }
        std::vector<std::pair<int, int>> reversible_stones = get_reversible_stones(row, col, player);
        if (reversible_stones.empty()) {
          continue;
        }
        puttable_positions.push_back(std::make_pair(row, col));
      }
    }
    return puttable_positions;
  }

  void put_stone(int row, int col, int player) {
    std::vector<std::pair<int, int>> reversible_stones = get_reversible_stones(row, col, player);
    m_board[row][col] = player;
    for (auto stone : reversible_stones) {
      m_board[stone.first][stone.second] = player;
    }
  }

  void display() const {
    write(1, "\E[H\E[2J", 7); // clear console
    std::cout << "  0 1 2 3 4 5 6 7\n";
    for (int row = 0; row < 8; row++) {
      std::cout << row << " ";
      for (int col = 0; col < 8; col++) {
        if (m_board[row][col] == -1) {
          std::cout << "○";
        } else if (m_board[row][col] == 1) {
          std::cout << "●";
        } else {
          std::cout << "・";
        }
      }
      std::cout << "\n";
    }
  }

private:
  int m_board[8][8] = {{0}};

  bool has_adjacent_stone(int row, int col) const {
    for (int r = -1; r <= 1; r++) {
      for (int c = -1; c <= 1; c++) {
        if (r == 0 && c == 0) {
          continue;
        }
        int new_row = row + r;
        int new_col = col + c;
        if (new_row < 0 || new_row > 7 || new_col < 0 || new_col > 7) {
          continue;
        }
        if (m_board[new_row][new_col] != 0) {
          return true;
        }
      }
    }
    return false;
  }

  std::vector<std::pair<int, int>> get_reversible_stones(int row, int col, int player) const {
    std::vector<std::pair<int, int>> reversible_stones;
    for (int r = -1; r <= 1; r++) {
      for (int c = -1; c <= 1; c++) {
        if (r == 0 && c == 0) {
          continue;
        }
        int new_row = row + r;
        int new_col = col + c;
        int reversible_count = 0;
        while (new_row >= 0 && new_row <= 7 && new_col >= 0 && new_col <= 7) {
          if (m_board[new_row][new_col] == -player) {
            reversible_count++;
          } else if (m_board[new_row][new_col] == player) {
            if (reversible_count > 0) {
              for (int i = 1; i <= reversible_count; i++) {
                reversible_stones.push_back(std::make_pair(row + r * i, col + c * i));
              }
            }
            break;
          } else {
            break;
          }
          new_row += r;
          new_col += c;
        }
      }
    }
    return reversible_stones;
  }
};

int main() {
  Board board;

  int current_player = 1;
  while (true) {
    // Get stone positions
    std::vector<std::pair<int, int>> puttable_positions = board.get_puttable_positions(current_player);
    if (puttable_positions.empty()) {
      current_player = -current_player;
      puttable_positions = board.get_puttable_positions(current_player);
      if (puttable_positions.empty()) {
        board.display();
        std::cout << "Both players cannot put a stone. Game over." << std::endl;
        break;
      }
    }

    // Display board
    board.display();
    std::cout << (current_player == 1 ? "Black" : "White") << " player's turn." << std::endl;
    std::cout << "Puttable positions: ";
    for (auto position : puttable_positions) {
      std::cout << " (" << position.first << ", " << position.second << ")";
    }

    // Input positions
    std::cout << std::endl;
    int row, col;
    std::string position;
    std::cout << "Input row and col: ";
    while (true) {
      std::cin >> position;
      if (position.length() != 2) {
        std::cout << "Invalid position. Please input again: ";
        continue;
      }
      row = position[0] - '0';
      col = position[1] - '0';
      bool is_valid_position = false;
      for (auto position : puttable_positions) {
        if (position.first == row && position.second == col) {
          is_valid_position = true;
          break;
        }
      }
      if (!is_valid_position) {
        std::cout << "Invalid position. Please input again: ";
        continue;
      }
      break;
    }

    // Update stones
    board.put_stone(row, col, current_player);
    current_player = -current_player;
  }

  return 0;
}
