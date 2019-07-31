# Wrist-Rescue
is a device which is able to use resource efficient machine learning to predict whether an elderly user has fallen.
Data is collected using a prototype setup with an Arduino, which is fed to a computer to create and test the algorithm. Code is collected with the data collector script, combined with the combiner, which also adds additional features like deltas and moving averages.
The process of the data analysis can be followed in the jupyter notebook "Data Analysis".
Finally, code is translated into C for the ATSAMD10D20 platform.

The final result is a random forest algorithm which is easily converted to C, and then stored in a .h file similar to that found in the directory titled "RF_2_Algo.h"

The system level code can be found as well called "main.c"
