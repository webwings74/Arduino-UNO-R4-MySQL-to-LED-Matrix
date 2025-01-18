<?php
// Arduino UNO R4 Wifi displaying messages on the onboard LED Matrix display from a (remote)
// MariaDB/MySQL database server. This PHP script adds, edits and deletes the messages in the
// arduino_messages table.
// (c)2025 Richard, webwings.nl

// Load Database Configuration
require_once 'config.php';

// Set Database Configuration
$servername = DB_SERVER;        // This can be localhost, an URL or an IP address
$username = DB_USERNAME;        // Database User Account
$password = DB_PASSWORD;        // Database User Password
$dbname = DB_NAME;              // Database Name, containing the messages table.
$dbport = DB_PORT;              // Database Port, can be different from 3306.

// Connecting to the database
$conn = new mysqli($servername, $username, $password, $dbname, $dbport);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Add a message
if (isset($_POST['add'])) {
    $message = $conn->real_escape_string($_POST['message']);
    $sql = "INSERT INTO arduino_messages (message_text) VALUES ('$message')";
    if ($conn->query($sql) === TRUE) {
        echo "Message added!";
    } else {
        echo "Error: " . $conn->error;
    }
}

// Edit message
if (isset($_POST['edit'])) {
    $id = intval($_POST['id']);
    $message = $conn->real_escape_string($_POST['message']);
    $sql = "UPDATE arduino_messages SET message_text='$message' WHERE id=$id";
    if ($conn->query($sql) === TRUE) {
        echo "Message edited!";
    } else {
        echo "Error: " . $conn->error;
    }
}

// Delete message
if (isset($_POST['delete'])) {
    $id = intval($_POST['id']);
    $sql = "DELETE FROM arduino_messages WHERE id=$id";
    if ($conn->query($sql) === TRUE) {
        echo "Message deleted!";
    } else {
        echo "Error: " . $conn->error;
    }
}

// Get all messages
$sql = "SELECT * FROM arduino_messages ORDER BY created_at DESC";
$result = $conn->query($sql);
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Arduino UNO Messages</title>
    <!-- Koppeling naar externe CSS-bestand -->
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <h1>Arduino UNO R4 Wifi LED-matrix</h1>

    <!-- Add message -->
    <h2>Add message:</h2>
    <form method="POST">
        <input type="text" name="message" placeholder="New message" required>
        <button type="submit" name="add">Add</button>
    </form>

    <!-- List of messages -->
    <h2>Current messages:</h2>
    <table border="1">
        <tr>
            <th>ID</th>
            <th>Message</th>
            <th>Created on</th>
            <th>Actions</th>
        </tr>
        <?php
        if ($result->num_rows > 0) {
            while ($row = $result->fetch_assoc()) {
                echo "<tr>
                    <td>{$row['id']}</td>
                    <td>{$row['message_text']}</td>
                    <td>{$row['created_at']}</td>
                    <td>
                        <form method='POST' style='display:inline;'>
                            <input type='hidden' name='id' value='{$row['id']}'>
                            <input type='text' name='message' value='{$row['message_text']}' required>
                            <button type='submit' name='edit'>Edit</button>
                        </form>
                        <form method='POST' style='display:inline;'>
                            <input type='hidden' name='id' value='{$row['id']}'>
                            <button type='submit' name='delete' onclick='return confirm(\"Are you sure to delete?\")'>Delete</button>
                        </form>
                    </td>
                </tr>";
            }
        } else {
            echo "<tr><td colspan='4'>No messages found.</td></tr>";
        }
        ?>
    </table>
</body>
</html>

<?php
// Close connection
$conn->close();
?>
