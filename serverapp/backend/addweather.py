import requests
import sqlite3
from datetime import datetime
from config import API

# Your WeatherAPI API key (replace with your actual API key)
API_KEY = API
CITY = "Barcelona"  # City is now Barcelona
BASE_URL = "http://api.weatherapi.com/v1/current.json"

# Function to fetch weather data from WeatherAPI
def fetch_weather_data():
    params = {
        'key': API_KEY,  # Your API key
        'q': CITY,  # City name (now it will fetch for Barcelona)
        'aqi': 'no'  # Optional: Set to 'no' to avoid getting air quality data
    }

    response = requests.get(BASE_URL, params=params)

    if response.status_code == 200:
        data = response.json()

        # Extracting relevant data from the API response
        temperature = data['current']['temp_c']  # Temperature in Celsius
        humidity = data['current']['humidity']  # Humidity percentage
        condition = data['current']['condition']['text']  # Weather conditions (e.g., "Clear", "Rainy")

        print(temperature)
        return {'temperature': temperature, 'humidity': humidity, 'condition': condition}

    else:
        print(f"Failed to get weather data: {response.status_code}")

# Main function to run the script
def main():
    fetch_weather_data()  # Fetch and store the weather data

if __name__ == "__main__":
    main()
