// "Secret" Configuration File, containing the credentials needed for
// displaying the messages on the Arduino UNO R4 Wifi LED matrix.
// (c)2025 Richard, webwings.nl

// Use and edit as needed, have fun! As the SQL file creates a database called arduino, this name
// is used in the DB_NAME constant. 

#ifndef SECRETS_H
#define SECRETS_H

// WiFi-Credentials
const char* WIFI_SSID = "my-wifi-network";
const char* WIFI_PASSWORD = "my-wifi-password";

// MySQL/MariaDB database-configuration
const char* DB_USER = "db-username";
const char* DB_PASSWORD = "db-user-password";
const char* DB_HOST = "host-url";               // Can be an URL or an IP addres
const int DB_PORT = 3307;                       // Standard would be 3306
const char* DB_NAME = "arduino";                // Name of the database used for the messages table

#endif // SECRETS_H