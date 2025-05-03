import sqlite3

# Connect to SQLite database (it will be created if it doesn't exist)
conn = sqlite3.connect('flights.db')

# Create a cursor object to interact with the database
cursor = conn.cursor()

# Create flights table
cursor.execute('''CREATE TABLE IF NOT EXISTS flights (
    flightnumber varchar(32) PRIMARY KEY,
    gate varchar(32),
    destination varchar(32),
    boarding_time varchar(32),
    departure_time varchar(32),
    status varchar(64)
)''')

# Insert sample data into the table
cursor.execute("INSERT INTO flights (flightnumber, gate, boarding_time, departure_time, status) VALUES (?, ?, ?, ?, ?)",
               ('AA1234', 'A1', '2025-05-01 10:00', '2025-05-01 11:00', 'On time'))
cursor.execute("INSERT INTO flights (flightnumber, gate, boarding_time, departure_time, status) VALUES (?, ?, ?, ?, ?)",
               ('BA2345', 'B2', '2025-05-01 12:30', '2025-05-01 14:00', 'Delayed'))

# Commit the changes and close the connection
conn.commit()
conn.close()

print("Database and table created, sample data inserted.")
