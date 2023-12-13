document.addEventListener("DOMContentLoaded", function () {
    // Function to fetch real-time data from the server
    function pollForRealTimeData() {
        // Fetch real-time data from the server
        fetch('https://intelligentsolar.onrender.com/get_real_time_data')
            .then(response => response.json())
            .then(updateRealTimeData)
            .catch(error => console.error('Error:', error));
    }

    // Function to update real-time data on the frontend
    // Function to update real-time data on the frontend
    function updateRealTimeData(realTimeData) {
        const weatherBox = document.getElementById('weather-box');
        const adviceBox = document.getElementById('advice-box');

        weatherBox.innerText = realTimeData.weather;
        adviceBox.innerText = realTimeData.advice;
    }


    // Initial call to fetch real-time data
    pollForRealTimeData();

    // Set up polling to fetch real-time data every 5 seconds (adjust as needed)
    setInterval(pollForRealTimeData, 7000);

    // Toggle Buttons
    const toggleButtons = document.querySelectorAll(".toggle");
    const statusMessage = document.getElementById("status-message");

    toggleButtons.forEach((button, index) => {
        button.addEventListener("click", () => {
            toggleStatus(button, index + 1);
        });
    });

    // Function to toggle the status and send the state to the server
    function toggleStatus(button, applianceNumber) {
        const status = button.parentElement.querySelector(".status");
        if (status.textContent === "Off") {
            status.textContent = "On";
            status.classList.add("on");
            // if (statusMessage.style.display === "block") {
            //     statusMessage.style.display = "none";
            // }
        } else {
            status.textContent = "Off";
            status.classList.remove("on");
            // statusMessage.style.display = "block";
        }

        // Determine the appliance ID based on the button clicked
        let applianceId;
        if (applianceNumber === 1) {
            applianceId = "office0";
        } else if (applianceNumber === 2) {
            applianceId = 'lab0';
        }

        // Make a POST request to the server's endpoint with the appliance ID
        fetch(`https://intelligentsolar.onrender.com/receive_state`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'dataType': "json",
            },
            body: JSON.stringify({ [applianceId]: status.textContent === "On" ? 1 : 0 }),
        })
            .then(response => response.json())
            .then(data => {
                console.log('Data sent successfully:', data);
            })
            .catch(error => {
                console.error('Error sending data to server:', error);
            });
    }

    // Function to update the gauge values and fetch data
    function rotateDial(value, dialId) {
        let deg = (value * 177.5) / 100;

        // Update the gauge value using the dialId
        document.getElementById(dialId + '-value').innerHTML = value + "%";

        // Apply the rotation to the dial using the dialId
        document.getElementById(dialId + '-arrow').style.transform = 'rotate(' + deg + 'deg)';
    }

    // Function to fetch data and update the gauges
    function fetchData() {
        fetch('https://intelligentsolar.onrender.com/get_real_time_data')
            .then(response => {
                if (!response.ok) {
                    throw new Error('Network response was not ok');
                }
                return response.json();
            })
            .then(data => {
                rotateDial(data.Temperature, 'temperature');
                rotateDial(data.Humidity, 'humidity');
                rotateDial(data.Light_intensity, 'light-intensity');
                rotateDial(data.Battery_percentage, 'battery-percent');
                rotateDial(data.energy_reading, 'energy-reading');

                document.getElementById('temperature-reading').innerText = 'Temperature: ' + data.Temperature + '%';
                document.getElementById('humidity-reading').innerText = 'Humidity: ' + data.Humidity + '%';
                document.getElementById('light-intensity').innerText = 'Light intensity: ' + data.Light_intensity + '%';
                document.getElementById('battery-percent').innerText = 'Battery percentage: ' + data.Battery_percentage + '%';
                document.getElementById('energy-reading').innerText = 'Energy reading: ' + data.energy_reading + '%';
        
            })
            .catch(error => console.error('Error fetching data:', error));
    }

    // Initial call to fetch and display data
    fetchData();

    // Call fetchData every 2000 milliseconds (2 seconds)
    setInterval(fetchData, 5000);
});
