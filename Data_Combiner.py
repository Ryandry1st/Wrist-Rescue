'''
	Data_Combiner.py
	Created on: July 22th, 2019
       Author: Ryan Dreifuerst
	
	Script intended to take all of the .xlsx files from a folder and combine them into a single csv file.
	Additional data features including a moving average and a change in each variable are added to potentially improve
	machine learning algorithms, which is the next code to run after combining all of the data.
	
	   Last updated 7/24/19
'''


import os, csv
import pandas as pd
import numpy as np

file_list = os.listdir()
file_list.remove('Data_Combiner.py')
if('All_Data.csv' in file_list):
    file_list.remove('All_Data.csv')
if('Data_Analysis.py' in file_list):
    file_list.remove('Data_Analysis.py')
if('Data_Collector_script.py' in file_list):
    file_list.remove('Data_Collector_script.py')
    
#print(file_list)

def avging(df, col, num_points):
    indexer = 0
    avging = np.array([df[col][0]]*num_points)
    avg = []
    
    for value in df[col]:
        avging[indexer] = value
        avg.append(np.sum(avging).astype(np.float32)/num_points)
        if indexer == num_points-1:
            indexer = 0
        else:
            indexer = indexer + 1
            
    return avg


def delta_var(df, col):
    deltas = []
    past_val = df[col][0]
    for value in df[col]:
        deltas.append(value - past_val)
        past_val = value

    return deltas


add_columns = ['Xavg', 'Yavg', 'Zavg', 'Pitchavg', 'Rollavg', 'Yawavg', 'dx', 'dy', 'dz', 'dpitch', 'droll', 'dyaw']
base_columns = ['Accel_x', 'Accel_y', 'Accel_z', 'Pitch', 'Roll', 'Yaw']
moving_average_num = 5
main_df = pd.DataFrame(columns = base_columns + add_columns + ['Danger'])
empty_files = []

for item in file_list:
    df = pd.read_excel(item)
    if(df['Accel_x'].empty):
        empty_files.append(item)
        continue
    for index, column in enumerate(base_columns):
        new_col = avging(df, column, moving_average_num)
        df.insert(df.columns.get_loc('Danger'), add_columns[index], new_col)
        
    for index, column in enumerate(base_columns):
        new_col = delta_var(df, column)
        df.insert(df.columns.get_loc('Danger'), add_columns[index+6], new_col)
        
    main_df = main_df.append(df)

print(main_df.columns)
print(main_df.describe())

main_df.to_csv('All_Data.csv', index=False)
for file in empty_files:
    print("removed file {} of size {}".format(file, os.path.getsize(file)))
    os.remove(file)


