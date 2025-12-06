#include <iostream>
#include <queue>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <string>
#include <climits>

using namespace std;

/**
 * @struct Process
 * @brief Represents a simulated process with scheduling-related parameters.
 */
struct Process
{
    int pid;             
    int arrivalTime;     
    int burstTime;       
    int startTime;       
    int finishTime;      
    int waitingTime;     
    int turnaroundTime;  
    int responseTime;    

    /**
     * @brief Default constructor initializing all values to zero.
     */
    Process()
        : pid(0), arrivalTime(0), burstTime(0), startTime(0),
          finishTime(0), waitingTime(0), turnaroundTime(0), responseTime(0)
    {}

    /**
     * @brief Parameterized constructor.
     * @param pid Process ID
     * @param arrivalTime Arrival time
     * @param burstTime CPU burst length
     */
    Process(int pid, int arrivalTime, int burstTime)
        : pid(pid), arrivalTime(arrivalTime), burstTime(burstTime),
          startTime(0), finishTime(0), waitingTime(0),
          turnaroundTime(0), responseTime(0)
    {}
};

/**
 * @brief Reads process data from a text file.
 *
 * The input file must contain two integers per line: arrival time and burst time.
 * The first line is assumed to be a header and is discarded.
 *
 * @param processes Vector to store loaded processes.
 * @param filename Input file path.
 */
