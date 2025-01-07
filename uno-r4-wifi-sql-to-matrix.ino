// Berichten vanaf een MariaDB/SQL Server laten zien op een
// Matrix Display van een Arduino UNO R4 Wifi.
// (c) 2025 Richard, webwings.nl 

#include "secrets.h"                    // Wifi & Database gegevens.
#include <WiFiS3.h>                     // WiFi-bibliotheek voor UNO R4 WiFi
#include <MySQL_Connection.h>           // MySQL bibliotheek
#include <MySQL_Cursor.h>               // Voor SQL-query's
#include <ArduinoGraphics.h>            // Grafische bibliotheek
#include <Arduino_LED_Matrix.h>         // LED-matrix ondersteuning

// WiFi-instellingen
const char* ssid = WIFI_SSID;           // Gebruik gegevens uit secrets.h
const char* password = WIFI_PASSWORD;   // Gebruik gegevens uit secrets.h

// MySQL database instellingen
const char* server_url = DB_HOST;       // Gebruik gegevens uit secrets.h
IPAddress server_ip;                    // IP-adres dat via DNS wordt opgehaald
const int server_port = DB_PORT;        // Gebruik gegevens uit secrets.h
char user[20];                          // Maak een array van vaste grootte
char password_db[20];                   // Voor wachtwoord
char database[32];                      // Ook voor de te gebruiken database

// Database en query
const char query[] = "SELECT message_text FROM arduino_messages ORDER BY created_at DESC;";

// WiFi- en MySQL-objecten
WiFiClient client;
MySQL_Connection conn((Client *)&client);

// LED-matrix object
ArduinoLEDMatrix matrix;

// Variabelen voor berichtenbeheer
String scrollText = " Verbinding maken... ";  // Starttekst
bool newTextAvailable = false;

// Setup
void setup() {
  // Variabelen voor de gebruiker en de database uit secrets.h
  strcpy(user, DB_USER);                  // Kopieer waarde uit secrets.h
  strcpy(password_db, DB_PASSWORD);       // Kopieer wachtwoord uit secrets.h
  strcpy(database, DB_NAME);              // Kopieer database naam uit secrets.h

  // Seriële monitor starten
  Serial.begin(115200);
  while (!Serial);

  // Verbinden met WiFi
  connectToWiFi();

  // DNS-resolutie uitvoeren
  Serial.print("Resolving hostname: ");
  Serial.println(server_url);
  if (!WiFi.hostByName(server_url, server_ip)) {
    Serial.println("DNS-resolutie mislukt!");
    while (1); // Stop als DNS niet werkt
  }
  Serial.print("Server IP-adres: ");
  Serial.println(server_ip);

  // Verbinden met MySQL-server
  connectToDatabase();

  // Initialiseer LED-matrix
  matrix.begin();
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);           // Witte tekstkleur
  matrix.textScrollSpeed(50);          // Scrollsnelheid instellen
  matrix.textFont(Font_5x7);           // Lettertype instellen
  matrix.endDraw();
}

// Functie om verbinding te maken met WiFi
void connectToWiFi() {
  Serial.print("Verbinding maken met WiFi...");
  WiFi.begin(ssid, password);
  int retries = 20;  // Maximaal 20 pogingen
  while (WiFi.status() != WL_CONNECTED && retries > 0) {
    delay(500);
    Serial.print(".");
    retries--;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Verbonden!");
  } else {
    Serial.println("\nWiFi Verbinden mislukt. Controleer instellingen!");
    while (1);  // Stop programma
  }
}

// Functie om verbinding te maken met MySQL
void connectToDatabase() {
  Serial.print("Verbinden met MySQL-server...");
  int retries = 3; // Maximaal 3 pogingen
  while (retries > 0 && !conn.connected()) {
    if (conn.connect(server_ip, server_port, user, password_db)) {
      Serial.println(" Verbonden!");

      // Database selecteren
      MySQL_Cursor* cursor = new MySQL_Cursor(&conn);
      char use_db[50];
      sprintf(use_db, "USE %s;", database);
      cursor->execute(use_db);  // Selecteer database
      delete cursor;
      Serial.println("Database geselecteerd!");
      return;
    } else {
      retries--;
      Serial.println(" Verbindingspoging mislukt. Opnieuw proberen...");
      delay(2000); // Wacht 2 seconden en probeer opnieuw
    }
  }

  Serial.println("Kan geen verbinding maken met de database. Stop programma.");
  while (1);  // Stop het programma
}

// Functie om berichten op te halen en weer te geven
void fetchMessagesAndScroll() {
  // Controleer WiFi-verbinding
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi-verbinding verloren. Opnieuw verbinden...");
    connectToWiFi();
  }

  // Controleer MySQL-verbinding
  if (!conn.connected()) {
    Serial.println("Databaseverbinding verloren. Opnieuw verbinden...");
    connectToDatabase();
  }

  // Query uitvoeren
  MySQL_Cursor* cursor = new MySQL_Cursor(&conn);
  if (!cursor->execute(query)) {
    Serial.println("Fout bij uitvoeren van query!");
    delete cursor;
    return;
  }

  // Kolomnamen ophalen
  column_names *cols = cursor->get_columns();

  // Resultaten ophalen en weergeven
  row_values *row;
  while ((row = cursor->get_next_row())) { // Doorloop alle rijen
    String message = row->values[0];      // Haal bericht op
    message = "     " + message;          // Voeg spaties toe voor scroll-effect
    Serial.println("Bericht: " + message);

    // Scroll het bericht op de LED-matrix
    matrix.beginDraw();
    matrix.beginText(0, 1, 0xFFFFFF);  // Positie en kleur
    matrix.println(message);           // Scroll de tekst
    matrix.endText(SCROLL_LEFT);       // Scroll van rechts naar links
    matrix.endDraw();

    // Wacht even voordat het volgende bericht komt
    delay(5000);  // 5 seconden pauze tussen berichten
  }

  // Cursor opruimen
  delete cursor;
}

// Hoofdprogramma
void loop() {
  // Controleer elke 30 seconden op updates
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 30000) { // 30 seconden vertraging
    fetchMessagesAndScroll();  // Haal berichten op en scroll ze
    lastUpdate = millis();
  }
}