/* SQL File to create the database and table containing some example messages */
/* for the Arduino UNO R4 Wifi with the onboard LED matrix display            */
/* (c)2025 Richard, webwings.nl                                               */

/* create database */
CREATE DATABASE arduino;
USE arduino;

/* Create messages table */
CREATE TABLE arduino_messages (
    id INT AUTO_INCREMENT PRIMARY KEY,
    message_text VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

/* Example messages */
INSERT INTO arduino_messages (message_text) VALUES 
('Hello Arduino User!'),
('Happy Experimenting!'),
('Scrolling Textdisplay!');