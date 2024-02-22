
import serial


ser = serial.Serial('/dev/ttyUSB0', 115200)
if ser.isOpen():
    try:
        while 1:
            data = ser.readline()
            if data:
                print(data.decode('utf-8'))
    except Exception as e:
        print("Problem to read the serial port: ", e)
    finally:
        ser.close()
else:
    print("Not is possible to open the serial port.")



