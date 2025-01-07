# Arduino UNO R4 Wifi SQL to LED Matrix
A small project to scroll messages from a MariaDB/MySQL database on a server to the LED Matrix on the Arduino UNO R4 Wifi.

## Files
Here is the list of files, and what they do.

### arduino-messages.sql
This is the SQL file, that creates the "arduino_messages" table in a MariaDB or MySQL database of your choice. Please create an account for the use of the table by the Arduino.

### secrets.h
This is the personal configuration file. Here you need to add your wifi credentials that the Arduino has to use, and the database information and database user account. Edit as needed.

### uno-r4-wifi-sql-to-matrix.ino
Off course, this is the Adruino IDE file to import in your Sketch (together with the secrets.h file!).

## Future updates
I'm currently working on the PHP serverside page on which you can easily add, edit and delete the messages in the "arduino_messages" table. When that part is done and tested, I'll put it also up here, it is separate from the Arduino, reading the database.
