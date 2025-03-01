# simple-STM32F411RE-W5500-WEB-server
This project demonstrates how to use the STM32F411 microcontroller to create a simple web server. The server listens for HTTP requests, collects data from an ADC (Analog-to-Digital Converter), and monitors the state of a button. The ADC readings are transmitted over SPI to the Wiznet W5500 Ethernet module for network communication. The potentiometer's voltage is read by the ADC to dynamically change the measurements. It is configured to be reachable on 192.168.55.10.
![image](https://github.com/user-attachments/assets/aab75ca7-9ae2-4eb8-8b82-39708aa556c2)

##Features
- Ethernet Connectivity: The project uses the W5500 Ethernet module to connect the STM32 microcontroller to a local network.
- ADC Monitoring: Continuously measures analog values from sensors and sends the results via HTTP.
- Button State Monitoring: Reads a button state and includes its value in the HTTP response.
- Web Server: The server listens for HTTP requests and responds with JSON data containing the ADC value and button state.
- Data Transmission: Sends collected data in JSON format via a POST request over SPI.
- Socket Communication: Establishes TCP connections to communicate with a remote server for data transmission.

## Components Used:
- STM32F411RE microcontroller (NUCLEO-F411RE board)
- W5500 Ethernet module
- Built it STM button
- Built in STM ADC
- Potentiometer to change voltage

## How does it Work?
- The STM32 microcontroller initializes the W5500 Ethernet module using SPI.
- A simple web server runs on the STM32, listening for HTTP requests with a help of W5500.
- The server continuously reads the ADC value from the potentiometer and the button state.
Upon receiving an HTTP request, the server responds with a JSON payload containing:
  - The ADC value.
  - The button state (whether pressed or not).
- The server can be accessed on 192.168.55.10.
- All data is sent in a JSON format.

## Physical view:
![image](https://github.com/user-attachments/assets/63534696-5225-42c0-a10b-636d8776eca5)

