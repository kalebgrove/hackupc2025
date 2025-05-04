const flightTable = document.getElementById('flight-table');
const weatherInfo = document.getElementById('weather-info');
const loadingIndicator = document.getElementById('loading-indicator');
const refreshButton = document.getElementById('refresh-button');
const blackoutStatusElement = document.getElementById('blackout-status');

let isBlackout = false;

// Update weather data
async function fetchWeather() {
  try {
    let found = false;
    const response = await fetch('http://<esp>/weather');
    if (!response.ok) throw new Error("Network response was not ok");

    const flights = await response.json();
    const tbody = weatherInfo.querySelector('tbody');
    tbody.innerHTML = '';

    flights.forEach(fl => {
      if(fl[0] != null) found = true;
      const row = document.createElement('tr');
      row.innerHTML = `
        <td>${fl[0]}</td>
        <td>${fl[1]}</td>
        <td>${fl[2]}</td>
      `;
      tbody.appendChild(row);
    });

    if(!found) {
      const row = document.createElement('tr');
      row.innerHTML = `<td colspan="3">No weather data found.</td>`;
      tbody.appendChild(row);
    }
  } catch (error) {
    console.error("Failed to fetch flight data:", error);
  }
}

// Update flight data
async function fetchData() {
    try {
      let found = false;
      const response = await fetch('http://<esp>/flights');
      if (!response.ok) throw new Error("Network response was not ok");
  
      const flights = await response.json();
      const tbody = flightTable.querySelector('tbody');
      tbody.innerHTML = '';

      // Get the query string from the current URL
      const queryString = window.location.search;

      // Parse the query string
      const urlParams = new URLSearchParams(queryString);

      // Get the value of 'flightNumber'
      const flightNumber = urlParams.get('flightNumber');
  
      flights.forEach(fl => {
        if (!(flightNumber === null) && fl[0] != flightNumber) return; // Skip if flight number doesn't match
        const row = document.createElement('tr');
        found = true;
        row.innerHTML = `
          <td>${fl[0]}</td>
          <td>${fl[1]}</td>
          <td>${fl[2]}</td>
          <td>${fl[3]}</td>
          <td>${fl[4]}</td>
          <td>${fl[5]}</td>
        `;
        tbody.appendChild(row);
      });

      if(!found) {
        const row = document.createElement('tr');
        row.innerHTML = `<td colspan="6">No flight data</td>`;
        tbody.appendChild(row);
      }
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
    fetchWeather();
    //fetchDataAdmin();
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
    fetchWeather();
    updateBlackoutStatusDisplay();
});
