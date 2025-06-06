# Arduino UNO R4 Wifi SQL to LED Matrix
A small project to scroll messages from a MariaDB/MySQL database on a server to the LED Matrix on the Arduino UNO R4 Wifi, like the one I have and as shown below in the picture. The code should be quite self-explanitory, however it does contain remarks. They can be mostly in dutch, though. I will correct that in the near future.

![Arduino Uno R4 Wifi, with the LED Matrix](arduino-uno-r4-wifi.jpg)

## Files
Here is the list of files, and a small description what they do.

### arduino-messages.php
This small PHP script can add, edit and delete messages in the arduino-messages table in the MariaDB/MySQL database server. In my case, the PHP file is on the same server as the database, but this is not nessesary.

### arduino-messages.sql
This is the SQL file, that creates the "arduino_messages" table in a MariaDB or MySQL database of your choice. Please create an account for the use of the table by the Arduino.

### config.php
The file that contains your database configuration. Please update as needed. It needs your database credentials for using the database.

### secrets.h
This is the personal configuration file. Here you need to add your wifi credentials that the Arduino has to use, and the database information and database user account. Edit as needed.

### style.css
The stylesheet, to nice-ify the PHP script page to edit the messages. Off course this is optional, however quite nice.

### uno-r4-wifi-sql-to-matrix.ino
Off course, this is the Adruino IDE file to import in your Sketch (together with the secrets.h file!).

## Things to remember
The arduino-messages.php file does not have any security measures for adding, editing and deleting messages. So if someone knows the URL of the PHP file, he/she can see and edit the messages. Let me know if this needs to be addressed, or if you have a good solution, you also can let me know. Enjoy using your Arduino's, ESP32's and all the other types of MCU's.

## Future modifications
Maybe it woud be a good option, if possible, to set a wifi connection via a different way of provisioning the data so the Arduino could be taken to a hotel or place with a public wifi. Any ideas how to do that will be appreciated.
