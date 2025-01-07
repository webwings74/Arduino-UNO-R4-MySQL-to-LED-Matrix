<?php
// Laad configuratie-instellingen
require_once 'config.php';

// Database configuratie
$servername = DB_SERVER;        // Of gebruik "127.0.0.1"
$username = DB_USERNAME;        // Databasegebruiker
$password = DB_PASSWORD;        // Wachtwoord
$dbname = DB_NAME;              // Database naam
$dbport = DB_PORT;              // Database poort

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
    <title>Berichtenbeheer</title>
</head>
<body>
    <h1>Berichtenbeheer voor Arduino LED-matrix</h1>

    <!-- Bericht toevoegen -->
    <h2>Bericht toevoegen:</h2>
    <form method="POST">
        <input type="text" name="message" placeholder="Nieuw bericht" required>
        <button type="submit" name="add">Toevoegen</button>
    </form>

    <!-- Lijst van berichten -->
    <h2>Huidige berichten:</h2>
    <table border="1">
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
</body>
</html>

<?php
// Verbinding sluiten
$conn->close();
?>