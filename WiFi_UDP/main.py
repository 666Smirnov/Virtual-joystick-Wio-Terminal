import socket
from pyvjoystick import vjoy

UDP_IP = "192.168.18.7"
UDP_PORT = 3333

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

j = vjoy.VJoyDevice(1)

def scaler(val):
    val = float(val)
    if val < -1:
        val = -1
    if val > 1:
        val = 1
    old_min, old_max = -1, 1
    new_min, new_max = 1, 32768
    old_range = old_max - old_min
    new_range = new_max - new_min
    converted = ((val - old_min) * new_range / old_range) + new_min
    return int(converted)

def str_to_bool(s):
    if s == "True":
        return True
    elif s == "False":
        return False
while True:
    axis, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    button, addr = sock.recvfrom(1024)  # buffer size is 1024 bytes
    XYZ = (axis.decode()).split()
    X = scaler(XYZ[0])
    Y = scaler(XYZ[1])
    Z = scaler(XYZ[2])

    j.set_axis(vjoy.HID_USAGE.X, X)
    j.set_axis(vjoy.HID_USAGE.Y, Y)
    j.set_axis(vjoy.HID_USAGE.Z, Z)

    buttons = (button.decode()).split()
    j.set_button(1, str_to_bool(buttons[0]))
    j.set_button(2, str_to_bool(buttons[1]))
    j.set_button(3, str_to_bool(buttons[2]))
    #print(buttons)