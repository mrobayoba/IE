Aquí se incluyen los scripts para Generar y recibir una señal AM por medie de las tarjetas de desarrollo ESP32 SoC series y Arduino UNO R4.



Dentro de la carpeta Arduino Uno R4 WiFi se encuentra el archivo main.ino para cargar el programa por medio de ARDUINO IDE y Tambien está el archivo main.cpp para los que usen cualquier otro editor de codigo.

OUTPUT_PIN -> A0
INPUT_PIN -> A1

En la carpeta ESP32 SoC series está el script para montar al ESP32 desde platformio.

OUTPUT_PIN -> IO25
INPUT_PIN -> IO13

Desde los main es posible modifica las frequencias del carrier y la moduladora por medio de las variables

carrierFreq, modFreq

Tenga en cuenta que cambiar la frecuencia de la moduladora está directamente ligado a la frecuencia de muestreo por lo que debe ajustar la variable samplingRate para poder obtener la salida deseada.

Debido a que el voltaje en la base del transistor npn es de ~700mV,  la onda está acotada por encima de este valor. Además el voltaje máximo tambien se encuentra acotado a 3300mV por seguridad de los microcontroladores.

Por último en la carpeta Python Serial Plotter está un script para la interfaz gráfica y un paquete de funciones para realizar la demodulación llamado lowPassFilter, el archivo que deben ejecutar en consola es SerialPlotter.py, antes de ejecutar el archivo se debe identificar el puerto de conexion al mcu y modificar la variable SERIAL_PORT. Tambien es posible cambiar el baudrate por medio de la variable BAUD_RATE.