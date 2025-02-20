import serial
from time import sleep
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

from lowPassFilter import butter_lowpass_filter_full_wave, offsetRemover

# Serial port settings
SERIAL_PORT = "COM9"  # Change this to match your port
BAUD_RATE = 115200
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
sleep(2)

# Initialize figure and axis
buffer_size = 400
fig, ax = plt.subplots()
x_data, y_data, y_data2 = [], [], []  # Data lists for plotting
plot_range = 3500

# Plot line (empty at the beginning)
line, = ax.plot([], [], label="IR sensor value", color='orange')
line2, = ax.plot([], [], label="AM signal demodulated", color='green')
ax.set_ylim(-plot_range,plot_range)

# Title and labels
ax.set_title("Real-Time Serial Data Plot")
ax.set_xlabel("Time [ms]")
ax.set_ylabel("Voltage [mV]")
ax.legend()  # Add legend

# Dynamic text annotation (will be updated in the animation)
text_annotation = ax.text(0.7, -0.12, "", transform=ax.transAxes, fontsize=12, color="black")

# Update function for animation
def update(frame):
    global x_data, y_data

    # Read and parse data from serial
    try:
        data = ser.readline().decode('utf-8').strip()
        if data: # append data
            value = float(data)  # Convert to float
            x_data.append(frame)  # Time step
            y_data.append(value)

            # Keep a fixed window of data points (pop)
            if len(x_data) > buffer_size:
                x_data.pop(0)
                y_data.pop(0)

            # remove DC offset
            y_data_aux = offsetRemover(y_data.copy())
            # filter data to extranct the carrier
            envelope = y_data_aux.copy()
            envelope_copy = envelope.copy()
            envelope= butter_lowpass_filter_full_wave(y_data_aux,200,10000,5)
            # compute Vrms
            Vrms = (1/np.sqrt(2))*np.max(envelope)            
            
            # Update line data
            line.set_data(x_data, y_data_aux)
            line2.set_data(x_data, envelope)
            
            ax.set_xlim(max(0, frame - 3*buffer_size/4), frame)  # Scroll x-axis

            # Update annotation with current value
            text_annotation.set_text(f"Vrms: {Vrms:.2f}mV")

    except Exception as e:
        print(f"Error: {e}")

    return line, line2, text_annotation

# Animate the plot
ani = animation.FuncAnimation(fig, update, interval=1)

plt.grid(True)
plt.show()

# Close the serial connection on exit
ser.close()
