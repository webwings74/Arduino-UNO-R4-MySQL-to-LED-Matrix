// Show messages from a MariaDB/SQL Server on the
// Matrix Display of a Arduino UNO R4 Wifi.
// (c) 2025 Richard, webwings.nl 

#include "secrets.h"                    // Wifi & Database credentials.
#include <WiFiS3.h>                     // WiFi-library for the UNO R4 WiFi
#include <MySQL_Connection.h>           // MySQL library
#include <MySQL_Cursor.h>               // For SQL-queries
#include <ArduinoGraphics.h>            // Graphic library
#include <Arduino_LED_Matrix.h>         // LED-matrix support

// WiFi-Setup
const char* ssid = WIFI_SSID;           // Use data from secrets.h
const char* password = WIFI_PASSWORD;   // Use data from secrets.h

// MySQL database instellingen
const char* server_url = DB_HOST;       // Use credentials from secrets.h
IPAddress server_ip;                    // IP-adress that is retrieved by DNS
const int server_port = DB_PORT;        // Use port from secrets.h
char user[20];                          // Username, define array
char password_db[20];                   // Password, define array
char database[32];                      // Database name, used to store the table.

// Database SQL Query to retrieve messages
const char query[] = "SELECT message_text FROM arduino_messages ORDER BY created_at DESC;";

// WiFi- en MySQL-objects
WiFiClient client;
MySQL_Connection conn((Client *)&client);

// LED-matrix object
ArduinoLEDMatrix matrix;

// Message variables
String scrollText = " Verbinding maken... ";  // Start text
bool newTextAvailable = false;

// Setup
void setup() {
  // Variabelen voor de gebruiker en de database uit secrets.h
  strcpy(user, DB_USER);                  // Database Username from secrets.h
  strcpy(password_db, DB_PASSWORD);       // Password from secrets.h
  strcpy(database, DB_NAME);              // Database from secrets.h

  // Start serial monitor
  Serial.begin(115200);
  while (!Serial);

  // Connect to WiFi
  connectToWiFi();

  // DNS resolve 
  Serial.print("Resolving hostname: ");
  Serial.println(server_url);
  if (!WiFi.hostByName(server_url, server_ip)) {
    Serial.println("DNS-resolution failed!");
    while (1); // Stop if DNS does not work
  }
  Serial.print("Server IP-adres: ");
  Serial.println(server_ip);

  // Connecct to MySQL-server
  connectToDatabase();

  // Init LED-matrix
  matrix.begin();
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);           // White textcolor
  matrix.textScrollSpeed(50);          // Scrollspeed
  matrix.textFont(Font_5x7);           // Font select
  matrix.endDraw();
}

// Function to connect to wifi
void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  int retries = 20;  // Max 20 retries
  while (WiFi.status() != WL_CONNECTED && retries > 0) {
    delay(500);
    Serial.print(".");
    retries--;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
  } else {
    Serial.println("\nWiFi Verbinden mislukt. Controleer instellingen!");
    while (1);  // Stop programma
  }
}

// Function to connect to MySQL
void connectToDatabase() {
  Serial.print("Connecting to MySQL-server...");
  int retries = 3; // Max 3 retries
  while (retries > 0 && !conn.connected()) {
    if (conn.connect(server_ip, server_port, user, password_db)) {
      Serial.println(" Connected!");

      // Database select
      MySQL_Cursor* cursor = new MySQL_Cursor(&conn);
      char use_db[50];
      sprintf(use_db, "USE %s;", database);
      cursor->execute(use_db);  // Select database
      delete cursor;
      Serial.println("Database selected!");
      return;
    } else {
      retries--;
      Serial.println(" Connecction failed. Trying again...");
      delay(2000); // Wait 2 seconds
    }
  }

  Serial.println("Cannot connect to the database. Stopping programm.");
  while (1);  // Stop programm
}

// Function to fetch messages and display them
void fetchMessagesAndScroll() {
  // Check Wifi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi-connection lost. Connecting again...");
    connectToWiFi();
  }

  // Check MySQL-connection
  if (!conn.connected()) {
    Serial.println("Database connection lost. Connecting again...");
    connectToDatabase();
  }

  // Execute Query
  MySQL_Cursor* cursor = new MySQL_Cursor(&conn);
  if (!cursor->execute(query)) {
    Serial.println("Fout bij uitvoeren van query!");
    delete cursor;
    return;
  }

  // Retrieve column names
  column_names *cols = cursor->get_columns();

  // Retrieve results and display on LED matrix
  row_values *row;
  while ((row = cursor->get_next_row())) {  // Cycle all rows sequentially
    String message = row->values[0];        // Get message
    message = "     " + message;            // Add spaces for scrolling effect
    Serial.println("Bericht: " + message);

    // Scroll message on the LED-matrix
    matrix.beginDraw();
    matrix.beginText(0, 1, 0xFFFFFF);  // Position and colour
    matrix.println(message);           // Scroll text
    matrix.endText(SCROLL_LEFT);       // Scroll right to left
    matrix.endDraw();

    // Wait a moment for the next message
    delay(1000);  // 1 second break between messages
  }

  // Clear cursor
  delete cursor;
}

// Main programm
void loop() {
  // Check every 30 seconds for message updates.
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 30000) {  // 30 seconds delay
    fetchMessagesAndScroll();           // Check for messages and retrieve them
    lastUpdate = millis();
  }
}