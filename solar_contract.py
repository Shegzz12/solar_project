# from pipes import Template
from flask import Flask, request, jsonify, render_template, send_file
from flask_cors import CORS
import csv
import util
from datetime import datetime
import threading
import os

util_path = "util.py"

app = Flask(__name__, static_url_path='/static')

# app = Flask(__name__)
CORS(app)

# Initialize values to be used if no data is received
result = 0
office = 0
lab = 0
Temperature = 36
Humidity = 50
Light_intensity = 35
Light_percent = ((Light_intensity /50)*100)
Battery_percentage = 30
energy_reading = 20
energy_reading2 = 20
timestamp = ""
weather = "Weather not so favorable for energy generation"
advice = "Reduce energy use on both lab & office"

csv_file_path = 'database.csv'
fieldnames = ['TIMESTAMP', 'LIGHT', 'TEMPERATURE', 'HUMIDITY', 'BATTERY_PERCENTAGE', 'RESULT', 'LAB_ENERGY', 'OFFICE_ENERGY']


@app.route('/download_csv')
def download_csv():
    #Sends the database to the front end for download
    return send_file(csv_file_path, as_attachment=True)


@app.route('/download_dataset')
def download_dataset():
    #Sends the database to the front end for download
    return send_file('sd.csv', as_attachment=True)

def read_records():
    records = []
    with open(csv_file_path, 'r', newline='') as file:
        reader = csv.DictReader(file)
        for row in reader:
            records.append({
                'TIMESTAMP': row['TIMESTAMP'],
                'LIGHT': row['LIGHT'],
                'TEMPERATURE': row['TEMPERATURE'],
                'HUMIDITY': row['HUMIDITY'],
                'BATTERY_PERCENTAGE': row['BATTERY_PERCENTAGE'],
                'RESULT': row['RESULT'],
                'LAB_ENERGY': row['LAB_ENERGY'],
                'OFFICE_ENERGY': row['OFFICE_ENERGY'],
            })
    return records


def write_records(records):
    with open(csv_file_path, 'w', newline='') as file:
        writer = csv.DictWriter(file, fieldnames=fieldnames)
        writer.writeheader()

        for record in records:
            # Ensure the timestamp is in the correct format (string)
            # timestamp = record['timestamp']
            # Write the record to the CSV file
            writer.writerow({
                'TIMESTAMP': record['TIMESTAMP'],
                'LIGHT': record['LIGHT'],
                'TEMPERATURE': record['TEMPERATURE'],
                'HUMIDITY': record['HUMIDITY'],
                'BATTERY_PERCENTAGE': record['BATTERY_PERCENTAGE'],
                'RESULT': record['RESULT'],
                'LAB_ENERGY': record['LAB_ENERGY'],
                'OFFICE_ENERGY': record['OFFICE_ENERGY'],
            })


@app.route('/delete_all_data', methods=['POST'])
def delete_all_data():
    try:
        # Delete all records from the CSV file
        write_records([])
        # Emit an update to connected clients after data deletion
        # socketio.emit('update_data', {'data': []}, namespace='/test')
        return jsonify({'message': 'All data deleted successfully'}), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500


@app.route('/receive_state', methods=['POST'])
def receive_state():
    global office, lab
    try:
        received_state = request.get_json()
        print("Received state:", received_state)
        
        if not received_state or not isinstance(received_state, dict):
            return jsonify({"error": "Invalid JSON data"}), 400


        if 'office0' in received_state:
            office = received_state['office0']
            print(f"Received office0: {office}")

        if 'lab0' in received_state:
            lab = received_state['lab0']
            print(f"Received lab0: {lab}")

    
        return jsonify({"message": "Data received successfully"}), 200

    except Exception as e:
        return f"Error: {str(e)}", 400


@app.route('/')
def index():
    return render_template('index.html')

# ... (your existing code)

@app.route('/get_real_time_data', methods=['GET'])
def get_real_time_data():
    # print("temp")
    # #predict_user_input()
    try:
        real_time_data = {
            'office': office,
            'lab': lab,
            'Temperature': Temperature,
            'Humidity': Humidity,
            'Light_intensity': Light_percent,
            'Battery_percentage': Battery_percentage,
            'energy_reading': energy_reading,
            'energy_reading2': energy_reading2,
            'result': result,
            'weather': weather,
            'advice': advice
        }
        return jsonify(real_time_data), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500



# ... (your existing code)


@app.route('/get_value', methods=['GET'])
def get_value():
    # Logic to retrieve the value
    value_to_display = "value"

    # Return the value as JSON
    return jsonify({'value': value_to_display})

@app.route('/send_data', methods=['POST'])
def receive_data():
    global timestamp, Temperature, Humidity, Light_intensity, Light_percent, Battery_percentage, energy_reading, energy_reading2, result
    try:
        received_data = request.get_json()
        print("Received data:", received_data)  # Add this line to print the received data
        # Read existing records
        records = read_records()

        if 'temp' in received_data and 'hum' in received_data and 'light' in received_data and 'percentage' in received_data and 'energy_reading' in received_data and 'energy_reading2' in received_data:
            Temperature = received_data['temp']
            Humidity = received_data['hum']
            Light_intensity = received_data['light']
            Battery_percentage = received_data['percentage']
            energy_reading = received_data['energy_reading']
            energy_reading2 = received_data['energy_reading2']
            Light_percent = ((Light_intensity /50)*100)
            print(f"Received temp: {Temperature}, hum: {Humidity}, light: {Light_intensity}, lightP: {Light_percent}, percentage: {Battery_percentage}, energy_reading: {energy_reading}, energy_reading2: {energy_reading2}")

            timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

            predict_user_input()
            # Append the received data to the CSV file
            new_record = {
                'TIMESTAMP': timestamp,
                'LIGHT': Light_percent,
                'TEMPERATURE': Temperature,
                'HUMIDITY': Humidity,
                'BATTERY_PERCENTAGE': Battery_percentage,
                'RESULT': result,
                'LAB_ENERGY': energy_reading,
                'OFFICE_ENERGY': energy_reading2,
            }
            records.append(new_record)

            # Write the updated records to the CSV file
            write_records(records)

            return "Data received and saved successfully", 200
        else:
            return "Invalid data format", 400
    except json.JSONDecodeError:
        return "Invalid JSON format", 400
    except Exception as e:
        return f"Error: {str(e)}", 400

@app.route('/gas_state', methods=['GET'])
def predict_user_input():

    global Temperature, Humidity, Light_intensity, result, weather, advice
    try:
        # Call the utility function to make predictions
        result = util.predict_user_input(Light_intensity, Temperature, Humidity)
        # Update weather and advice based on the result
        if result == 1:
            weather = "Poor weather condition for energy generation"
            advice  = "Please switch off all devices"
        elif result == 2:
            weather = "Weather not so favorable for energy generation"
            advice = "Reduce energy use on both lab & office"
        elif result == 3:
            weather = "Fair weather condition"
            advice = "Reduce energy use for either lab or office"
        elif result == 4:
            weather = "Good weather condition"
            advice = "Energy sufficient for lab & office"

        # Construct the response
        response = jsonify({
            'result': result,
            'office': office,
            'lab': lab,
            'weather': weather,
            'advice': advice
        })
        return response
    except Exception as e:
        return jsonify({'error': str(e)})

# Assuming you have a list of locations

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=False)
