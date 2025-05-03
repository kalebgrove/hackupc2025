import sqlite3

# Connect to SQLite database (it will be created if it doesn't exist)
conn = sqlite3.connect('info.db')

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

cursor.execute('''CREATE TABLE IF NOT EXISTS weather (
               id INTEGER AUTO_INCREMENT PRIMARY KEY,
               temperature REAL,
               humidity REAL,
               condition varchar(32)
               )''')

# Commit the changes and close the connection
conn.commit()
conn.close()

print("Database and table created, sample data inserted.")
