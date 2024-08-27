from socket import *
import time
import mysql.connector
import json

mydb = mysql.connector.connect(
    host = "localhost",
    user = "vesic.e116",
    password = "Fh4nC'(#",
    database = "db_vesic_e116"
)

port = 50044
serverSocket = socket(AF_INET, SOCK_DGRAM)
serverSocket.bind(('', port))
print ('UDP server (port ' + str(port) + ')\n')

msgCnt = 1
while True:
    try:
        messageIn, clientAddress = serverSocket.recvfrom(4096)
        ts = time.localtime()
        print('\033[0;34;40mUDP server (' + str(port) + ') Poruka#', str(msgCnt))
        print(time.strftime('%Y-%m-%d %H:%M:%S\033[0;37;40m', ts))
        data = json.loads(messageIn.decode("utf-8"))
        print (" Temperatura: ", data["temp"]["value"], "\u00b0C")
        print (" Vlaznost vazduha:", data["humi"]["value"], "%")
        print (" Osvetljenost: ", data["lum"]["value"], "lux")
        print (" Grejac: ", data["heater"]["value"])
        mycursor = mydb.cursor()
        
        mycursor.execute("INSERT INTO wgs_data (temp, humi, lum, heater) VALUES " + "(" + str(data["temp"]["value"]) + "," + str(data["humi"]["value"]) + "," + str(data["lum"]["value"]) + "," + str(data["heater"]["value"]) + ")")
        
        mydb.commit()
        serverSocket.sendto(bytearray("OK!", "utf-8"), clientAddress)
        msgCnt += 1
    except:
        print('Greska!!!')