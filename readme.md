# About 
This repository presents a new prototype for unit testing in C++ and  examples for testing these tools.

The project is completed during the preparation of Aleksei Radchenko work under Testing of software at SPbPU Institute of Computer Science and Cybersecurity (SPbPU ICSC).

# Authors and contributors 
The advisor and contributor Vladimir A. Parkhomenko Seniour Lecturer of SPbPU ICSC. The main contributor Aleksei Radchenko student of SPbPU ICSC.

# Warranty
The contributors give no warranty for the using of the software.

# Licence
This program is open to use anywhere and is licensed under the GNU General Public License v3.0.

# Project structure
The project is organized into several repositories featuring projects tested with doctest, GoogleTest, and Catch2.

For the cista and lazycsv libraries, which already included doctest coverage, we added test suites using Catch2 and GoogleTest. For the simpleson project, we implemented comprehensive tests using all three frameworks.

The outputs directory contains the results of our performance analysis:

Build time: Measured using the hyperfine utility to benchmark the CMake build process for each test suite.

Execution time: Measured using the perf utility to analyze the runtime performance of the compiled tests.