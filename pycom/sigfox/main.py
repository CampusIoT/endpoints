from network import Sigfox
from machine import RTC
import socket
import time
import pycom
import binascii

pycom.heartbeat(False)
rtc = RTC()

# Initialize PyTrack.
#py = Pytrack()
#l76 = L76GNSS(py, timeout=30)

print('Boot at ', rtc.now())

RED   = 0xff0000;
GREEN = 0x00ff00;
BLUE  = 0x0000ff;
ORANGE = 0xffa500;
CYAN = 0x00B7EB;
PINK = 0xFF69B4;
OFF   = 0x000000;


# init Sigfox for RCZ1 (Europe)
sigfox = Sigfox(mode=Sigfox.SIGFOX, rcz=Sigfox.RCZ1)

print('Radio MAC=', binascii.hexlify(sigfox.mac()))
print('Sigfox ID=', binascii.hexlify(sigfox.id()))

# create a Sigfox socket
s = socket.socket(socket.AF_SIGFOX, socket.SOCK_RAW)

# make the socket blocking
s.setblocking(True)

# configure it as uplink only
s.setsockopt(socket.SOL_SIGFOX, socket.SO_RX, False)

cpt = 0

# TODO recv downlink for setting the RTC, ...

while True:
    #coord = l76.coordinates()
    #f.write("{} - {}\n".format(coord, rtc.now()))
    # print("{} - {} - {}".format(coord, rtc.now(), gc.mem_free()))
    #print("{} - {}".format(coord, gc.mem_free()))
    cpt = cpt + 1
    pkt = bytes([cpt & 0xFF, (cpt >> 8) & 0xFF])
    # TODO add coordinates (if PyTrack), LIS2HH12 values for acc
    print('Sending:', pkt)
    s.send(pkt)
    pycom.rgbled(GREEN)
    time.sleep(0.1)
    pycom.rgbled(OFF)
    time.sleep(60*10) # sleep 10 minutes (uplink messages are limited up to 144 per day)
