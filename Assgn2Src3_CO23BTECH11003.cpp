#include <iostream>
#include <fstream>
#include <vector>
#include <atomic>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <pthread.h>

using namespace std;
using namespace chrono;

using Clock = chrono::high_resolution_clock;

int k, n, taskInc;
vector<vector<int>> matrix;
atomic<int> counter(0);
atomic<bool> flag(false);
ofstream outFile("outputboundcas.txt");

atomic<int> resultsIndex(0);       // Index for results vector
vector<pair<int, string>> results; // Store results as taskNumber, resultString

atomic<int> timesIndex(0);       // Index for enterTimes and exitTimes vectors
vector<microseconds> enterTimes; // Store enter times in microseconds
vector<nanoseconds> exitTimes;   // Store exit times in nanoseconds

string formatTime(const Clock::time_point &timePoint)
{
    auto time = Clock::to_time_t(timePoint);
    auto tm = *localtime(&time);
    auto microseconds = duration_cast<chrono::microseconds>(timePoint.time_since_epoch()) % 1000000;

    stringstream ss;
    ss << put_time(&tm, "%H:%M:%S") << "." << setw(6) << setfill('0') << microseconds.count();
    return ss.str();
}

// Bounded CAS acquire lock
void acquireLockBoundedCAS()
{
    bool expected = false;
    int retries = 0;
    const int maxRetries = 10; // Maximum number of retries before yielding

    while (true)
    {
        if (flag.compare_exchange_weak(expected, true, memory_order_acquire))
        {
            break; // Lock acquired
        }
        expected = false;

        retries++;
        if (retries >= maxRetries)
        {
            sched_yield(); // Yield to avoid busy-waiting
            retries = 0;
        }
    }
}

// CAS-based lock release
void releaseLockCAS()
{
    flag.store(false, memory_order_release);
}

bool check_row(int row)
{
    vector<bool> seen(n + 1, false);
    for (int j = 0; j < n; j++)
    {
        int value = matrix[row][j];
        if (value > 0 && value <= n)
        {
            if (seen[value])
                return false;
            seen[value] = true;
        }
    }
    return true;
}

bool check_column(int column)
{
    vector<bool> seen(n + 1, false);
    for (int j = 0; j < n; j++)
    {
        int value = matrix[j][column];
        if (value > 0 && value <= n)
        {
            if (seen[value])
                return false;
            seen[value] = true;
        }
    }
    return true;
}

bool check_subgrid(int startrow, int startcol, int size)
{
    vector<bool> seen(n + 1, false);
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            int value = matrix[startrow + i][startcol + j];
            if (value > 0 && value <= n)
            {
                if (seen[value])
                    return false;
                seen[value] = true;
            }
        }
    }
    return true;
}

