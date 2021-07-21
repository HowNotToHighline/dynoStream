# dynoStream

The goal of this project is to be a modular dyno/load cell driver.
It's based around a c++ backend with websockets, and a frontend which can connect/control that websocket.
The current plan is to make the frontend in react.js and use protobuf to efficiently pass data and commands between the front- and backend.

Proper documentation on the project will follow at a later moment.
I'm still not really sure what's the best approach is, so it will probably change in the future.
And if you have a better name, please let me know, I'm not so great at it, as you've probably noticed.

Minimum requirements:

- Show real-time peak force
- Handle 400kHz samplerate
- Have drivers for LabJack's and custom serial devices (at the very least)
- Run on a RaspberryPi
- Storage of the data to disk for later analysis
- A calibration system
- Automatically be able to start and stop recording based on the recorded force

Nice-to-haves:

- Real-time graph
- Multiple calibration profiles for different load cells
- Decimation
- Look back and ahead when automatically triggering
- Live switching of load cells/hardware drivers
