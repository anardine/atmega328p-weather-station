<?php
// filepath: php_api/fetch_data.php
// Read-only API to fetch last N hours/days of weather data and latest indicators
// Place this file in the same directory as api.php and ensure DB credentials match.

header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET');
header('Access-Control-Allow-Headers: Content-Type');

// Database configuration (copy from api.php)
$servername = "localhost"; // Typically localhost on shared hosts
$username = "database_username";
$password = "database_pwd";
$dbname = "database_name";

// Connect
$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
    http_response_code(500);
    echo json_encode(['status' => 'error', 'message' => 'Database connection failed']);
    exit();
}

// Parse parameters
$range = isset($_GET['range']) ? trim($_GET['range']) : '24h';
$deviceParam = isset($_GET['device']) ? trim($_GET['device']) : '';
$device = $conn->real_escape_string($deviceParam);

// Range supports values like 6h, 12h, 24h, 48h, 7d
$interval_sql = 'INTERVAL 24 HOUR';
if (preg_match('/^(\d+)([hd])$/i', $range, $m)) {
    $qty = intval($m[1]);
    $unit = strtolower($m[2]) === 'd' ? 'DAY' : 'HOUR';
    if ($qty > 0 && $qty <= 31) {
        $interval_sql = 'INTERVAL ' . $qty . ' ' . ($unit === 'DAY' ? 'DAY' : 'HOUR');
    }
}

// Canonical sensor types we care about
$wanted = ['temperature','humidity','pressure','rain'];
$wantedSql = "('temperature','humidity','pressure','rain')";

// Optional device filter (treat empty or 'all' as no filter)
$deviceWhere = '';
if ($device !== '' && strtolower($device) !== 'all') {
    $deviceWhere = " AND device = '" . $device . "' ";
}

// Fetch time series for selected range (and device if provided)
$sql = "SELECT sensor_type, value, UNIX_TIMESTAMP(timestamp) AS ts
        FROM sensor_data
        WHERE timestamp >= NOW() - $interval_sql
          AND sensor_type IN $wantedSql" . $deviceWhere .
       " ORDER BY timestamp ASC";

$result = $conn->query($sql);

$series = [
    'temperature' => [],
    'humidity'    => [],
    'pressure'    => [],
    'rain'        => []
];

if ($result) {
    while ($row = $result->fetch_assoc()) {
        $name = strtolower($row['sensor_type']);
        if (!in_array($name, $wanted, true)) continue;
        $series[$name][] = [
            't' => (int)$row['ts'] * 1000, // ms for JS Date
            'v' => is_numeric($row['value']) ? (float)$row['value'] : null
        ];
    }
}

// Compute latest values for indicators as the most recent datapoint overall (not limited by range)
$latest = [
    'temperature' => null,
    'humidity'    => null,
    'pressure'    => null,
    'rain'        => null
];

$sqlLatest = "SELECT sensor_type, value
              FROM sensor_data
              WHERE sensor_type IN $wantedSql" . $deviceWhere .
            " ORDER BY timestamp DESC
              LIMIT 200"; // fetch a few, then pick first per sensor_type
$resLatest = $conn->query($sqlLatest);
if ($resLatest) {
    $seen = [];
    while ($row = $resLatest->fetch_assoc()) {
        $name = strtolower($row['sensor_type']);
        if (!in_array($name, $wanted, true)) continue;
        if (isset($seen[$name])) continue;
        $seen[$name] = true;
        $latest[$name] = is_numeric($row['value']) ? (float)$row['value'] : null;
        // Stop early if we have all
        if (count($seen) === count($wanted)) break;
    }
}

// Load device list for the dropdown
$devices = [];
$resDevices = $conn->query("SELECT DISTINCT device FROM sensor_data WHERE device IS NOT NULL AND device <> '' ORDER BY device ASC");
if ($resDevices) {
    while ($row = $resDevices->fetch_assoc()) {
        $devices[] = $row['device'];
    }
}

// Prepare payload
$payload = [
    'status'  => 'success',
    'range'   => $range,
    'device'  => ($device === '' ? 'all' : $device),
    'devices' => $devices,
    'latest'  => [
        'temperature' => $latest['temperature'],
        'humidity'    => $latest['humidity'],
        'pressure'    => $latest['pressure'],
        // Consider any value >= 0.5 as raining (if sensor provides 0/1)
        'rain'        => is_null($latest['rain']) ? null : ((float)$latest['rain'] >= 0.5)
    ],
    'series'  => $series
];

echo json_encode($payload);

$conn->close();
