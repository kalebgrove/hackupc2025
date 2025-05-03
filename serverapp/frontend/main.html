<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Airport Information System</title>
    <link rel="stylesheet" href="style.css">
    <script src="script.js" defer></script>
</head>
<body>
    <div class="container">
        <header>
            <h1>Airport Information</h1>
            <div id="blackout-status">Operating Normally</div>
            <div id="loading-indicator" class="loading">Loading...</div>
        </header>
        <main>
            <section id="flight-section">
                <h2>Flights</h2>
                <div id="flight-table-container">
                    <table id="flight-table">
                        <thead>
                            <tr>
                                <th>Flight Number</th>
                                <th>Gate</th>
                                <th>Boarding Time</th>
                                <th>Departure Time</th>
                                <th>Status</th>
                                <th>Update Status</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr>
                                <td colspan="6">No flights available</td>
                            </tr>
                        </tbody>
                    </table>
                </div>
            </section>
            <section id="weather-section">
                <h2>Weather</h2>
                <div id="weather-info">
                    <p>Loading weather...</p>
                </div>
            </section>
            <section id="catastrophe-section">
                <h2>Nearby Catastrophes</h2>
                <div id="catastrophe-info">
                    <p>Loading catastrophes...</p>
                </div>
            </section>
            <section id="map-section">
                <h2>Airport Map</h2>
                <img id="airport-map" src="map" alt="Airport Map">
            </section>
             <section id="controls-section">
                <h2>Controls</h2>
                <button id="refresh-button">Refresh Data</button>
            </section>
        </main>
        <footer>
            <p>&copy; 2024 Airport Info System</p>
        </footer>
    </div>
</body>
</html>
```css
body {
    font-family: Arial, sans-serif;
    margin: 0;
    padding: 0;
    background-color: #f4f4f4;
    color: #333;
    display: flex;
    flex-direction: column;
    min-height: 100vh;
}

.container {
    max-width: 1200px;
    margin: 0 auto;
    padding: 20px;
    background-color: #fff;
    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
    border-radius: 8px;
    flex: 1;
    display: flex;
    flex-direction: column;
}

header {
    text-align: center;
    padding: 20px 0;
    border-bottom: 1px solid #ddd;
    display: flex; /* Use flexbox for layout */
    flex-direction: column;
    align-items: center; /* Center items horizontally */
}

h1 {
    margin: 0;
    font-size: 24px;
    margin-bottom: 10px;
}

#blackout-status {
    font-size: 16px;
    color: green;
    margin-bottom: 10px;
}


main {
    flex: 1;
    display: flex;
    flex-direction: column;
    gap: 20px;
}

section {
    padding: 20px;
    border-radius: 8px;
    background-color: #f9f9f9;
}

h2 {
    margin: 0 0 10px 0;
    font-size: 20px;
}

#flight-table-container {
    overflow-x: auto;
}

#flight-table {
    width: 100%;
    border-collapse: collapse;
    margin-top: 10px;
}

#flight-table th, #flight-table td {
    padding: 10px;
    border: 1px solid #ddd;
    text-align: left;
}

#flight-table th {
    background-color: #f0f0f0;
}

#flight-table tbody tr:nth-child(odd) {
    background-color: #fff;
}

#flight-table tbody tr:nth-child(even) {
    background-color: #f5f5f5;
}

#weather-info, #catastrophe-info {
    padding: 10px;
    border: 1px solid #ddd;
    border-radius: 8px;
    background-color: #fff;
}

#airport-map {
    max-width: 100%;
    height: auto;
    border: 1px solid #ddd;
    border-radius: 8px;
}

footer {
    text-align: center;
    padding: 10px 0;
    border-top: 1px solid #ddd;
    margin-top: 20px;
}

.loading {
    text-align: center;
    font-style: italic;
    color: #888;
}

#controls-section {
    display: flex;
    justify-content: center;
    margin-top: 20px;
}

#refresh-button {
    padding: 10px 20px;
    background-color: #4CAF50;
    color: white;
    border: none;
    border-radius: 5px;
    cursor: pointer;
    font-size: 16px;
}

#refresh-button:hover {
    background-color: #367c39;
}

/* Responsive adjustments */
@media screen and (max-width: 768px) {
    .container {
        padding: 10px;
    }
    #flight-table th, #flight-table td {
        padding: 8px;
    }
    h1 {
        font-size: 20px;
    }
    h2 {
        font-size: 18px;
    }
    main {
        flex-direction: column;
    }
    section {
        padding: 10px;
    }
    #flight-table-container {
        overflow-x: auto;
    }
}
```javascript
const flightTable = document.getElementById('flight-table');
const weatherInfo = document.getElementById('weather-info');
const catastropheInfo = document.getElementById('catastrophe-info');
const loadingIndicator = document.getElementById('loading-indicator');
const refreshButton = document.getElementById('refresh-button');
const blackoutStatusElement = document.getElementById('blackout-status');


let isBlackout = false; // Global variable in JavaScript

