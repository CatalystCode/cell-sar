#!/usr/bin/env python

import json
import os
import sys

def parse_line(line):
   try:
      message = json.loads(line)
   except ValueError:
      return "";

   if message['type'] != 'phy':
      return "";

   phyinfo = message['data']

   result = []

   result.append('"=""{}"""'.format(message['IMSI']))
   result.append(str(phyinfo['TA']))
   result.append(str(phyinfo['UpRSSI']))
   result.append(str(phyinfo['TxPwr']))
   result.append(str(message['timestamp']))

   return ','.join(result) + os.linesep

def main():
   input_file = "/tmp/sar.log"
   if len(sys.argv) > 1 and os.path.isfile(sys.argv[1]):
      input_file = sys.argv[1]

   entries = [
      "IMSI,TA,UpRSSI,TxPwr,timestamp" + os.linesep,
   ]

   print("Loading file {}".format(input_file))
   with open(input_file, 'r') as sar_log:
      for line in sar_log:
         entry = parse_line(line)
         if entry:
            entries.append(entry)            

   output_file = "{}.csv".format(os.path.split(input_file)[1].split('.')[0])
   print("Writing to file {}".format(output_file))
   with open(output_file, 'w') as csv_file:
      csv_file.writelines(entries)

if __name__ == '__main__':
   main()
