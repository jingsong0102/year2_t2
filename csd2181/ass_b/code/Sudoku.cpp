/*!
@file Sudoku.cpp
@author Wei Jingsong (jingsong.wei@digipen.edu)
@SIT id 2200646
@course csd2183
@section A
@assignment Bonus Assignment
@date 3/28/2024
@brief This file contains the definition of the Sudoku class and its related classes and structures.
*/
#include "Sudoku.h"
/**
 * @brief Constructor for Sudoku puzzle solver.
 * @param basesize Size of the base Sudoku grid (e.g., 3 for a 9x9 grid)
 * @param stype Type of symbols used in the puzzle (numeric or alphabetical)
 * @param callback Callback function for reporting solving progress
 */
Sudoku::Sudoku(int basesize, SymbolType stype, SUDOKU_CALLBACK callback) : size_(basesize * basesize), stype_(stype), callback_(callback)
{
    // Allocate memory for the board
    board_ = new char[size_ * size_];
    stats_.basesize = basesize;
}
/**
 * @brief Destructor for Sudoku puzzle solver.
 */
Sudoku::~Sudoku()
{
    delete[] board_;
}
/**
 * @brief Set up the initial state of the Sudoku board.
 * @param values Array of characters representing the initial state of the board
 * @param size Size of the values array
 */
void Sudoku::SetupBoard(const char *values, size_t size)
{
    for (size_t i = 0; i < size; i++)
        board_[i] = values[i] == '.' ? EMPTY_CHAR : values[i];
}
/**
 * @brief Solve the Sudoku puzzle.
 */
void Sudoku::Solve()
{
    callback_(*this, board_, MSG_STARTING, stats_.moves, stats_.basesize, -1, 0);
    // Call a recursive place_value method to place 1 (or A) at the
    // starting point, index 0 (row 1, column 1) or whatever the first
    // empty cell is.
    if (place_value(0, 0))
        // Send MSG_FINISHED_OK if a solution was found, otherwise send
        // MSG_FINISHED_FAIL.
        callback_(*this, board_, MSG_FINISHED_OK, stats_.moves, stats_.basesize, -1, 0);
    else
        callback_(*this, board_, MSG_FINISHED_FAIL, stats_.moves, stats_.basesize, -1, 0);
}
/**
 * @brief Get the current state of the Sudoku board.
 * @return Pointer to the array representing the Sudoku board
 */
const char *Sudoku::GetBoard() const
{
    return board_;
}
/**
 * @brief Get the statistics of the Sudoku solving process.
 * @return SudokuStats structure containing solving statistics
 */
Sudoku::SudokuStats Sudoku::GetStats() const
{
    return stats_;
}
/**
 * @brief Recursive function to place a symbol on the Sudoku board.
 * @param col Column index
 * @param row Row index
 * @return True if the puzzle is solved, false otherwise
 */
bool Sudoku::place_value(int col, int row)
{
    // If row is equal to size_, then the board is full and we have a
    // solution.
    if (row == size_)
        return true;
    // If the cell is not empty, then move to the next cell.
    if (board_[row * size_ + col] != EMPTY_CHAR)
    {
        if (col == size_ - 1)
            return place_value(0, row + 1);
        else
            return place_value(col + 1, row);
    }
    char value = stype_ == SYM_NUMBER ? '1' : 'A';
    // Try to place a value in the cell.
    for (int i = 0; i < size_; i++)
    {
        if (callback_(*this, board_, MSG_ABORT_CHECK, stats_.moves, stats_.basesize, row * size_ + col, value))
        {
            return false;
        }
        board_[row * size_ + col] = value;
        stats_.moves++;
        stats_.placed++;
        callback_(*this, board_, MSG_PLACING, stats_.moves, stats_.basesize, row * size_ + col, value);
        // If the value is valid, then move to the next cell.
        if (is_valid_move(col, row, value))
        {
            if (col == size_ - 1)
            {
                if (place_value(0, row + 1))
                    return true;
            }
            else
            {
                if (place_value(col + 1, row))
                    return true;
            }
            board_[row * size_ + col] = EMPTY_CHAR;
            stats_.backtracks++;
            callback_(*this, board_, MSG_REMOVING, stats_.moves, stats_.basesize, row * size_ + col, value);
        }
        board_[row * size_ + col] = EMPTY_CHAR;
        stats_.placed--;
        callback_(*this, board_, MSG_REMOVING, stats_.moves, stats_.basesize, row * size_ + col, value);
        value++;
    }
    return false;
}
/**
 * @brief Check if placing a symbol in a given position is a valid move.
 * @param col Column index
 * @param row Row index
 * @param value Symbol to be placed
 * @return True if the move is valid, false otherwise
 */
bool Sudoku::is_valid_move(int col, int row, char value)
{
    // Check for duplicates in the row, column, and sub-grid.
    for (int i = 0; i < size_; i++)
    {
        if (i != col && board_[row * size_ + i] == value)
            return false;
        if (i != row && board_[i * size_ + col] == value)
            return false;
    }
    int max_row = stats_.basesize - 1;
    while (row > max_row)
    {
        max_row += stats_.basesize;
    }
    int max_col = stats_.basesize - 1;
    while (col > max_col)
    {
        max_col += stats_.basesize;
    }
    for (int i = max_row - stats_.basesize + 1; i <= max_row; i++)
    {
        for (int j = max_col - stats_.basesize + 1; j <= max_col; j++)
        {
            if (i == row || j == col)
            {
                continue;
            }
            if (board_[i * size_ + j] == value)
            {
                return false;
            }
        }
    }
    return true;
}