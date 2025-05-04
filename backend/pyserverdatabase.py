from flask import Flask, jsonify, request
from flask_cors import CORS
from sqlalchemy import create_engine, Column, Integer, String, REAL
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker, scoped_session
from generator import generate_flight_data
from sqlalchemy import text
from addweather import fetch_weather_data

app = Flask(__name__)
CORS(app)

# Setup SQLAlchemy with the existing flights.db
Base = declarative_base()
engine = create_engine('sqlite:///info.db', connect_args={"check_same_thread": False})
SessionFactory = sessionmaker(bind=engine)

session = scoped_session(SessionFactory)

# Assuming the database has a table for storing flight information, adjust as necessary
class FlightData(Base):
    __tablename__ = 'flights'  # Change to match the table name in your database
    flightnumber = Column(String, primary_key=True)
    gate = Column(String)
    destination = Column(String)
    boarding_time = Column(String)
    departure_time = Column(String)
    status = Column(String)

class WeatherData(Base):
    __tablename__ = 'weather'
    id = Column(Integer, autoincrement=True, primary_key=True)
    temperature = Column(REAL)
    humidity = Column(Integer)
    condition = Column(String)

# Define routes

@app.route('/weather', methods=['GET'])
def get_dataweather():
    # Fetch all weather data from the database
    data = session.query(WeatherData).all()
    
    # Constructing a matrix (list of lists) from the fetched data
    weather_matrix = [
        [item.temperature, item.humidity, item.condition]
        for item in data
    ]
    
    return jsonify(weather_matrix)

@app.route('/flights/<flightnumber>', methods=['PUT'])
def update_flight(flightnumber):
    """
    Updates an existing flight in the database.
    """
    db_session = session()
    try:
        # Get the updated flight data from the request's JSON body
        new_data = request.get_json()

        # Find the flight in the database by flightnumber (primary key)
        flight_to_update = db_session.query(FlightData).filter_by(flightnumber=flightnumber).first()

        if flight_to_update:
            # Update the flight's attributes with the new data
            flight_to_update.gate = new_data.get('gate', flight_to_update.gate)
            flight_to_update.destination = new_data.get('destination', flight_to_update.destination)
            flight_to_update.boarding_time = new_data.get('boarding_time', flight_to_update.boarding_time)
            flight_to_update.departure_time = new_data.get('departure_time', flight_to_update.departure_time)
            flight_to_update.status = new_data.get('status', flight_to_update.status)

            db_session.commit()
            return jsonify({"message": "Flight data updated successfully!"})
        else:
            return jsonify({"error": "Flight not found"}), 404  # Return 404 if not found

    except Exception as e:
        db_session.rollback()
        return jsonify({"error": str(e)}), 500
    finally:
        db_session.close()

@app.route('/flights', methods=['GET'])
def get_dataflights():
    # Fetch all flight data from the database
    data = session.query(FlightData).all()
    
    # Constructing a matrix (list of lists) from the fetched data
    flights_matrix = [
        [item.flightnumber, item.gate, item.destination, item.boarding_time, item.departure_time, item.status]
        for item in data
    ]
    
    return jsonify(flights_matrix)

@app.route('/del-weather', methods=['POST'])
def del_dataweather() :
    session.execute(text('DELETE FROM weather'))
    session.commit()
    print("All entries from the weather table have been deleted.")
    return jsonify({"message": "All entries from the weather table have been deleted."})

@app.route('/del-flights', methods=['POST'])
def del_dataflights():
    session.execute(text('DELETE FROM flights'))
    session.commit()
    print("All entries from the flights table have been deleted.")
    return jsonify({"message": "All entries from the flights table have been deleted."})

@app.route('/add-flight-admin', methods=['POST'])
def add_flight_admin():
    db_session = session()

    try:
        new_data = request.get_json()

        new_flight = FlightData(
            flightnumber=new_data.get('flightnumber'),  # Use .get() to handle missing fields
            gate=new_data.get('gate'),
            destination=new_data.get('destination'),
            boarding_time=new_data.get('boarding_time'),
            departure_time=new_data.get('departure_time'),
            status=new_data.get('status')
        )

        db_session.add(new_flight)

        db_session.commit()

    except Exception as e:
        db_session.rollback()
        return jsonify({"error": str(e)}), 500
    finally:
        db_session.close() 

@app.route('/add-flights', methods=['POST'])
def add_dataflights():
            
    new_data = generate_flight_data()
    new_entry = FlightData(
        flightnumber=new_data['flightnumber'],
        gate=new_data['gate'],
        destination=new_data['destination'],
        boarding_time=new_data['boarding_time'],
        departure_time=new_data['departure_time'],
        status=new_data['status']
    )
    session.add(new_entry)
    session.commit()
    return jsonify({"message": "Flight data added successfully!"})

@app.route('/add-weather', methods=['POST'])
def add_dataweather():
    new_data = fetch_weather_data()  # Assuming this fetches and adds weather data
    new_entry = WeatherData(
        temperature=new_data['temperature'],
        humidity=new_data['humidity'],
        condition=new_data['condition']
    )
    session.add(new_entry)
    session.commit()

    return jsonify({"message": "Weather data added successfully!"})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
