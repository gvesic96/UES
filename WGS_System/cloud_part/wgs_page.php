<?php
$conn = new mysqli("localhost", "vesic.e116", "Fh4nC'(#", "db_vesic_e116");
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Fetch temperature data
$tempResult = $conn->query("SELECT time, temp FROM wgs_data");
$tempLabels = $tempData = "";

while ($row = $tempResult->fetch_assoc()) {
    if ($tempLabels != "") {
        $tempLabels .= ", ";
        $tempData .= ", ";
    }
    $tempLabels .= "'" . $row["time"] . "'";
    $tempData .= $row["temp"];
}

// Fetch humidity data
$humiResult = $conn->query("SELECT time, humi FROM wgs_data");
$humiLabels = $humiData = "";

while ($row = $humiResult->fetch_assoc()) {
    if ($humiLabels != "") {
        $humiLabels .= ", ";
        $humiData .= ", ";
    }
    $humiLabels .= "'" . $row["time"] . "'";
    $humiData .= $row["humi"];
}

// Fetch lumen data
$lumResult = $conn->query("SELECT time, lum FROM wgs_data");
$lumLabels = $lumData = "";

while ($row = $lumResult->fetch_assoc()) {
    if ($lumLabels != "") {
        $lumLabels .= ", ";
        $lumData .= ", ";
    }
    $lumLabels .= "'" . $row["time"] . "'";
    $lumData .= $row["lum"];
}

// Fetch heater data
$heaterResult = $conn->query("SELECT time, heater FROM wgs_data");
$heaterLabels = $heaterData = "";

while ($row = $heaterResult->fetch_assoc()) {
    if ($heaterLabels != "") {
        $heaterLabels .= ", ";
        $heaterData .= ", ";
    }
    $heaterLabels .= "'" . $row["time"] . "'";
    $heaterData .= $row["heater"];
}

//----------------------------------------------------------------------------------
$heaterResult = $conn->query("SELECT time, heater FROM wgs_data");

// Initialize variables for time calculation
$lastTime = null;
$lastHeaterStatus = null;
$totalOnTime = 0; // Total time heater was ON in seconds
$powerRating = 9.26; // Heater power rating in watts

while ($row = $heaterResult->fetch_assoc()) {
    // Get the current time and heater status
    $currentTime = strtotime($row["time"]); // Convert time to timestamp
    $heaterStatus = $row["heater"];
    
    if ($lastTime !== null && $lastHeaterStatus == 1) {
        // If the heater was ON and we have a previous timestamp, calculate the duration
        $totalOnTime += $currentTime - $lastTime;
    }
    
    // Update last time to the current row's time
    $lastTime = $currentTime;
	$lastHeaterStatus = $heaterStatus;
}

$conn->close();

$totalOnTimeHours = $totalOnTime / 3600;

// Calculate energy consumption in Wh
$energyConsumption = $totalOnTimeHours * $powerRating;

?>

<!DOCTYPE html>
<html>
	<head>
		
		
		
		<script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.6.0/Chart.min.js"></script>
		<!--<style>canvas{margin-left:auto; margin-right:auto;}</style>-->
		
		<title>Watermelon Guard System</title>
		
		<style>
        body {
            background-color: #d0f0c0; /* light green background */
            font-family: Arial, sans-serif;
        }
        
        canvas{margin-left:auto; margin-right:auto;}
		
		table {align:center; margin-left:auto; margin-right:auto;}
		table {font-family: arial, sans-serif;}
		td, th {text-align: left; padding: 8px;}
		tr:nth-child(even) {background-color: #dddddd;}
		tr:nth-child(odd) {background-color: #eeeeee;}
		
		</style>
	
	</head>
	
	<body>
		
		<h1 style="text-align:center;">Watermelon Guard System</h1>
			
	
		<canvas id="grafik" width = "400" height = "200"></canvas>
		<canvas id="grafik2" width = "400" height = "200"></canvas>
		<canvas id="grafik3" width = "400" height = "200"></canvas>
		<canvas id="grafik4" width = "400" height = "200"></canvas>
		
		
		<script>
			var ctx = document.getElementById("grafik").getContext("2d");
			var grafik = new Chart(ctx, {
				type: 'line',
				data: {
					labels: [<?php echo $tempLabels; ?>],
					datasets: [{
						label: 'Temperatura',
						data: [<?php echo $tempData; ?>],
						backgroundColor: '#ff0000',
						fill: false
					}]
				},
				options: {
					responsive: false,
					maintainAspectRatio: false
				}
			});
		
			var ctx2 = document.getElementById("grafik2").getContext("2d");
			var grafik2 = new Chart(ctx2, {
				type: 'line',
				data: {
					labels: [<?php echo $humiLabels; ?>],
					datasets: [{
						label: 'Vlaznost',
						data: [<?php echo $humiData; ?>],
						backgroundColor: '#0000ff',
						fill: false
					}]
				},
				options: {
					responsive: false,
					maintainAspectRatio: false
				}
			});
			
			var ctx3 = document.getElementById("grafik3").getContext("2d");
			var grafik3 = new Chart(ctx3, {
				type: 'line',
				data: {
					labels: [<?php echo $lumLabels; ?>],
					datasets: [{
						label: 'Osvetljenost',
						data: [<?php echo $lumData; ?>],
						backgroundColor: '#ffffff',
						fill: false
					}]
				},
				options: {
					responsive: false,
					maintainAspectRatio: false
				}
			});
			
			var ctx4 = document.getElementById("grafik4").getContext("2d");
			var grafik4 = new Chart(ctx4, {
				type: 'line',
				data: {
					labels: [<?php echo $heaterLabels; ?>],
					datasets: [{
						label: 'Grejac 9.26W',
						data: [<?php echo $heaterData; ?>],
						backgroundColor: '#ff0000',
						fill: false
					}]
				},
				options: {
					responsive: false,
					maintainAspectRatio: false
				}
			});
			
		</script>
		
		
		<table>
			<caption>Potrosnja grejaca</caption>
			<tr>
				<th>Ukupna kolicina utrosene energije (Wh)</th>
				<th><?php echo number_format($energyConsumption, 2); ?> Wh</th>
			</tr>
			
		</table>
	</body>
</html>