void readProcess(vector<Process> &processes, const string &filename)
{
    ifstream inputFile(filename);
    if (!inputFile)
    {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    string dummyLine;
    getline(inputFile, dummyLine);  // Discard header line

    int id = 0, arrivalTime, burstTime;

    while (inputFile >> arrivalTime >> burstTime)
    {
        processes.emplace_back(id, arrivalTime, burstTime);
        id++;
    }

    inputFile.close();
}

/**
 * @brief Simulates FIFO (First-In, First-Out) scheduling.
 *
 * @param processes Queue of processes in arrival order.
 * @return Queue of scheduled processes with computed statistics.
 */
queue<Process> fifoAlgorithm(queue<Process> &processes)
{
    queue<Process> scheduled;
    int currentTime = 0;

    while (!processes.empty())
    {
        Process p = processes.front();
        processes.pop();

        if (p.arrivalTime > currentTime)
            currentTime = p.arrivalTime;

        p.startTime = currentTime;
        p.waitingTime = p.startTime - p.arrivalTime;
        p.responseTime = p.startTime - p.arrivalTime;
        p.finishTime = currentTime + p.burstTime;
        p.turnaroundTime = p.finishTime - p.arrivalTime;

        currentTime += p.burstTime;

        scheduled.push(p);
    }

    return scheduled;
}

/**
 * @brief Simulates non-preemptive Shortest Job First (SJF) scheduling.
 *
 * The algorithm selects the shortest available burst among processes
 * that have already arrived.
 *
 * @param processesVec Vector of all processes.
 * @return Queue of scheduled processes with computed statistics.
 */
queue<Process> sjfAlgorithm(vector<Process> processesVec)
{
    int n = processesVec.size();
    vector<bool> done(n, false);
    queue<Process> scheduled;
    int currentTime = 0, completed = 0;

    while (completed < n)
    {
        int idx = -1;
        int minBurst = INT_MAX;

        for (int i = 0; i < n; ++i)
        {
            if (!done[i] && processesVec[i].arrivalTime <= currentTime)
            {
                if (processesVec[i].burstTime < minBurst)
                {
                    minBurst = processesVec[i].burstTime;
                    idx = i;
                }
            }
        }

        if (idx == -1)
        {
            int nextArrival = INT_MAX;
            for (int i = 0; i < n; ++i)
                if (!done[i])
                    nextArrival = min(nextArrival, processesVec[i].arrivalTime);

            currentTime = nextArrival;
            continue;
        }

        Process &p = processesVec[idx];
        p.startTime = currentTime;
        p.waitingTime = p.startTime - p.arrivalTime;
        p.responseTime = p.startTime - p.arrivalTime;
        p.finishTime = currentTime + p.burstTime;
        p.turnaroundTime = p.finishTime - p.arrivalTime;

        currentTime += p.burstTime;
        done[idx] = true;
        completed++;

        scheduled.push(p);
    }

    return scheduled;
}

/**
 * @brief Computes and prints overall scheduling statistics.
 *
 * Statistics include:
 * - Total elapsed time
 * - Throughput (per assignment definition)
 * - CPU utilization
 * - Average waiting time
 * - Average turnaround time
 * - Average response time
 *
 * @param scheduled Queue of scheduled processes.
 * @param outFile Output file stream.
 */
void displayStatistics(queue<Process> &scheduled, ofstream &outFile)
{
    int totalBurstTime = 0;
    int totalWaitingTime = 0;
    int totalTurnaroundTime = 0;
    int totalResponseTime = 0;
    int lastFinishTime = 0;

    int n = scheduled.size();

    while (!scheduled.empty())
    {
        Process p = scheduled.front();
        scheduled.pop();

        totalBurstTime += p.burstTime;
        totalWaitingTime += p.waitingTime;
        totalTurnaroundTime += p.turnaroundTime;
        totalResponseTime += p.responseTime;

        lastFinishTime = max(lastFinishTime, p.finishTime);
    }

    // Per assignment: throughput = total burst time / number of processes
    double throughput = (double)totalBurstTime / n;

    double cpuUtilization = (double)totalBurstTime / lastFinishTime;
    double avgWaitingTime = (double)totalWaitingTime / n;
    double avgTurnaroundTime = (double)totalTurnaroundTime / n;
    double avgResponseTime = (double)totalResponseTime / n;

    cout << fixed << setprecision(2);
    cout << "Number of processes: " << n << endl;
    cout << "Total elapsed time: " << lastFinishTime << endl;
    cout << "Throughput: " << throughput << endl;
    cout << "CPU Utilization: " << cpuUtilization * 100 << "%" << endl;
    cout << "Average waiting time: " << avgWaitingTime << endl;
    cout << "Average turnaround time: " << avgTurnaroundTime << endl;
    cout << "Average response time: " << avgResponseTime << endl;

    outFile << fixed << setprecision(2);
    outFile << "Number of processes: " << n << endl;
    outFile << "Total elapsed time: " << lastFinishTime << endl;
    outFile << "Throughput: " << throughput << endl;
    outFile << "CPU Utilization: " << cpuUtilization * 100 << "%" << endl;
    outFile << "Average waiting time: " << avgWaitingTime << endl;
    outFile << "Average turnaround time: " << avgTurnaroundTime << endl;
    outFile << "Average response time: " << avgResponseTime << endl;
}

/**
 * @brief Main entry point. Runs FIFO and SJF scheduling simulations.
 *
 * @param argc Argument count
 * @param argv Command-line arguments
 * @return int Exit status
 */
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <datafile.txt>" << endl;
        return 1;
    }

    ofstream outFile("output.txt");
    if (!outFile)
    {
        cerr << "Error: Unable to create output.txt" << endl;
        return 1;
    }

    string filename = argv[1];
    vector<Process> processes;
    readProcess(processes, filename);

    // FIFO Scheduling
    queue<Process> fifoQueue;
    for (const Process &p : processes)
        fifoQueue.push(p);

    cout << "FIFO Scheduling Statistics:\n";
    outFile << "FIFO Scheduling Statistics:\n";
    fifoQueue = fifoAlgorithm(fifoQueue);
    displayStatistics(fifoQueue, outFile);
    outFile << endl;

    // SJF Scheduling
    cout << "\nSJF Scheduling Statistics:\n";
    outFile << "SJF Scheduling Statistics:\n";
    queue<Process> sjfQueue = sjfAlgorithm(processes);
    displayStatistics(sjfQueue, outFile);

    outFile.close();
    return 0;
}
