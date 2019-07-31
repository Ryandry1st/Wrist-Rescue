'''
	Data_Collector_script.py
	Created on: Nov 24th, 2018
       Author: Ryan Dreifuerst
	
	Program for taking the data from a over serial communication device and placing it into
	an xlsx file. Expects 7 columns, x, y, z, pitch, roll, yaw, and danger from the serial device. When a stop value is sent,
	-1234567, then the program will stop and close the workbook, otherwise the file will never close/show up.
	All data appears in a file which is titled by the date and time of the reading. In recent updates the device is not purely for arduino,
	but naming convention is maintained.
	
	   Last updated 7/23/19
'''

import serial, time, xlsxwriter
#com = input(r"Which number COM Port is the arduino on? Check in device manager>>Ports")
comport = 'COM5' #+com

def read_again():
    print("Starting!")
    comport = 'COM5'
    try:
        Arduino = serial.Serial(comport, 115200, timeout = .5)
    except serial.serialutil.SerialException as e:
        pass
    start = time.strftime("%d-%m-%Y") +"_" + time.strftime("%H-%M-%S")
    name = start + '_IO_Data.xlsx'
    workbook = xlsxwriter.Workbook(name)
    worksheet = workbook.add_worksheet()
    row = 0
    col = 0

    worksheet.write(row, 0, 'Accel_x')
    worksheet.write(row, 1, 'Accel_y')
    worksheet.write(row, 2, 'Accel_z')
    worksheet.write(row, 3, 'Pitch')
    worksheet.write(row, 4, 'Roll')
    worksheet.write(row, 5, 'Yaw')
    worksheet.write(row, 6, 'Danger')

    row += 1
    cont = 0

    while cont == 0:
        try:
            data = Arduino.readline() # read the first line
        except serial.serialutil.SerialException as e:
            Arduino = serial.Serial(comport, 115200, timeout = .5)
            data = Arduino.readline()
        if data == b'\r0\t0\t0\t0\t0\t0\t0\n':
            cont = 1
            Arduino.close()
            workbook.close()
            read_again()
        elif data:
            print(data)          
            # removes EOL lines
            data = data.split(b'\t') # splits up the data by the tabs
            if(b'\r-1234567\n' in data):
                print("end signal received")
                cont = 1
            # print(len(data))

            else:
                if(len(data) >= 7):
                    for i in range(0, 7):
                        worksheet.write(row, int(i), data[i])
                else:
                    Arduino.close()
                    workbook.close()
                    read_again()
            row+=1
        


    print("Done!")
    workbook.close()
    Arduino.close()

for i in range(5):
    read_again()
