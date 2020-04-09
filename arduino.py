import serial
import matplotlib.pyplot as plt
from drawnow import *
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from datetime import datetime

sender ='theodoulos.christou@gmail.com'
password ='lgtelevision1'
receiver = 'theodoulos.christou@gmail.com'
subject_of_email ='Watering System Plant'
message_of_email ='Your Watering System Plant is on. You can check the data visualisation of your plants in python.'


msg = MIMEMultipart()
msg['From'] = sender
msg['To'] = receiver
msg['Subject'] = subject_of_email

msg.attach(MIMEText(message_of_email, 'plain'))

server = smtplib.SMTP('smtp.gmail.com', 587)
server.starttls()
server.login(sender, password)
str = msg.as_string()
server.sendmail(sender, receiver, str)
server.quit()


humidity = []
temperatureC = []
temperatureF = []
moistureSensorValue = []
heatIndexInCelsius = []
heatIndexInF = []
waterLevel = []

data = serial.Serial('COM6', 9600)
plt.ion()

def makeFig():
    plt.figure(1)
    plt.ylim(0, 100)
    plt.xlim(0, 100)
    # plt.title('Plotting Live Data For The Watering System Plant')
    dt = datetime.now()
    dateAndTime = dt.strftime("%d/%m/%Y %H:%M:%S")
    plt.title('Ζωντανη Μεταδοση Δεδομενων Ποτιστικου Συστηματος \n Date and Time ' + dateAndTime)
    plt.grid(True)
    plt.ylabel('Y-Axis')
    plt.xlabel('X-Axis')
    plt.plot(humidity, 'black', label='Humidity')
    plt.plot(temperatureC, 'coral', label='Temperature C')
    plt.plot(temperatureF, 'yellow', label='Temperature F')
    plt.plot(moistureSensorValue, 'green', label='Moisture Level')
    plt.plot(heatIndexInCelsius, 'grey', label='Heat Index Celsius')
    plt.plot(heatIndexInF, 'firebrick', label='Heat Index Farheneit')
    plt.plot(waterLevel, 'b', label='Water Level')
    plt.grid(True)
    plt.legend()
    plt.show()

while True:
    while(data.inWaiting()==0):
        pass
    string = data.readline()
    dataArray = string.decode().split(",")
    h = float(dataArray[0])
    c = float(dataArray[1])
    f = float(dataArray[2])
    m = float(dataArray[3])
    hic = float(dataArray[4])
    hif = float(dataArray[5])
    w = float(dataArray[6])
    humidity.append(h)
    temperatureC.append(c)
    temperatureF.append(f)
    moistureSensorValue.append(m)
    heatIndexInCelsius.append(hic)
    heatIndexInF.append(hif)
    waterLevel.append(w)
    drawnow(makeFig)
    print(h)
    print(c)
    print(f)
    print(m)
    print(hic)
    print(hif)
    print(w)