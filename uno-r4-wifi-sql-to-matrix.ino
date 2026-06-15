// Show messages from a MariaDB/SQL Server on the
// Matrix Display of a Arduino UNO R4 Wifi.
// (c) 2025 Richard, webwings.nl

/**
 * @file    uno-r4-wifi-sql-to-matrix.ino
 * @brief   Scrolls messages from a MariaDB/MySQL database on the built-in
 *          LED matrix of the Arduino UNO R4 WiFi.
 *
 * @details Dependencies / libraries used:
 *  - secrets.h            : Local header file containing WiFi and database credentials.
 *  - WiFiS3.h             : WiFi library for the Arduino UNO R4 WiFi board.
 *  - MySQL_Connection.h   : Library for establishing a connection to a MySQL/MariaDB server.
 *  - MySQL_Cursor.h       : Library for executing SQL queries and retrieving results.
 *  - ArduinoGraphics.h    : Graphics library required for rendering text on the LED matrix.
 *  - Arduino_LED_Matrix.h : Library for controlling the built-in LED matrix on the UNO R4 WiFi.
 */

#include "secrets.h"                    // WiFi & Database credentials
#include <WiFiS3.h>                     // WiFi library for the UNO R4 WiFi
#include <MySQL_Connection.h>           // MySQL library
#include <MySQL_Cursor.h>               // For SQL queries
#include <ArduinoGraphics.h>            // Graphics library
#include <Arduino_LED_Matrix.h>         // LED matrix support

// WiFi setup
const char* ssid = WIFI_SSID;           // Use SSID from secrets.h
const char* password = WIFI_PASSWORD;   // Use password from secrets.h

// MySQL database settings
const char* server_url = DB_HOST;       // Use host from secrets.h
IPAddress server_ip;                    // IP address retrieved via DNS lookup
const int server_port = DB_PORT;        // Use port from secrets.h
char user[20];                          // Database username array
char password_db[20];                   // Database password array
char database[32];                      // Database name array

// SQL query to retrieve messages from the database
const char query[] = "SELECT message_text FROM arduino_messages ORDER BY created_at DESC;";

// WiFi and MySQL objects
WiFiClient client;
MySQL_Connection conn((Client *)&client);

// LED matrix object
ArduinoLEDMatrix matrix;

// Message variables
String scrollText = " Connecting... ";  // Initial display text
bool newTextAvailable = false;

/**
 * @brief   Initialises the Arduino on startup.
 *
 * @details Copies credentials from secrets.h into local variables,
 *          starts the serial monitor, connects to WiFi, resolves the
 *          database server hostname via DNS, connects to the MySQL
 *          database, and initialises the LED matrix for text scrolling.
 */
void setup() {
  // Copy credentials from secrets.h into local variables
  strcpy(user, DB_USER);                  // Database username from secrets.h
  strcpy(password_db, DB_PASSWORD);       // Password from secrets.h
  strcpy(database, DB_NAME);              // Database name from secrets.h

  // Start serial monitor
  Serial.begin(115200);
  while (!Serial);

  // Connect to WiFi
  connectToWiFi();

  // Resolve hostname to IP address via DNS
  Serial.print("Resolving hostname: ");
  Serial.println(server_url);
  if (!WiFi.hostByName(server_url, server_ip)) {
    Serial.println("DNS resolution failed!");
    while (1); // Stop if DNS resolution fails
  }
  Serial.print("Server IP address: ");
  Serial.println(server_ip);

  // Connect to MySQL server
  connectToDatabase();

  // Initialise LED matrix
  matrix.begin();
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);           // White text colour
  matrix.textScrollSpeed(50);          // Scroll speed in milliseconds
  matrix.textFont(Font_5x7);           // Select 5x7 pixel font
  matrix.endDraw();
}

/**
 * @brief   Connects to the WiFi network.
 *
 * @details Uses the SSID and password defined in secrets.h.
 *          Retries up to 20 times with a 500ms delay between attempts.
 *
 * @note    Halts the program permanently if the connection cannot be
 *          established after all retries.
 */
void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  int retries = 20;  // Maximum number of connection attempts
  while (WiFi.status() != WL_CONNECTED && retries > 0) {
    delay(500);
    Serial.print(".");
    retries--;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("
WiFi connected!");
  } else {
    Serial.println("
WiFi connection failed. Please check your settings!");
    while (1);  // Halt program
  }
}

/**
 * @brief   Connects to the MySQL/MariaDB database server.
 *
 * @details Uses the host, port, username, and password defined in secrets.h.
 *          Retries up to 3 times with a 2-second delay between attempts.
 *          After a successful connection, selects the target database.
 *
 * @note    Halts the program permanently if the connection cannot be
 *          established after all retries.
 */
void connectToDatabase() {
  Serial.print("Connecting to MySQL server...");
  int retries = 3; // Maximum number of connection attempts
  while (retries > 0 && !conn.connected()) {
    if (conn.connect(server_ip, server_port, user, password_db)) {
      Serial.println(" Connected!");

      // Select the target database
      MySQL_Cursor* cursor = new MySQL_Cursor(&conn);
      char use_db[50];
      sprintf(use_db, "USE %s;", database);
      cursor->execute(use_db);
      delete cursor;
      Serial.println("Database selected!");
      return;
    } else {
      retries--;
      Serial.println(" Connection failed. Retrying...");
      delay(2000); // Wait 2 seconds before retrying
    }
  }

  Serial.println("Unable to connect to the database. Halting program.");
  while (1);  // Halt program
}

/**
 * @brief   Fetches messages from the database and scrolls them on the LED matrix.
 *
 * @details Checks WiFi and database connections, reconnecting if necessary.
 *          Executes the SQL query to retrieve all messages from the
 *          arduino_messages table and scrolls each message across the LED
 *          matrix from right to left. Waits 1 second between messages.
 *
 * @note    If the SQL query fails, the function returns early without
 *          displaying any messages.
 */
void fetchMessagesAndScroll() {
  // Check WiFi connection; reconnect if lost
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    connectToWiFi();
  }

  // Check MySQL connection; reconnect if lost
  if (!conn.connected()) {
    Serial.println("Database connection lost. Reconnecting...");
    connectToDatabase();
  }

  // Execute the SQL query
  MySQL_Cursor* cursor = new MySQL_Cursor(&conn);
  if (!cursor->execute(query)) {
    Serial.println("Error executing query!");
    delete cursor;
    return;
  }

  // Retrieve column metadata
  column_names *cols = cursor->get_columns();

  // Iterate through result rows and display each message on the LED matrix
  row_values *row;
  while ((row = cursor->get_next_row())) {
    String message = row->values[0];        // Read message text from result
    message = "     " + message;            // Add leading spaces for scroll effect
    Serial.println("Message: " + message);

    // Scroll the message across the LED matrix
    matrix.beginDraw();
    matrix.beginText(0, 1, 0xFFFFFF);  // Set start position and colour
    matrix.println(message);           // Set scroll text
    matrix.endText(SCROLL_LEFT);       // Scroll from right to left
    matrix.endDraw();

    // Pause before displaying the next message
    delay(1000);  // 1-second pause between messages
  }

  // Free cursor memory
  delete cursor;
}

/**
 * @brief   Main program loop.
 *
 * @details Calls fetchMessagesAndScroll() every 30 seconds to check for
 *          new messages in the database and display them on the LED matrix.
 *
 * @note    Uses millis() to avoid blocking the program with delay().
 */
void loop() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 30000) {  // Check every 30 seconds
    fetchMessagesAndScroll();
    lastUpdate = millis();
  }
}
