Your submission contains a README.md (or README.txt) file that describes whether you have implemented mts correctly. (Complete)

Your submission can be successfully compiled by make without warnings or errors. The produced executable is named as mts. (Complete)

Your mts can be executed with one command line parameter, i.e., the path to the input file. E.g., ./mts input.txt. (Complete)

Your mts saves the simulation output to output.txt following the format defined in Section 3.3. If mts is invoked multiple times on different input files, you should overwrite output.txt with the results from the latest run. (Complete)

Your mts has no deadlocks, and the simulation can complete within 1 minute using an input file that satisfies this condition. (Complete)


All the custom test cases that I ran based ont he rules:
1. Only one train is on the main track at any given time.
2. Only loaded trains can cross the main track.
3. If there are multiple loaded trains, the one with the high priority crosses.
4. If two loaded trains have the same priority, then:

a) If they are both traveling in the same direction, the train which finished loading first gets the clearance to cross first. If they finished loading at the same time, the one that appeared first in the input file gets the clearance to cross first.
b) If they are traveling in opposite directions, pick the train which will travel in the direction opposite of which the last train to cross the main track traveled. If no trains have crossed the main track yet, the Westbound train has the priority.

5. To avoid starvation, if there are two trains in the same direction traveling through the main track back to back, the trains waiting in the opposite direction get a chance to dispatch one train if any.

Multiple loaded trains high priority goes first:

e 1 10
w 2 2 
e 3 3
E 4 4
w 5 5

00:00:00.1 Train  0 is ready to go East
00:00:00.1 Train  0 is ON the main track going East
00:00:00.2 Train  1 is ready to go West
00:00:00.3 Train  2 is ready to go East
00:00:00.4 Train  3 is ready to go East
00:00:00.5 Train  4 is ready to go West
00:00:01.1 Train  0 is OFF the main track after going East
00:00:01.1 Train  3 is ON the main track going East
00:00:01.5 Train  3 is OFF the main track after going East
00:00:01.5 Train  1 is ON the main track going West
00:00:01.7 Train  1 is OFF the main track after going West
00:00:01.7 Train  2 is ON the main track going East
00:00:02.0 Train  2 is OFF the main track after going East
00:00:02.0 Train  4 is ON the main track going West
00:00:02.5 Train  4 is OFF the main track after going West



Two loaded trains with the same priority:

1. SAME DIRECTION - trains which finished loading first goes

e 1 10
e 4 3
e 3 3
e 2 3

00:00:00.1 Train  0 is ready to go East
00:00:00.1 Train  0 is ON the main track going East
00:00:00.2 Train  3 is ready to go East
00:00:00.3 Train  2 is ready to go East
00:00:00.4 Train  1 is ready to go East
00:00:01.1 Train  0 is OFF the main track after going East
00:00:01.1 Train  3 is ON the main track going East
00:00:01.4 Train  3 is OFF the main track after going East
00:00:01.4 Train  2 is ON the main track going East
00:00:01.7 Train  2 is OFF the main track after going East
00:00:01.7 Train  1 is ON the main track going East
00:00:02.0 Train  1 is OFF the main track after going East

2. DIFFERENT DIRECTION - trains traveling in the opposite direction of the last train

e 1 10
W 4 2
E 3 2

00:00:00.1 Train  0 is ready to go East
00:00:00.1 Train  0 is ON the main track going East
00:00:00.3 Train  2 is ready to go East
00:00:00.4 Train  1 is ready to go West
00:00:01.1 Train  0 is OFF the main track after going East
00:00:01.1 Train  1 is ON the main track going West
00:00:01.3 Train  1 is OFF the main track after going West
00:00:01.3 Train  2 is ON the main track going East
00:00:01.5 Train  2 is OFF the main track after going East


Check other queue if trains go back to back from same direction

e 1 10
W 2 3
W 3 2
e 4 1
W 4 2

00:00:00.1 Train  0 is ready to go East
00:00:00.1 Train  0 is ON the main track going East
00:00:00.2 Train  1 is ready to go West
00:00:00.3 Train  2 is ready to go West
00:00:00.4 Train  3 is ready to go East
00:00:00.4 Train  4 is ready to go West
00:00:01.1 Train  0 is OFF the main track after going East
00:00:01.1 Train  1 is ON the main track going West
00:00:01.4 Train  1 is OFF the main track after going West
00:00:01.4 Train  2 is ON the main track going West
00:00:01.6 Train  2 is OFF the main track after going West
00:00:01.6 Train  3 is ON the main track going East
00:00:01.7 Train  3 is OFF the main track after going East
00:00:01.7 Train  4 is ON the main track going West
00:00:01.9 Train  4 is OFF the main track after going West