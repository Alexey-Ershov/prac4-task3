#! /usr/bin/env python3


import random
import sys
import glob
import os
import lxml.etree as xml


def printHelp():
    print("Usage:")
    print("    ./generate_data.py [number_of_requests]")

def parseCmd():
    if len(sys.argv) >= 2:
        if sys.argv[1] == "-h" or sys.argv[1] == "--help":
            printHelp()
            sys.exit()

        req_num = int(sys.argv[1])

    else:
        req_num = 1

    if len(sys.argv) >= 3:
        serv_num = int(sys.argv[2])

    else:
        serv_num = 1

    if len(sys.argv) >= 4:
        req_size = int(sys.argv[3])

    else:
        req_size = None

    if len(sys.argv) >= 5:
        serv_size = int(sys.argv[4])

    else:
        serv_size = None

    return req_num, serv_num, req_size, serv_size

def cleanDir(path):
    files = glob.glob(path + "/*")
    for f in files:
        os.remove(f)

def createXml(filename, size, index, type):
    root = xml.Element("configuration", n=str(index))
    for i in range(size):
        if type == "vm":
            left_c = 1
            right_c = 4
            left_r = 4
            right_r = 16

        else:
            left_c = 4
            right_c = 16
            left_r = 16
            right_r = 64
        
        core_num_ = random.randint(left_c, right_c)
        ram_ = random.randint(left_r, right_r)
        root.append(xml.Element(type,
                                core_num=str(core_num_),
                                ram=str(ram_)))
    
    with open(filename, "w") as file:
        file.write(xml.tostring(root, pretty_print=True).decode("utf-8"))


if __name__ == "__main__":
    cleanDir("id/requests")
    cleanDir("id/servers")

    req_num, serv_num, req_size, serv_size = parseCmd()
    print(req_num, serv_num, req_size, serv_size)

    rand_flag = False
    for i in range(req_num):
        if req_size is None or rand_flag:
            req_size = random.randint(10, 20)
            rand_flag = True

        print("DEBUG: req_size = {}".format(req_size))

        n = str(i)
        if len(n) == 1:
            n = "0" + n

        filename = "./id/requests/" + "r" + n + ".xml"
        
        print("DEBUG: filename = {}\n".format(filename))

        createXml(filename, req_size, i, "vm")

    rand_flag = False
    for i in range(serv_num):
        if serv_size is None or rand_flag:
            serv_size = random.randint(5, 10)
            rand_flag = True

        print("DEBUG: serv_size = {}".format(serv_size))

        n = str(i)
        if len(n) == 1:
            n = "0" + n

        filename = "./id/servers/" + "s" + n + ".xml"
        
        print("DEBUG: filename = {}\n".format(filename))

        createXml(filename, serv_size, i, "serv")
