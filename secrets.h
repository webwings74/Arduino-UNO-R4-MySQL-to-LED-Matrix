// "Secret" Configuration File, containing the credentials needed for
// displaying the messages on the Arduino UNO R4 Wifi LED matrix.
// (c)2025 Richard, webwings.nl
// Use and edit as needed, have fun! 

#ifndef SECRETS_H
#define SECRETS_H

// WiFi-Credentials
const char* WIFI_SSID = "my-wifi-network";
const char* WIFI_PASSWORD = "my-wifi-password";

// MySQL/MariaDB database-configuration
const char* DB_USER = "db-username";
const char* DB_PASSWORD = "db-user-password";
const char* DB_HOST = "host-url";
const int DB_PORT = 3307;
const char* DB_NAME = "arduino";

#endif // SECRETS_H