// Helper function to update the DOM with flight data
function updateFlightTable(data) {
    const tbody = flightTable.querySelector('tbody');
    tbody.innerHTML = ''; // Clear existing rows

    if (data && data.length > 0) {
        data.forEach(flight => {
            const row = document.createElement('tr');
            row.innerHTML = `
                <td>${flight.flight_number}</td>
                <td>${flight.flight_gate}</td>
                <td>${flight.boarding_time}</td>
                <td>${flight.departure_time}</td>
                <td>${flight.status}</td>
                <td><button data-flight="${flight.flight_number}">Update Status</button></td>
            `;
            tbody.appendChild(row);

            // Add event listener to the "Update Status" button.
            const updateButton = row.querySelector('button');
            updateButton.addEventListener('click', () => {
                const newStatus = prompt(`Enter new status for flight ${flight.flight_number}:`);
                if (newStatus) {
                    // Send an AJAX request to the server to update the status
                    fetch(`/updateFlightStatus?flightNumber=${flight.flight_number}&status=${encodeURIComponent(newStatus)}`)
                        .then(response => {
                            if (response.ok) {
                                alert(`Status for flight ${flight.flight_number} updated successfully.`);
                                fetchData(); // Refresh the data
                            } else {
                                alert(`Failed to update status: ${response.statusText}`);
                            }
                        })
                        .catch(error => {
                            console.error('Error updating flight status:', error);
                            alert('Error updating flight status.');
                        });
                }
            });
        });
    } else {
        tbody.innerHTML = '<tr><td colspan="6">No flights available</td></tr>';
    }
}

// Helper function to update the DOM with weather data
function updateWeatherInfo(data) {
    if (data && data.length > 0) {
        const weather = data[0]; // Assuming the API returns an array
        weatherInfo.innerHTML = `
            <p><strong>Conditions:</strong> ${weather.conditions}</p>
            <p><strong>Temperature:</strong> ${weather.temperature}°C</p>
            <p><strong>Humidity:</strong> ${weather.humidity}%</p>
        `;
    } else {
        weatherInfo.innerHTML = '<p>Error fetching weather data.</p>';
    }
}

// Helper function to update the DOM with catastrophe data
function updateCatastropheInfo(data) {
    if (data && data.length > 0) {
        let html = '';
        data.forEach(catastrophe => {
            html += `<p><strong>Type:</strong> ${catastrophe.type}</p>`;
            html += `<p><strong>Location:</strong> ${catastrophe.location}</p>`;
            html += `<p><strong>Description:</strong> ${catastrophe.description}</p>`;
            html += '<hr>';
        });
        catastropheInfo.innerHTML = html;
    } else {
        catastropheInfo.innerHTML = '<p>No catastrophes reported.</p>';
    }
}

// Function to fetch data from the ESP32 server
function fetchData() {
    if (!isBlackout) { // Only fetch if not in blackout
        loadingIndicator.style.display = 'block'; // Show loading indicator

        // Fetch flight data
        Promise.all([
            fetch('/flights').then(response => response.json()),
            fetch('/weather').then(response => response.json()),
            fetch('/catastrophes').then(response => response.json())
        ])
        .then(([flightData, weatherData, catastropheData]) => {
            updateFlightTable(flightData);
            updateWeatherInfo(weatherData);
            updateCatastropheInfo(catastropheData);
            loadingIndicator.style.display = 'none'; // Hide loading indicator
        })
        .catch(error => {
            console.error('Error fetching data:', error);
            loadingIndicator.style.display = 'none'; // Hide on error too
            alert('Failed to fetch data. Please check your connection.');
        });
    }
    else{
        //fetch local data
        loadingIndicator.style.display = 'block';
        Promise.all([
            fetch('/flights').then(response => response.json()),
            fetch('/weather').then(response => response.json()),
            fetch('/catastrophes').then(response => response.json())
        ])
        .then(([flightData, weatherData, catastropheData]) => {
            updateFlightTable(flightData);
            updateWeatherInfo(weatherData);
            updateCatastropheInfo(catastropheData);
            loadingIndicator.style.display = 'none'; // Hide loading indicator
        })
        .catch(error => {
            console.error('Error fetching data:', error);
            loadingIndicator.style.display = 'none'; // Hide on error too
            alert('Failed to fetch local data.  Data may be outdated.');
        });
    }
}

// Function to simulate blackout.  In a real scenario, this would be
// triggered by an event (e.g., loss of power).  For this demo,
// we'll just use a button to toggle it.
function simulateBlackout() {
    isBlackout = !isBlackout; // Toggle the blackout status
    updateBlackoutStatusDisplay(); // Update the display
    fetchData(); // Re-fetch data to update the UI
}

// Function to update the blackout status display
function updateBlackoutStatusDisplay() {
    if (isBlackout) {
        blackoutStatusElement.textContent = "Operating in Blackout Mode. Data may be outdated.";
        blackoutStatusElement.style.color = "red";
    } else {
        blackoutStatusElement.textContent = "Operating Normally";
        blackoutStatusElement.style.color = "green";
    }
}

// Initial data load when the page loads
window.onload = () => {
    fetchData();
    updateBlackoutStatusDisplay();
    // Add a button to simulate a blackout (for testing purposes)
    const blackoutButton = document.createElement('button');
    blackoutButton.textContent = 'Simulate Blackout';
    blackoutButton.id = 'blackout-button'; // Add an ID for styling
    blackoutButton.onclick = simulateBlackout;
    document.body.appendChild(blackoutButton);
};

// Refresh data when the refresh button is clicked
refreshButton.addEventListener('click', () => {
    fetchData();
    updateBlackoutStatusDisplay();
});