void *workerfunction(void *arg)
{
    int threadId = *(int *)arg;
    stringstream buffer; // Local buffer for thread output

    while (true)
    {
        auto requestTime = Clock::now();

        // Log request to enter CS
        buffer << "Thread " << threadId << " requests to enter CS1 at "
               << formatTime(requestTime) << " hrs\n";
        auto enterTime = Clock::now();
        // Enter Critical Section
        acquireLockBoundedCAS();

        int task = counter.fetch_add(taskInc); // Task incrementing
        releaseLockCAS();
        auto leaveTime = Clock::now();
        // Exit Critical Section (CS)

        if (task >= 3 * n)
            break;

        // Log entry into CS
        buffer << "Thread " << threadId << " enters CS1 at "
               << formatTime(enterTime) << " hrs\n";

        // Log grabbing row/column/subgrid
        if (task < n)
        {
            buffer << "Thread " << threadId << " grabs row " << task + 1 << " at "
                   << formatTime(enterTime) << " hrs\n";
        }
        else if (task < 2 * n)
        {
            buffer << "Thread " << threadId << " grabs column " << task - n + 1 << " at "
                   << formatTime(enterTime) << " hrs\n";
        }
        else
        {
            int subgrid = sqrt(n);
            int gridrow = (task - 2 * n) / subgrid;
            int gridcol = (task - 2 * n) % subgrid;
            buffer << "Thread " << threadId << " grabs subgrid " << gridrow + 1 << "," << gridcol + 1 << " at "
                   << formatTime(enterTime) << " hrs\n";
        }

        // Log exit from CS
        buffer << "Thread " << threadId << " leaves CS1 at "
               << formatTime(leaveTime) << " hrs\n";

        // Perform task validation (outside CS)
        bool valid = true;
        if (task < n)
        {
            valid = check_row(task);
        }
        else if (task < 2 * n)
        {
            valid = check_column(task - n);
        }
        else
        {
            int subgrid = sqrt(n);
            int gridrow = (task - 2 * n) / subgrid;
            int gridcol = (task - 2 * n) % subgrid;
            valid = check_subgrid(gridrow * subgrid, gridcol * subgrid, subgrid);
        }

        auto completeTime = Clock::now();
        buffer << "Thread " << threadId << " completes checking of "
               << (task < n ? "row " : (task < 2 * n ? "column " : "subgrid "))
               << (task < n ? to_string(task + 1) : (task < 2 * n ? to_string(task - n + 1) : to_string(static_cast<int>((task - 2 * n) / sqrt(n)) + 1)))
               << " at " << formatTime(completeTime)
               << " hrs and finds it as " << (valid ? "valid." : "invalid.") << "\n";

        int index = resultsIndex.fetch_add(1);
        results[index] = make_pair(task, valid ? "valid" : "invalid");

        int timeIndex = timesIndex.fetch_add(1);
        enterTimes[timeIndex] = duration_cast<microseconds>(enterTime - requestTime);
        exitTimes[timeIndex] = duration_cast<nanoseconds>(leaveTime - enterTime);
    }

    acquireLockBoundedCAS();
    outFile << buffer.rdbuf();
    releaseLockCAS();

    pthread_exit(NULL);
    return NULL;
}

int main()
{
    ifstream inFile("inp.txt");
    inFile >> k >> n >> taskInc;
    matrix.resize(n, vector<int>(n));

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            inFile >> matrix[i][j];

    results.resize(3 * n);
    enterTimes.resize(3 * n);
    exitTimes.resize(3 * n);

    pthread_t threads[k];
    int threadIds[k];

    for (int i = 0; i < k; i++)
    {
        threadIds[i] = i;
        pthread_create(&threads[i], NULL, workerfunction, &threadIds[i]);
    }

    for (int i = 0; i < k; i++)
    {
        pthread_join(threads[i], NULL);
    }

    bool sudokuValid = true;
    for (const auto &[task, result] : results)
    {
        if (result == "invalid")
        {
            sudokuValid = false;
            break;
        }
    }

    outFile << "Sudoku is " << (sudokuValid ? "valid." : "invalid.") << "\n";

    // Calculate average and worst-case times
    microseconds totalEnterTime(0);
    nanoseconds totalExitTime(0);
    microseconds worstEnterTime(0);
    nanoseconds worstExitTime(0);

    for (const auto &time : enterTimes)
    {
        totalEnterTime += time;
        if (time > worstEnterTime)
            worstEnterTime = time;
    }

    for (const auto &time : exitTimes)
    {
        totalExitTime += time;
        if (time > worstExitTime)
            worstExitTime = time;
    }

    double avgEnterTime = enterTimes.empty() ? 0 : static_cast<double>(totalEnterTime.count()) / enterTimes.size();
    double avgExitTime = exitTimes.empty() ? 0 : static_cast<double>(totalExitTime.count()) / exitTimes.size();

    outFile << fixed << setprecision(6);
    outFile << "The total time taken is " << (totalEnterTime + duration_cast<microseconds>(totalExitTime)).count() << " microseconds.\n";
    outFile << "Average time taken by a thread to enter the CS is " << avgEnterTime << " microseconds\n";
    outFile << "Average time taken by a thread to exit the CS is " << avgExitTime << " nanoseconds\n";
    outFile << "Worst-case time taken by a thread to enter the CS is " << worstEnterTime.count() << " microseconds\n";
    outFile << "Worst-case time taken by a thread to exit the CS is " << worstExitTime.count() << " nanoseconds\n";

    outFile.close();
    cout << "All threads completed.\n";
    return 0;
}