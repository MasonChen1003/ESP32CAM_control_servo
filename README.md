# ESP32CAM_control_servo

使用 ESP32CAM 當控制板，搭配 esp motor shield 來控制四顆伺服馬達 
控制軟體為嵐奕科技的 V7RC
- 手機需先安裝 V7RC，採用 BLE 和 ESP32CAM 連線來控制，控制介面為"坦克模式"  

用 ESP32CAM 需注意要避開 CAM 用的 timer 1   
所以 LEDC 我用的是 channel 4/5/6/7   

以下為查到的 LEDC channel 和 timer 的對應表 

 * LEDC Chan to Group/Channel/Timer Mapping
** ledc: 0  => Group: 0, Channel: 0, Timer: 0
** ledc: 1  => Group: 0, Channel: 1, Timer: 0
** ledc: 2  => Group: 0, Channel: 2, Timer: 1
** ledc: 3  => Group: 0, Channel: 3, Timer: 1
** ledc: 4  => Group: 0, Channel: 4, Timer: 2
** ledc: 5  => Group: 0, Channel: 5, Timer: 2
** ledc: 6  => Group: 0, Channel: 6, Timer: 3
** ledc: 7  => Group: 0, Channel: 7, Timer: 3
** ledc: 8  => Group: 1, Channel: 0, Timer: 0
** ledc: 9  => Group: 1, Channel: 1, Timer: 0
** ledc: 10 => Group: 1, Channel: 2, Timer: 1
** ledc: 11 => Group: 1, Channel: 3, Timer: 1
** ledc: 12 => Group: 1, Channel: 4, Timer: 2
** ledc: 13 => Group: 1, Channel: 5, Timer: 2
** ledc: 14 => Group: 1, Channel: 6, Timer: 3
** ledc: 15 => Group: 1, Channel: 7, Timer: 3
