const flightTable = document.getElementById('flight-table');
const weatherInfo = document.getElementById('weather-info');
const loadingIndicator = document.getElementById('loading-indicator');
const refreshButton = document.getElementById('refresh-button');
const blackoutStatusElement = document.getElementById('blackout-status');

let isBlackout = false;

// Update weather data
async function fetchWeather() {

}

// Update flight data
async function fetchData() {
    try {
      const response = await fetch('http://<ESP32_IP_ADDRESS>/flights');
      if (!response.ok) throw new Error("Network response was not ok");
  
      const flights = await response.json();
      const tbody = flightTable.querySelector('tbody');
      tbody.innerHTML = ''; // Clear existing rows
  
      flights.forEach(flight => {
        const row = document.createElement('tr');
        row.innerHTML = `
          <td>${flight.flightNumber}</td>
          <td>${flight.gate}</td>
          <td>${flight.destination}</td>
          <td>${flight.boarding}</td>
          <td>${flight.departure}</td>
          <td>${flight.status}</td>
        `;
        tbody.appendChild(row);
      });
    } catch (error) {
      console.error("Failed to fetch flight data:", error);
    }
}

// Simulate blackout. In a real scenario, this would be
// triggered by loss of power, or similar event.
// For this demo, we just use a button to toggle it.
function simulateBlackout() {
    isBlackout = !isBlackout; // Toggle the blackout status
    updateBlackoutStatusDisplay(); // Update the display
    fetchData(); // Re-fetch data to update the UI
}

// Function to update the blackout status display
function updateBlackoutStatusDisplay() {
    const styleSheet = document.getElementById('theme-stylesheet'); // Change the style sheet to dark mode version
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
