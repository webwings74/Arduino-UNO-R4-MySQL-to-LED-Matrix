CREATE DATABASE arduino;
USE arduino;

CREATE TABLE arduino_messages (
    id INT AUTO_INCREMENT PRIMARY KEY,
    message_text VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

INSERT INTO arduino_messages (message_text) VALUES 
('Hello Arduino User!'),
('Happy Experimenting!'),
('Scrolling Textdisplay!');