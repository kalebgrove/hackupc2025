# Airport Information System

## Overview

The **Airport Information System** provides real-time updates about flights, weather, and operational status at an airport. It is designed for both passengers and administrators. The system can operate in a **blackout mode** where data is served from a local network (ESP32-based) if the main server is down.

## Features

- **Flight Management** (Admin only): Add, edit, and delete flight information including flight number, gate, destination, boarding time, and status.
- **Weather Updates**: Real-time weather conditions for the airport.
- **Blackout Mode**: Automatically switches to a local network and displays basic data in the event of a system failure.

## Technologies

- **Frontend**: HTML, CSS, JavaScript
- **Backend**: Flask (Python)
- **Database**: SQLite
- **ESP32**: Used for blackout mode and local network communication.
