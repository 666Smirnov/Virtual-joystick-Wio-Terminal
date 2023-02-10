import asyncio
from asyncore import loop
from pyvjoystick import vjoy
from bleak import BleakScanner, BleakClient

# 2C:F7:F1:1B:DA:93: UART Servicess
# ED:65:8B:5B:ED:C3


x_characteristic = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
y_characteristic = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
z_characteristic = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

j = vjoy.VJoyDevice(1)

def scaler(val):
    val = float(val)
    old_min, old_max = -1, 1
    new_min, new_max = 1, 32768

    old_range = old_max - old_min
    new_range = new_max - new_min
    converted = ((val - old_min) * new_range / old_range) + new_min

    return int(converted)

async def main():
    devices = await BleakScanner.discover()
    for d in devices:
        print(d)
        if d.address == '2C:F7:F1:1B:DA:93':
            myDevice = d
            print('Found it')
            break
    async with BleakClient(myDevice.address, loop=loop) as client:
        while True:
            val_x = scaler((await client.read_gatt_char(x_characteristic)).decode())
            val_y = scaler((await client.read_gatt_char(y_characteristic)).decode())
            val_z = scaler((await client.read_gatt_char(z_characteristic)).decode())

            j.set_axis(vjoy.HID_USAGE.X, val_x)
            j.set_axis(vjoy.HID_USAGE.Y, val_y)
            j.set_axis(vjoy.HID_USAGE.Z, val_z)

            print(val_x, val_y, val_z)
            #print(val.decode("uint") )



asyncio.run(main())