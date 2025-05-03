import sqlite3

# Connect to SQLite database (it will be created if it doesn't exist)
conn = sqlite3.connect('weather.db')

# Create a cursor object to interact with the database
cursor = conn.cursor()

# Create flights table
cursor.execute('''CREATE TABLE IF NOT EXISTS weather (
    temperature int PRIMARY KEY,
    weather varchar(32),
    humidity int
)''')

# Commit the changes and close the connection
conn.commit()
conn.close()

print("Database and table created, sample data inserted.")
