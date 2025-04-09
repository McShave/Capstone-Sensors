While looking at guides for setting up an ESP32 web server I noticed some mention the built in feature for motion detection.

This version makes use of an example built into an older version of the esp32 board, having the optional feature for facial detection
to enroll a face and recgonition for when that face is/isn't detected later.

However, I understand why they stopped including this function in the example as it is meant for the esp32s3 board that has the hardware
to support facial recongition and the board we use barely can manage the detection aspect.

This version is modifed to work on WPA2-Enterprise networks as it was tested on Carleton's wireless network.
