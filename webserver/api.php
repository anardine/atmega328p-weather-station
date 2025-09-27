<?php
// filepath: api.php (upload this to the root folder of your domain. For Example: www.yourdomain.com/api.php)

header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET'); // Changed for GET requests
header('Access-Control-Allow-Headers: Content-Type');

// Database configuration
$servername = "localhost"; // HostGator typically uses localhost. Check if other hosting agents are different
$username = "database_username";
$password = "database_pwd";
$dbname = "database_name";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
    http_response_code(500);
    echo json_encode(['status' => 'error', 'message' => 'Database connection failed']);
    exit();
}

// Only allow GET requests
if ($_SERVER['REQUEST_METHOD'] !== 'GET') {
    http_response_code(405);
    echo json_encode(['status' => 'error', 'message' => 'Only GET method allowed']);
    exit();
}

// Get GET data
$sensor_type = isset($_GET['type']) ? $_GET['type'] : '';
$sensor_name = isset($_GET['sensor']) ? $_GET['sensor'] : '';
$value = isset($_GET['value']) ? $_GET['value'] : '';
$unit = isset($_GET['unit']) ? $_GET['unit'] : '';
$device = isset($_GET['device']) ? $_GET['device'] : '';

// Validate required fields
if (empty($sensor_type) || empty($sensor_name) || $value === '') {
    http_response_code(400);
    echo json_encode(['status' => 'error', 'message' => 'Missing required fields']);
    exit();
}

// Prepare and execute SQL statement
$sql = "INSERT INTO sensor_data (sensor_type, sensor_name, value, unit, device, timestamp) VALUES (?, ?, ?, ?,?, NOW())";
$stmt = $conn->prepare($sql);

if (!$stmt) {
    http_response_code(500);
    echo json_encode(['status' => 'error', 'message' => 'SQL prepare failed']);
    exit();
}

// Ensure correct data types for binding
$stmt->bind_param("ssdss", $sensor_type, $sensor_name, $value, $unit, $device);

if ($stmt->execute()) {
    http_response_code(200);
    echo json_encode(['status' => 'success', 'message' => 'Data saved successfully', 'id' => $conn->insert_id]);
} else {
    http_response_code(500);
    echo json_encode(['status' => 'error', 'message' => 'Failed to save data']);
}

$stmt->close();
$conn->close();
?>