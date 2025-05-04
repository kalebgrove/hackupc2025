// Get references to the form and the table
const flightForm = document.getElementById('flightForm');
const flightTableBody = document.getElementById('flightTableAdmin').querySelector('tbody');

// 1. Function to add a new flight row to the table
function addFlightRow(flightNumber, gate, destination, boardingTime, departureTime, status) {
    // Create a new table row element
    const row = document.createElement('tr');

    // Set the HTML content of the row using template literals for better readability
    row.innerHTML = `
        <td>${flightNumber}</td>
        <td>${gate}</td>
        <td>${destination}</td>
        <td>${boardingTime}</td>
        <td>${departureTime}</td>
        <td>${status}</td>
        <td class="action-buttons">
            <button onclick="editRow(this)">Edit</button>
            <button onclick="deleteRow(this)">Delete</button>
        </td>
    `;

    // Append the new row to the table's body
    flightTableBody.appendChild(row);
}

// 2. Function to handle the "Edit" button click
function editRow(buttonElement) {
    // Get the parent row of the clicked button
    const row = buttonElement.parentElement.parentElement;
    // Get all the table cells (<td> elements) in the row
    const cells = row.querySelectorAll('td');

    // Loop through the first 6 cells (flight data)
    for (let i = 0; i < 6; i++) {
        // Get the current text content of the cell
        const currentValue = cells[i].textContent;
        // Replace the cell's content with an input field containing the current value
        cells[i].innerHTML = `<input type="text" value="${currentValue}" style="width: 80%; padding: 5px; font-size: 12px;" />`;
    }

    // Change the button text to "Save"
    buttonElement.textContent = 'Save';
    // Change the button's onclick function to call the saveRow function
    buttonElement.onclick = function() {
        saveRow(buttonElement);
    };
}

// 3. Function to handle the "Save" button click
function saveRow(buttonElement) {
    const row = buttonElement.parentElement.parentElement;
    const cells = row.querySelectorAll('td');
    const flightNumber = cells[0].querySelector('input').value;
    const gate = cells[1].querySelector('input').value;
    const destination = cells[2].querySelector('input').value;
    const boardingTime = cells[3].querySelector('input').value;
    const departureTime = cells[4].querySelector('input').value;
    const status = cells[5].querySelector('input').value;

    const updatedFlightData = {
        flightnumber: flightNumber,
        gate: gate,
        destination: destination,
        boarding_time: boardingTime,
        departure_time: departureTime,
        status: status
    };

    fetch(`http://10.192.136.63:5000/flights/${flightNumber}`, {
        method: 'PUT',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(updatedFlightData)
    })
    .then(response => {
        if (!response.ok) {
            return response.json().then(err => {  // Get error message from JSON
                throw new Error(err.error || 'Failed to update flight data'); // Use err.error if available
            });
        }
        return response.json();
    })
    .then(data => {
        console.log('Flight data updated:', data);
        // Update the table cells with the new values
        cells[0].textContent = flightNumber;
        cells[1].textContent = gate;
        cells[2].textContent = destination;
        cells[3].textContent = boardingTime;
        cells[4].textContent = departureTime;
        cells[5].textContent = status;

        buttonElement.textContent = 'Edit';
        buttonElement.onclick = function() {
            editRow(buttonElement);
        };
         // Optionally show a success message to the user here
    })
    .catch(error => {
        console.error('Error updating flight data:', error);
        alert('Failed to update flight data: ' + error.message); // Show error message to user
    });
}


// 4. Function to fetch flight data from the server
async function fetchDataAdmin() {
    try {
        // Fetch data from the specified URL
        const response = await fetch('http://10.192.136.63:5000/flights');
        // If the response is not successful, throw an error
        if (!response.ok) {
            throw new Error("Network response was not ok");
        }

        // Parse the JSON data from the response
        const flights = await response.json();
        console.log("Flights data from backend:", flights); // Debug: Print the fetched data

        // Clear the existing table rows
        flightTableBody.innerHTML = '';

        // Loop through the flights array and add each flight to the table
        flights.forEach(flight => {
            const row = document.createElement('tr');
            row.innerHTML = `
                <td>${flight[0]}</td>
                <td>${flight[1]}</td>
                <td>${flight[2]}</td>
                <td>${flight[3]}</td>
                <td>${flight[4]}</td>
                <td>${flight[5]}</td>
                <td class="action-buttons">
                    <button onclick="editRow(this)">Edit</button>
                    <button onclick="deleteRow(this)">Delete</button>
                </td>
            `;
            flightTableBody.appendChild(row);
        });
    } catch (error) {
        // Handle any errors that occur during the fetch operation
        console.error("Failed to fetch flight data:", error);
    }
}

// 5. Event listener to call fetchDataAdmin when the page loads
window.onload = () => {
    fetchDataAdmin();
};

// 6. Event listener for the form submission to add a new flight
flightForm.addEventListener('submit', (event) => {
    event.preventDefault(); // Prevent the default form submission behavior

    // Get the values from the input fields
    const flightNumber = document.getElementById('flightNumber').value.trim();
    const gate = document.getElementById('gate').value.trim();
    const destination = document.getElementById('destination').value.trim();
    const boardingTime = document.getElementById('boardingTime').value.trim();
    const departureTime = document.getElementById('departureTime').value.trim();
    const status = document.getElementById('status').value.trim();

    // Check if all fields are filled
    if (flightNumber && gate && destination && boardingTime && departureTime && status) {
        // Add the flight row to the table
        addFlightRow(flightNumber, gate, destination, boardingTime, departureTime, status);
        // Reset the form to clear the input fields
        flightForm.reset();
    }
});

// 7. Function to delete a flight row
function deleteRow(buttonElement) {
    const row = buttonElement.parentElement.parentElement;
    row.remove();
}
