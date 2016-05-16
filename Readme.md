#Snake game on Arduino
using arduino and the old nokia
3310 display (PCD8544)

![alt tag](http://cl.ly/image/1C0T3c3z0C3N/312558_133166523508240_756856555_n.jpg)

##video  
https://www.youtube.com/watch?v=y3MxgT6U4jI)

##Hardware
- arduino uno  
- nokia 3310 display  
- buzzer  
- 2 leds  
- 5 buttons
- 1uf capacitor
- 12v battery 

##Installation
(PCD8544)  
 pin1 -> VDD(3.3V)  
 pin2 -> SCK  
 pin3 -> SDIN  
 pin4 -> D/C  
 pin5 -> SCE  
 pin6 -> GND  
 pin7 -> VOUT(-> 1uF capacitor -> GND)  
 pin8 -> RESET  
     
Arduino    
 3  -> SCK   (2)  
 4  -> SDIN  (3)  
 5  -> D/C   (4)  
 6  -> RESET (6)  
 7  -> SCE   (5)  
 8  -> Speaker/buzzer  
 9  -> led1  
 10 -> led2  
 A1 -> Button (Pause)  
 A2 -> Button (Down)  
 A3 -> Button (Left)  
 A4 -> Button (Up)  
 A5 -> Butoon (Right)  
