# Moisture
Auto Soil Watering and Monitoring using Arduino

<h2> # Usage </h2>
<br>
The code relies on two Arduino libraries:
1. aJson <br>
2. Simpletimer <br>

<h2> # Point to your server by modifying the configs in .ino file <br> </h2>
byte serverIP[] = { 127, 0, 0, 1 };  // configure this <br>

//  your network config <br>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; <br>
byte ip[] = { 192, 168, 1, 1 }; <br>
byte submask[] = { 255, 255, 255, 0 }; <br>
byte gateway[] = { 192, 168, 1, 254 }; <br>
byte dns[] = { 192, 168, 1, 254 }; <br>

<h2> # Read the JSON from your server and use Cubism to visualize that data <br> </h2>

