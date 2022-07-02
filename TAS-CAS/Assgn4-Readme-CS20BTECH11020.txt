1) Browse to the directory where all the files are present. There must be a "inp-params.txt" file with n,k,lambda1,lambda2 values
    if Compiling the files individually.


2) To Compile each of the file one by one  
    a. TAS          :   $ g++ SrcAssgn4-tas-CS20BTECH11020.cpp -lpthread -o tas
    b. CAS          :   $ g++ SrcAssgn4-cas-CS20BTECH11020.cpp -lpthread -o cas
    c. CAS-Bounded  :   $ g++ SrcAssgn4-cas-bounded-CS20BTECH11020.cpp -lpthread -o cas-bounded


3) To execute each of the following run:
    a. TAS          : ./tas
    b. CAS          : ./cas
    c. CAS-Bounded  : ./cas-bounded

4) The log is written to a file called <filename>-Log.txt and stats are written to a file called <filename>-Stats.txt
