<?php
// Laad configuratie-instellingen
require_once 'config.php';

// Database configuratie
$servername = DB_SERVER;        // localhost, URL of IP in config.php
$username = DB_USERNAME;        // Database gebruikersnaam
$password = DB_PASSWORD;        // Database Wachtwoord
$dbname = DB_NAME;              // Database naam (arduino)
$dbport = DB_PORT;              // Database poort (Standaard 3306)

// Verbinding maken met de database
$conn = new mysqli($servername, $username, $password, $dbname, $dbport);

// Controleer verbinding
if ($conn->connect_error) {
    die("Verbinding mislukt: " . $conn->connect_error);
}

// Bericht toevoegen
if (isset($_POST['add'])) {
    $message = $conn->real_escape_string($_POST['message']);
    $sql = "INSERT INTO arduino_messages (message_text) VALUES ('$message')";
    if ($conn->query($sql) === TRUE) {
        echo "Bericht toegevoegd!";
    } else {
        echo "Fout: " . $conn->error;
    }
}

// Bericht bijwerken
if (isset($_POST['edit'])) {
    $id = intval($_POST['id']);
    $message = $conn->real_escape_string($_POST['message']);
    $sql = "UPDATE arduino_messages SET message_text='$message' WHERE id=$id";
    if ($conn->query($sql) === TRUE) {
        echo "Bericht bijgewerkt!";
    } else {
        echo "Fout: " . $conn->error;
    }
}

// Bericht verwijderen
if (isset($_POST['delete'])) {
    $id = intval($_POST['id']);
    $sql = "DELETE FROM arduino_messages WHERE id=$id";
    if ($conn->query($sql) === TRUE) {
        echo "Bericht verwijderd!";
    } else {
        echo "Fout: " . $conn->error;
    }
}

// Alle berichten ophalen
$sql = "SELECT * FROM arduino_messages ORDER BY created_at DESC";
$result = $conn->query($sql);
?>

<!DOCTYPE html>
<html lang="nl">
<head>
    <meta charset="UTF-8">
    <title>Arduino Berichtenbeheer</title>
    <link rel="stylesheet" href="style.css">
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
            background-color:rgb(77, 68, 182);
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
            background-color:rgb(101, 110, 248);
            color: white;
            cursor: pointer;
            border: none;
        }

        form button:hover {
            background-color:rgb(57, 96, 196);
        }

        a {
            display: block;
            text-align: center;
            margin: 20px 0;
            text-decoration: none;
            color:rgb(66, 61, 219);
        }

        a:hover {
            text-decoration: underline;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Arduino UNO R4 LED-Matrix</h1>

        <!-- Bericht toevoegen -->
        <h2>Bericht toevoegen</h2>
        <form method="POST">
            <input type="text" name="message" placeholder="Nieuw bericht" required>
            <button type="submit" name="add">Toevoegen</button>
        </form><br><br>

        <!-- Lijst van berichten -->
        <h2>Huidige berichten</h2>
        <table>
            <tr>
                <th>ID</th>
                <th>Bericht</th>
                <th>Aangemaakt op</th>
                <th>Acties</th>
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
                                <button type='submit' name='edit'>Wijzigen</button>
                            </form>
                            <form method='POST' style='display:inline;'>
                                <input type='hidden' name='id' value='{$row['id']}'>
                                <button type='submit' name='delete' onclick='return confirm(\"Weet je zeker dat je dit bericht wilt verwijderen?\")'>Verwijderen</button>
                            </form>
                        </td>
                    </tr>";
                }
            } else {
                echo "<tr><td colspan='4'>Geen berichten gevonden.</td></tr>";
            }
            ?>
        </table>
        <a href="https://code.webwings.nl/crud-countdown.php" target="_blank">Beheer Event Dashboard</a>
    </div>
</body>
</html>