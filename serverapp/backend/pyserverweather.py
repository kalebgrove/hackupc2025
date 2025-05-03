from flask import Flask, request, jsonify
from sqlalchemy import create_engine, Column, Integer, String, text
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker, scoped_session
from addweather import fetch_weather_data

app = Flask(__name__)

# Setup SQLAlchemy with the existing weather.db
Base = declarative_base()
engine = create_engine('sqlite:///weather.db', connect_args={"check_same_thread": False})  # Allow cross-thread access
SessionFactory = sessionmaker(bind=engine)
session = scoped_session(SessionFactory)  # Use scoped_session to handle thread-specific sessions

# Define the WeatherData model
class WeatherData(Base):
    __tablename__ = 'weather'
    id = Column(Integer, primary_key=True)
    temperature = Column(Integer)
    humidity = Column(Integer)
    conditions = Column(String)

@app.route('/data', methods=['GET'])
def get_data():
    # Fetch all weather data from the database
    data = session.query(WeatherData).all()
    
    # Constructing a matrix (list of lists) from the fetched data
    weather_matrix = [
        [item.temperature, item.humidity, item.conditions]
        for item in data
    ]
    
    return jsonify(weather_matrix)

@app.route('/add-weather', methods=['POST'])
def add_dataweather():
    fetch_weather_data()  # Assuming this fetches and adds weather data
    return jsonify({"message": "Weather data added successfully!"})

@app.route('/del', methods=['POST'])
def del_data():
    # Use 'text()' to ensure the query is interpreted correctly by SQLAlchemy
    session.execute(text('DELETE FROM weather'))
    session.commit()
    print("All entries from the weather table have been deleted.")
    return jsonify({"message": "All entries from the weather table have been deleted."})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5100, debug=True)
