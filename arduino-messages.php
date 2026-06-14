<?php
/**
 * arduino-messages.php
 *
 * Web dashboard for managing messages displayed on the Arduino UNO R4 WiFi LED matrix.
 * Allows adding, editing, and deleting messages stored in the arduino_messages table.
 *
 * Libraries / dependencies used:
 * - config.php : External configuration file containing database credentials (DB_SERVER,
 *                DB_USERNAME, DB_PASSWORD, DB_NAME, DB_PORT).
 * - mysqli     : PHP built-in extension for connecting to and querying a MySQL/MariaDB database.
 *
 * (c) 2025 Richard, webwings.nl
 */

// Load database configuration settings from config.php
require_once 'config.php';

// Database connection variables (sourced from config.php constants)
$servername = DB_SERVER;        // Hostname, URL or IP address
$username   = DB_USERNAME;      // Database username
$password   = DB_PASSWORD;      // Database password
$dbname     = DB_NAME;          // Database name
$dbport     = DB_PORT;          // Database port (default: 3306)

/**
 * connectToDatabase()
 * Establishes a connection to the MySQL/MariaDB database using the
 * credentials defined in config.php. Terminates the script with an
 * error message if the connection cannot be established.
 *
 * @return mysqli  An active database connection object.
 */
function connectToDatabase($servername, $username, $password, $dbname, $dbport) {
    $conn = new mysqli($servername, $username, $password, $dbname, $dbport);
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }
    return $conn;
}

// Establish database connection
$conn = connectToDatabase($servername, $username, $password, $dbname, $dbport);

/**
 * addMessage()
 * Inserts a new message into the arduino_messages table.
 * The message text is sanitised using real_escape_string to prevent SQL injection.
 *
 * @param mysqli $conn     Active database connection.
 * @param string $message  The message text to insert.
 */
function addMessage($conn, $message) {
    $message = $conn->real_escape_string($message);
    $sql = "INSERT INTO arduino_messages (message_text) VALUES ('$message')";
    if ($conn->query($sql) === TRUE) {
        echo "Message added successfully!";
    } else {
        echo "Error: " . $conn->error;
    }
}

/**
 * editMessage()
 * Updates an existing message in the arduino_messages table by ID.
 * The ID is cast to an integer and the message text is sanitised to prevent SQL injection.
 *
 * @param mysqli $conn     Active database connection.
 * @param int    $id       The ID of the message to update.
 * @param string $message  The new message text.
 */
function editMessage($conn, $id, $message) {
    $id      = intval($id);
    $message = $conn->real_escape_string($message);
    $sql = "UPDATE arduino_messages SET message_text='$message' WHERE id=$id";
    if ($conn->query($sql) === TRUE) {
        echo "Message updated successfully!";
    } else {
        echo "Error: " . $conn->error;
    }
}

/**
 * deleteMessage()
 * Deletes a message from the arduino_messages table by ID.
 * The ID is cast to an integer to prevent SQL injection.
 *
 * @param mysqli $conn  Active database connection.
 * @param int    $id    The ID of the message to delete.
 */
function deleteMessage($conn, $id) {
    $id  = intval($id);
    $sql = "DELETE FROM arduino_messages WHERE id=$id";
    if ($conn->query($sql) === TRUE) {
        echo "Message deleted successfully!";
    } else {
        echo "Error: " . $conn->error;
    }
}

// Handle POST requests
if (isset($_POST['add'])) {
    addMessage($conn, $_POST['message']);
}
if (isset($_POST['edit'])) {
    editMessage($conn, $_POST['id'], $_POST['message']);
}
if (isset($_POST['delete'])) {
    deleteMessage($conn, $_POST['id']);
}

/**
 * getMessages()
 * Retrieves all messages from the arduino_messages table,
 * ordered by creation date (newest first).
 *
 * @param mysqli $conn  Active database connection.
 * @return mysqli_result  Query result containing all messages.
 */
function getMessages($conn) {
    $sql = "SELECT * FROM arduino_messages ORDER BY created_at DESC";
    return $conn->query($sql);
}

// Retrieve all messages for display
$result = getMessages($conn);
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Arduino Message Dashboard</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            background-color: #f9f9f9;
        }

        .container {
            max-width: 1200px;
            margin: 0 auto;
            background-color: white;
            border-radius: 8px;
            padding: 20px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        }

        h1, h2 {
            text-align: center;
            color: #333;
        }

        table {
            width: 100%;
            border-collapse: collapse;
            margin: 20px 0;
        }

        table th {
            border: 1px solid #ddd;
            padding: 10px;
            text-align: center;
        }

        table td {
            border: 1px solid #ddd;
            padding: 10px;
            text-align: left;
        }

        table th {
            background-color: rgb(77, 68, 182);
            color: white;
        }

        table tr:nth-child(even) {
            background-color: #f2f2f2;
        }

        form {
            display: flex;
            justify-content: center;
            gap: 10px;
            margin: 20px 0;
        }

        form input, form button {
            padding: 10px;
            border: 1px solid #ccc;
            border-radius: 4px;
            font-size: 14px;
        }

        form button {
            background-color: rgb(101, 110, 248);
            color: white;
            cursor: pointer;
            border: none;
        }

        form button:hover {
            background-color: rgb(57, 96, 196);
        }

        a {
            display: block;
            text-align: center;
            margin: 20px 0;
            text-decoration: none;
            color: rgb(66, 61, 219);
        }

        a:hover {
            text-decoration: underline;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Arduino UNO R4 LED Matrix</h1>

        <!-- Add message form -->
        <h2>Add Message</h2>
        <form method="POST">
            <input type="text" name="message" placeholder="New message" required>
            <button type="submit" name="add">Add</button>
        </form><br><br>

        <!-- Message list -->
        <h2>Current Messages</h2>
        <table>
            <tr>
                <th>ID</th>
                <th>Message</th>
                <th>Created at</th>
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
                                <button type='submit' name='delete' onclick='return confirm(\"Are you sure you want to delete this message?\")'>Delete</button>
                            </form>
                        </td>
                    </tr>";
                }
            } else {
                echo "<tr><td colspan='4'>No messages found.</td></tr>";
            }
            ?>
        </table>
    </div>
</body>
</html>
