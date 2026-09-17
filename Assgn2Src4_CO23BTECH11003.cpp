#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>

using namespace std;
using namespace chrono;

int N;
bool is_valid = true;

void validate_rows(ofstream &output, const vector<vector<int>> &sudoku);
void validate_columns(ofstream &output, const vector<vector<int>> &sudoku);
void validate_subgrids(ofstream &output, const vector<vector<int>> &sudoku);

int main(int argc, char *argv[])
{
    ifstream file("inp.txt");
    if (!file.is_open())
    {
        cerr << "Error opening file." << endl;
        return -1;
    }

    int K;
    // omitted, reading just for uniformity
    if (!(file >> K))
    {
        cerr << "Invalid Sudoku size in file." << endl;
        file.close();
        return -1;
    }

    if (!(file >> N))
    {
        cerr << "Invalid Sudoku size in file." << endl;
        file.close();
        return -1;
    }

    // omitted, reading just for uniformity
    int taskInc;
    if (!(file >> taskInc))
    {
        cerr << "Invalid task increment in file." << endl;
        file.close();
        return -1;
    }

    vector<vector<int>> sudoku(N, vector<int>(N));

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (!(file >> sudoku[i][j]))
            {
                cerr << "Invalid Sudoku grid in file." << endl;
                file.close();
                return -1;
            }
        }
    }
    file.close();

    auto start_time = high_resolution_clock::now();

    ofstream output("outputseq.txt");

    validate_rows(output, sudoku);
    validate_columns(output, sudoku);
    validate_subgrids(output, sudoku);

    auto end_time = high_resolution_clock::now();
    auto time_taken = duration_cast<microseconds>(end_time - start_time).count();

    if (is_valid)
    {
        output << "Sudoku is valid." << endl;
    }
    else
    {
        output << "Sudoku is invalid." << endl;
    }
    output << "The total time taken is " << time_taken << " microseconds." << endl;

    output.close();
    return 0;
}

void validate_rows(ofstream &output, const vector<vector<int>> &sudoku)
{
    for (int i = 0; i < N; i++)
    {
        vector<bool> seen(N + 1, false);
        for (int j = 0; j < N; j++)
        {
            int value = sudoku[i][j];
            if (value > 0 && value <= N)
            {
                if (seen[value])
                {
                    is_valid = false;
                    return;
                }
                seen[value] = true;
            }
        }
    }
}

void validate_columns(ofstream &output, const vector<vector<int>> &sudoku)
{
    for (int i = 0; i < N; i++)
    {
        vector<bool> seen(N + 1, false);
        for (int j = 0; j < N; j++)
        {
            int value = sudoku[j][i];
            if (value > 0 && value <= N)
            {
                if (seen[value])
                {
                    is_valid = false;
                    return;
                }
                seen[value] = true;
            }
        }
    }
}

void validate_subgrids(ofstream &output, const vector<vector<int>> &sudoku)
{
    int n = static_cast<int>(sqrt(N));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            vector<bool> seen(N + 1, false);
            for (int r = 0; r < n; r++)
            {
                for (int c = 0; c < n; c++)
                {
                    int value = sudoku[i * n + r][j * n + c];
                    if (value > 0 && value <= N)
                    {
                        if (seen[value])
                        {
                            is_valid = false;
                            return;
                        }
                        seen[value] = true;
                    }
                }
            }
        }
    }
}