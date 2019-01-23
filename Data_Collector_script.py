'''
	Data_Collector_script.py
	Created on: Nov 24th, 2018
       Author: dreifuerstrm
	
	Program for taking the data from the Arduino over serial communication and placing it into
	an xlsx file. Expects 4 columns, x, y, z, and danger from the Arduino. When a stop value is sent,
	-1234567, then the program will stop and close the workbook, otherwise the file will never close/show up.
	All data appears in a file which is titled by the date and time of the reading
	   
	   Last updated 12/3/18
'''

import serial, time, xlsxwriter
com = raw_input("Which number COM Port is the arduino on? Check in device manager>>Ports \n")
comport = 'COM'+com
Arduino = serial.Serial(comport, 19200, timeout = .5)
start = time.strftime("%d-%m-%Y") +"_" + time.strftime("%H-%M-%S")
name = start + '_IO_Data.xlsx'
workbook = xlsxwriter.Workbook(name)
worksheet = workbook.add_worksheet()
row = 0
col = 0
worksheet.write(row, 0, 'Accel_x')
worksheet.write(row, 1, 'Accel_y')
worksheet.write(row, 2, 'Accel_z')
worksheet.write(row, 3, 'Danger')

row += 1
cont = 0

while cont == 0:
    data = Arduino.readline()[:20] # read the first line
    if data:
        print data          
        # removes EOL lines
        data = data.split('\t') # splits up the data by the tabs
        if('-1234567' in data):
            print("end signal received")
            cont = 1
        # print(len(data))

        else:
            for i in range(0, 4):
                worksheet.write(row, i, data[i])
        row+=1
    


workbook.close()
print "Done!"
