# ESP32 Recon Scout (Micro UGV)

Impact-tolerant miniature UGV for low-latency tactical video reconnaissance.

### Hardware Specs
* **MCU:** ESP32-CAM (ESP32-S with OV2640 Sensor)
* **Actuators:** 2x DC Motors
* **Motor Driver:** Dual H-Bridge DRV8833
* **Power:** 1S LiPo with 3.3V/5V voltage regulation
* **Chassis:** 3D-printed shock-absorbent concentric shell

### Architecture
* **Vision Subsystem:** I2S image capture & raw UDP socket fragmentation (Zero-handshake).
* **Control Subsystem:** High-frequency UDP packet listener & hardware PWM generation.
* **Network:** Direct Wi-Fi streaming prioritizing ultra-low latency over packet integrity.

### Disclaimer
This proof-of-concept is designed for educational robotics and authorized embedded systems research only.
