// Include library
// #include <BMC_SBUS.h>
#include <RM_DBUS.h>
#include <ps5Controller.h>

//Declare BMC_SBUS/RM_DBUS Object
// BMC_SBUS SBUS;
RM_DBUS DBUS;

// Sbus delay value
const int busWAIT = 7;      //frame timing delay in msecs

// Declare sbus control channels
int panChannel = 1;
int tiltChannel = 2;
int rollChannel = 4;

void setup() {
  // Start BMC_SBUS/RM_DBUS object
  // SBUS.begin();
  DBUS.begin();

  // Start serial
  Serial.begin(115200);
  Serial.println("Ready.");

  // Start 
  ps5.begin("24:A6:FA:09:85:4A"); //replace with MAC address of your controller
}

void loop() {
    while (ps5.isConnected() == true) {
      if (ps5.Right()) Serial.println("Right Button");
      if (ps5.Down()) Serial.println("Down Button");
      if (ps5.Up()) Serial.println("Up Button");
      if (ps5.Left()) Serial.println("Left Button");

      if (ps5.Square()) Serial.println("Square Button");
      if (ps5.Cross()) Serial.println("Cross Button");
      if (ps5.Circle()) Serial.println("Circle Button");
      if (ps5.Triangle()) Serial.println("Triangle Button");

      if (ps5.UpRight()) Serial.println("Up Right");
      if (ps5.DownRight()) Serial.println("Down Right");
      if (ps5.UpLeft()) Serial.println("Up Left");
      if (ps5.DownLeft()) Serial.println("Down Left");

      if (ps5.L1()) Serial.println("L1 Button");
      if (ps5.R1()) Serial.println("R1 Button");

      if (ps5.Share()) Serial.println("Share Button");
      if (ps5.Options()) Serial.println("Options Button");
      if (ps5.L3()) Serial.println("L3 Button");
      if (ps5.R3()) Serial.println("R3 Button");

      if (ps5.PSButton()) Serial.println("PS Button");
      if (ps5.Touchpad()) Serial.println("Touch Pad Button");

      if (ps5.L2()) {
        Serial.printf("L2 button at %d\n", ps5.L2Value());
      }
      if (ps5.R2()) {
        Serial.printf("R2 button at %d\n", ps5.R2Value());
      }

      if (ps5.LStickX()) {
        Serial.printf("Left Stick x at %d\n", ps5.LStickX());
      }
      if (ps5.LStickY()) {
        Serial.printf("Left Stick y at %d\n", ps5.LStickY());
      }
      if (ps5.RStickX()) {
        Serial.printf("Right Stick x at %d\n", ps5.RStickX());
      }
      if (ps5.RStickY()) {
        Serial.printf("Right Stick y at %d\n", ps5.RStickY());
      }
      Serial.println();

      // Map dualsense values which go from -128 to 128 to Sbus values 0-2047
      // For no move send 1023
      int ch_yaw = map(ps5.LStickX(),-128,128,0,2047);
      int ch_throttle = map(ps5.LStickY(),-128,128,0,2047);
      int ch_roll = map(ps5.RStickX(),-128,128,0,2047);
      int ch_pitch = map(ps5.RStickY(),-128,128,0,2047);

      // Set SBUS/DBUS tilt
      // SBUS.Servo(tiltChannel,sendValueY);
      // SBUS.Servo(rollChannel,sendValueX);         
      // SBUS.Servo(panChannel,sendValueZ);
      DBUS.write_channel(1, ch_throttle);
      DBUS.write_channel(2, ch_yaw);
      DBUS.write_channel(3, ch_pitch);
      DBUS.write_channel(4, ch_roll);

      // Update SBUS/DBUS object and send data
      // SBUS.Update();
      // SBUS.Send();
      DBUS.update();
      DBUS.send();

      // Delay for BUS
      delay(busWAIT);

      // This delay is to make the output more human readable
      // Remove it when you're not trying to see the output
      delay(300);
    }
}