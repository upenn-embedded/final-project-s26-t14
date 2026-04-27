### 0. Abstract

The Mini Arcade Station features a simplified smash-TV shooting game with an external model turret gun in place of joy sticks. It is an embedded game on AtMega328PB.

### 1. Video

### 2. Images

<p align="center">
  <img src="image/FR_whole.png" alt="whole setup" width="660">
</p>

<p align="center">
  <img src="image/FR_playerPOV.png" alt="player pov" width="215">
  <img src="image/FR_isometric.png" alt="isometric" width="215">
  <img src="image/FR_side.png" alt="side" width="215">
</p>

### 3. Results

Our final design comes in two parts:

1. A laser-cut acrylic arcade box containing 2 AtMega328PB's, 2 LED segment displays, 1 GPIO extender, 1 LCD screen, 2 speakers, and connecting wires used to output game state. A LED strip is along the back board of the box for back lighting, featuring its own power bank, power module, and on/off switch.
2. A 3D-printed turret gun with 2 push buttons, 1 IMU chip, and 2 rotary potentiometers as user input.

#### 3.1 Software Requirements Specification (SRS) Results

#### 3.2 Hardware Requirements Specification (HRS) Results

### 4. Conclusion

We learned how to integrate across different timelines and people. For example, when Yi Lu first wrote the sound code, she didn't have the IMU or the game yet. When the sound code was passed to Amaris, Amaris had to package Yi Lu's code in sound.c library and ensured that it didn't use any pins/timers occupied by IMU polling. Finally, when Amaris passed that to Daniel, Daniel had to use a second AtMega for sound generation, in addition to changing the IMU polling to interrupts.

We are proud of see this project through its many stages. Our final design was more than what we originally proposed as we added/repurposed sensors, and due to these hardware changes our game had to be more complicated. The fun of this project really came from sparking new ideas with one another and executing it as a team. Some next steps to this project could be: adding mini vibrational motor discs in the turret handles, having concurrent music during game play, and including more player/enemy spirites.

### 5. References
