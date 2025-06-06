import random
import datetime
import json

def generate_flight_number():
    """Generates a random flight number in the format AA1234."""
    airline_codes = ['AA', 'DL', 'UA', 'WN', 'B6', 'AS', 'F9', 'NK', 'HA']  # Common airline codes
    airline = random.choice(airline_codes)
    flight_number = random.randint(1000, 9999)
    return f"{airline}{flight_number}"

def generate_gate():
    """Generates a random gate number (e.g., A1, B22, C5)."""
    terminal_letters = ['A', 'B', 'C', 'D', 'E']
    terminal = random.choice(terminal_letters)
    gate_number = random.randint(1, 30)  # Assuming gates are numbered 1-30
    return f"{terminal}{gate_number}"

def generate_boarding_time(departure_time):
    """Generates a boarding time that is between 30 and 60 minutes before departure."""
    min_before = 30
    max_before = 60
    minutes_before = random.randint(min_before, max_before)
    boarding_time = departure_time - datetime.timedelta(minutes=minutes_before)
    return boarding_time

def generate_departure_time():
    """Generates a random departure time for today between 10:00 AM and 10:00 PM."""
    start_hour = 10
    end_hour = 22
    hour = random.randint(start_hour, end_hour)
    minute = random.randint(0, 59)
    # Get today's date
    today = datetime.date.today()
    # Combine today's date with the random time.
    departure_time = datetime.datetime(today.year, today.month, today.day, hour, minute)
    return departure_time

def generate_status():
    """Generates a random flight status."""
    statuses = ['On Time', 'Delayed', 'Canceled', 'Boarding', 'Departed', 'Arrived']
    return random.choice(statuses)

def generate_destination() :
    countries = [
        "Afghanistan", "Albania", "Algeria", "Andorra", "Angola",
        "Antigua and Barbuda", "Argentina", "Armenia", "Australia", "Austria",
        "Azerbaijan", "Bahamas", "Bahrain", "Bangladesh", "Barbados",
        "Belarus", "Belgium", "Belize", "Benin", "Bhutan", "Bolivia",
        "Bosnia and Herzegovina", "Botswana", "Brazil", "Brunei", "Bulgaria",
        "Burkina Faso", "Burundi", "Cabo Verde", "Cambodia", "Cameroon",
        "Canada", "Central African Republic", "Chad", "Chile", "China",
        "Colombia", "Comoros", "Congo (Congo-Brazzaville)",
        "Costa Rica", "Croatia", "Cuba", "Cyprus", "Czechia (Czech Republic)",
        "Denmark", "Djibouti", "Dominica", "Dominican Republic", "Ecuador",
        "Egypt", "El Salvador", "Equatorial Guinea", "Eritrea", "Estonia",
        "Eswatini (fmr. 'Swaziland')", "Ethiopia", "Fiji", "Finland", "France",
        "Gabon", "Gambia", "Georgia", "Germany", "Ghana", "Greece", "Grenada",
        "Guatemala", "Guinea", "Guinea-Bissau", "Guyana", "Haiti", "Holy See",
        "Honduras", "Hungary", "Iceland", "India", "Indonesia", "Iran", "Iraq",
        "Ireland", "Israel", "Italy", "Ivory Coast", "Jamaica", "Japan", "Jordan",
        "Kazakhstan", "Kenya", "Kiribati", "Kuwait", "Kyrgyzstan", "Laos", "Latvia",
        "Lebanon", "Lesotho", "Liberia", "Libya", "Liechtenstein", "Lithuania",
        "Luxembourg", "Madagascar", "Malawi", "Malaysia", "Maldives", "Mali", "Malta",
        "Marshall Islands", "Mauritania", "Mauritius", "Mexico", "Micronesia", "Moldova",
        "Monaco", "Mongolia", "Montenegro", "Morocco", "Mozambique", "Myanmar (formerly Burma)",
        "Namibia", "Nauru", "Nepal", "Netherlands", "New Zealand", "Nicaragua", "Niger",
        "Nigeria", "North Korea", "North Macedonia", "Norway", "Oman", "Pakistan", "Palau", 
        "Panama", "Papua New Guinea", "Paraguay", "Peru", "Philippines",
        "Poland", "Portugal", "Qatar", "Romania", "Russia", "Rwanda", "Saint Kitts and Nevis",
        "Saint Lucia", "Saint Vincent and the Grenadines", "Samoa", "San Marino",
        "Sao Tome and Principe", "Saudi Arabia", "Senegal", "Serbia", "Seychelles", "Sierra Leone",
        "Singapore", "Slovakia", "Slovenia", "Solomon Islands", "Somalia", "South Africa",
        "South Korea", "South Sudan", "Spain", "Sri Lanka", "Sudan", "Suriname", "Sweden",
        "Switzerland", "Syria", "Taiwan", "Tajikistan", "Tanzania", "Thailand", "Timor-Leste",
        "Togo", "Tonga", "Trinidad and Tobago", "Tunisia", "Turkey", "Turkmenistan", "Tuvalu",
        "Uganda", "Ukraine", "United Arab Emirates", "United Kingdom", "United States of America",
        "Uruguay", "Uzbekistan", "Vanuatu", "Venezuela", "Vietnam", "Yemen", "Zambia", "Zimbabwe"
    ]
    return random.choice(countries)

def generate_flight_data():
    """Generates all flight data and returns it as a dictionary."""
    departure_time = generate_departure_time()
    boarding_time = generate_boarding_time(departure_time) # Pass departure_time
    flight_data = {
        'flightnumber': generate_flight_number(),
        'gate': generate_gate(),
        'destination': generate_destination(),
        'boarding_time': boarding_time.strftime("%H:%M"),  # Format the datetime objects
        'departure_time': departure_time.strftime("%H:%M"), # Format the datetime objects
        'status': generate_status(),
    }
    return flight_data

if __name__ == "__main__":
    flight = generate_flight_data()
    flight_json = json.dumps(flight, indent=4)
    print(flight_json)