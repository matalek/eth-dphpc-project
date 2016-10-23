#!/usr/bin/env python
import subprocess
import csv

path = raw_input('Insert tester path:')
subprocess.call('g++ -std=c++11 ' + path +  ' -o test' , shell=True)

choice = 0
while choice != 1 and choice !=2:
    choice = input('Select 1 to load points from file, 2 to insert them manually:\n')

points = ''

if choice == 1:
    file_path = raw_input('Insert path of file to read points from:\n')
    with open(file_path, 'rb') as f:
        reader = csv.reader(f)
        for row in reader:
            points += ' ' + str(row[0]) + ',' + str(row[1])

else:
    path = ''
    print('Insert points in the form x,y and end with '''"exit"'')
    while path != 'exit':
        points += path + ' '
        path = raw_input()


subprocess.call('./test' + points, shell=True)