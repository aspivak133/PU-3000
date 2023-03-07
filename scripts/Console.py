import socket
import time
import sys
import os

# Addresses of the hardware target 
REMOTE_IP_ADDRESS = os.getenv('PSA_CONSOLE_TX_IP_ADDR')

# Address on which to bind the RX socket
LOCAL_IP_ADDRESS = os.getenv('PSA_CONSOLE_RX_IP_ADDR')

# Port on which to send commands
UDP_TX_PORT = 21500

# Port from which to receive messages
UDP_RX_PORT = 21501

# Create TX and RX sockets
socket.setdefaulttimeout(0.0)

sockTX = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
txAddr = (REMOTE_IP_ADDRESS, UDP_TX_PORT)

sockRX = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
rxAddr = (LOCAL_IP_ADDRESS, UDP_RX_PORT)
sockRX.bind(rxAddr)
sockRX.setblocking(1)
sockRX.settimeout(1.0)

# Process commands and receive messages

print "IAP-7000 PSA console"
print "Type \"exit\" and press <enter> to exit the console"
print "Type \"help\" and press <enter> to list the available commands (if target is running)"

while True:
    input = raw_input('> ')
    if input == "exit":
        break
    elif input == "":
        continue
    sockTX.sendto(input, txAddr)
    endOfMessage = False
    while not endOfMessage:
        try:
            incomingMessage = sockRX.recv(4096)
            if incomingMessage == "<EOM>":
                endOfMessage = True
            else:
                sys.stdout.write(incomingMessage)
                sys.stdout.flush()
        except socket.error as errMsg:
            sys.stdout.write("Timeout, no data received\n")
            endOfMessage = True

print("Exiting console\n")
sockTX.close()
sockRX.close()
sys.exit(0)    
