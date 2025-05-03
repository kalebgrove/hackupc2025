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
            fetch('http://localhost:8080/flights').then(response => response.json()),
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
    const styleSheet = document.getElementById('theme-stylesheet');
    if (isBlackout) {
        styleSheet.href = "blackout.css";
        blackoutStatusElement.textContent = "Operating in Blackout Mode. Data may be outdated.";
        blackoutStatusElement.style.color = "red";
    } else {
        styleSheet.href = "style.css";
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
