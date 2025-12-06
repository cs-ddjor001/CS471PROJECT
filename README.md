# CS 471 Project #

The project deals with two problems related to operating systems:
1. CPU Scheduling (50 points)
2. Process Synchronization (50 points)
The background material required to solve these problems has been covered in Module 2
and 3s. You can choose either Java or C++ as a programming language for the project.
Solutions to each of these problems will be graded separately. You are required to submit
the following by the deadline. You will submit a single directory that has two
subdirectories for each of the problems. The directory you submit will have the
following structure:
CS471PROJECT (main dir)
-------------- Recorded Video
-------------- CPUSCHED (subdir 1)
-------------- README file with directions (make sure to include
actual commands that can be copied and pasted to run)
-------------- SOURCE code (well documented)
-------------- Executable code
-------------- Sample input data files
-------------- Sample output file(s)
--------------- PRODUCER-CONSUMER (subdir 2)
-------------- README file with directions (make sure to include
actual commands that can be copied and pasted to run)
-------------- SOURCE code (well documented)
-------------- Executable code
-------------- Sample input data files
-------------- Sample output file(s)
Details of each of these problems are in the appendix.

# Appendix #

# Problem 1: CPU Scheduling # 
This part of the project simulates a CPU scheduler. Since it is a simulation, there are no
real processes to be scheduled. Instead, you simulate the arrival of new processes.
Whenever a new process arrives (in simulation) into the ready queue, the CPU scheduler
is invoked. Each simulated process has the following parameters: < Arrival time, CPU
burst units>. Each of these is an integer parameter.
For example, if a process <100, 20> is read by your simulator, it means a new process P
arrived at the system at simulation time 100, it executes for a CPU burst time of 20 units
before finishing and leaving the system. At the time of invocation of the scheduler, the
user indicates the type of scheduling to be enforced. You are required to implement the
following two scheduling types:
1. FIFO
2. SJF without preemption
Each run will handle scheduling of 500 (simulated) processes. In other words, as soon as
the number of processes that have completed CPU execution reaches 500, you can stop
running the program and print the following statistics. All times are expressed in terms
of CPU burst units. So they are not actual elapsed time (in msec) but simulated time.
Statistics for the Run
Number of processes: 500
Total elapsed time (for the scheduler):
Throughput (Number of processes executed in one unit of CPU burst time):
CPU utilization:
Average waiting time (in CPU burst times):
Average turnaround time (in CPU burst times): Average response time
(in CPU burst times):
Since there are only 500 processes, it may be easy to read all 500 processes data first
(from datafile-txt), and store the records in a queue in the program, and then process
them. This is an easy way to do it. But you can choose any method you like.
The following formulas will be utilized for calculating Problem 1:
Total elapsed time: The amount of time from initiation to termination of the application.
Throughput (Number of processes executed in one unit of CPU burst time) so you can calculate it using the formula: Total burst
time( It is the sum of all given burst length) / Total number of processes
CPU Utilization: The CPU Utilization measures the percentage of the time that the CPU is performing work so you can calculate it
using the formula: Total Burst Time/ Total elapsed time
Average waiting time: total waiting time / Number of processes
Turnaround time = Burst time + Waiting time or
Turnaround time = Exit time - Arrival time
Average turnaround time: Total turnaround time / No of processes
Response time = Time at which the process gets the CPU for the first time - Arrival time Average
response time: Total response time / No of processes

# Problem 2: Producer-Consumer Problem #

Here, we have a set of p producers and c consumers, each running as a single thread.
They are synchronized via shared buffer of size b (i.e., it can accommodate b items).
Each buffer item contains the following information: Sales Date (DD/MM/YY), store ID
(integer), register# (integer), sale amount (float). Each item represents a sales record
from a specific cashier register in a particular location of a retail chain. Thus, each
producer reports sales from a specific store location. Each consumer represents an entity
that reads sales records and computes sales statistics locally. Each buffer item is
consumed by one and-only-one consumer. When all sales records have been read
(indicated by a special flag set by another designated thread), each consumer adds its
local statistics to the global statistics (in the shared space). It also prints its own local
statistics along with its ID. In addition, your main program (parent process) prints the
overall (global) statistics.
The statistics to be maintained are:
Store-wide total sales
Month-wise total sales (in all stores)
Aggregate sales (all sales together)
Total time for simulation (from begin to end)
Each producer produces records randomly. Assume that the DD field is 1-30, MM is 01-
12, and YY is always 16. Store IDs are in the 1 to p range (where p is the number of
producers). The register numbers range from 1-6 for any store. The sale amount in each
item can range between 0.50 and 999.99. Each producer generates its record with
random data. Run the program until 1000 items are produced by all producers together.
Obviously, the number of items produced so far (by all producers) needs to be
maintained in shared memory. Each producer is assigned a fixed store ID when it is
created. It has the following structure:

While the total number of items generated by all producers is less than 1000
do
{
Randomly generate DD, MM, register#, sale amount.
Create a sales record and place it in the shared buffer.
Increment the number of records count (in the shared memory)
Randomly sleep for 5-40 milliseconds
}

Your report should clearly indicate the places where shared variables and semaphores
were employed by your code to manage shared variables. Run your program for p=2, 5,
and 10 (i.e., try all three possible values for number of producers). Similarly, run it for
c=2, 5, and 10. So there will be 9 runs in total. Your report should compile the results
from all 9 runs and make a comparison of the time. Use pseudo random number
generators to generate data.
