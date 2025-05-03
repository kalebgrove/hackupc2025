from flask import Flask, request, jsonify
from sqlalchemy import create_engine, Column, Integer, String
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
from generator import generate_flight_data
import time

app = Flask(__name__)

# Setup SQLAlchemy with the existing flights.db
Base = declarative_base()
engine = create_engine('sqlite:///flights.db')
Session = sessionmaker(bind=engine)
session = Session()

# Assuming the database has a table for storing flight information, adjust as necessary
class FlightData(Base):
    __tablename__ = 'flights'  # Change to match the table name in your database
    flightnumber = Column(String, primary_key=True)
    gate = Column(String)
    destination = Column(String)
    boarding_time = Column(String)
    departure_time = Column(String)
    status = Column(String)

# Define routes

@app.route('/data', methods=['GET'])
def get_data():
    # Fetch all flight data from the database
    data = session.query(FlightData).all()
    
    # Constructing a matrix (list of lists) from the fetched data
    flights_matrix = [
        [item.flightnumber, item.gate, item.destination, item.boarding_time, item.departure_time, item.status]
        for item in data
    ]
    
    return jsonify(flights_matrix)

@app.route('/data', methods=['POST'])
def add_data():
            
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

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
