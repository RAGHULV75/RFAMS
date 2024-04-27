# RFAMS
RF ID Based Attendance Management Using Firebase Realtime Database and Android Application

This code appears to be written for an Arduino board and utilizes the Firebase Realtime Database (RTDB) to track student attendance. Here's a breakdown of the code:

**Libraries:**

- `Arduino.h`: Standard Arduino library.
- WiFi libraries (`WiFi.h` or `ESP8266WiFi.h`): For connecting to a Wi-Fi network (depending on the board).
- `Firebase_ESP_Client.h`: Library for interacting with Firebase.
- `WiFiUdp.h`, `NTPClient.h`, `SPI.h`: Libraries for time synchronization and RFID communication.
- `MFRC522.h`: Library for interacting with the MFRC522 RFID reader chip.
- Additional libraries (`addons/TokenHelper.h`, `addons/RTDBHelper.h`): Likely custom libraries for Firebase authentication and database interaction (not provided).

**Constants:**

- Wi-Fi credentials (SSID and password).
- Firebase API key and database URL.
- Pins for LED, LED_R (possibly for status indication), RFID reader (SS_PIN, RST_PIN).

**Global Variables:**

- `statuss`: Integer variable (purpose unclear).
- `out`: Integer variable (purpose unclear).
- `ntpUDP`: Instance of `WiFiUdp` for time synchronization.
- `timeClient`: Instance of `NTPClient` for getting time from NTP server.
- Arrays for weekdays and months.
- `fbdo`: Instance of `FirebaseData` for interacting with Firebase.
- `auth`: Instance of `FirebaseAuth` for user authentication (not used in this code).
- `config`: Instance of ` firebaseConfig` for Firebase configuration.
- Variables for tracking previous data sending time (`sendDataPrevMillis`), count, signup successful flag (`signupOK`), string variables (`a`, `uid`).

**Setup Function:**

- Initializes serial communication at 9600 baud.
- Sets pin modes for LED and LED_R (possibly for status indication).
- Connects to the specified Wi-Fi network and prints the IP address upon successful connection.
- Starts the NTP time client with an offset of 19800 seconds.
- Initializes SPI communication and the MFRC522 reader.
- Sets up Firebase configuration with API key and database URL.
- Attempts to sign up with Firebase anonymously (unused in the provided code). Prints a message on success or failure.
- Sets a callback function for token status and begins Firebase connection.

**Loop Function:**

- **RFID Loop:**
    - Checks for a new card and reads its serial number using the MFRC522 reader.
    - If a card is read, it constructs a string representation of the UID.
    - Calls the `isAuthorized` function to check if the UID is authorized.
        - If authorized:
            Prints "Authorized card" message with the UID.
            Turns on LED (possibly indicating access granted).
            Calls `sendToFirebase` to send attendance data.
            Turns off LED.
        - If unauthorized:
            Prints "Un Authorized Card" message with the UID.
            Turns on LED_R (possibly indicating unauthorized access).
            Waits for 2 seconds before turning it off.
- Clears the UID variable.

**isAuthorized Function:**

- Takes a string `uid` as input.
- Compares the `uid` with a predefined list of authorized IDs (`authorizedIds`).
- Returns `true` if the `uid` is found in the list, `false` otherwise.

**sendToFirebase Function:**

- Checks if Firebase is ready, signup was successful, and enough time has passed since the last data send (at least 15 seconds).
- If conditions are met, updates the `sendDataPrevMillis` variable.
- Gets the current epoch time and formatted time using the NTP client.
- Extracts the month, day, and year from the epoch time.
- Constructs a formatted date string (DD:MM:YYYY).
- Determines the current class period based on the hour (using a simplified logic).
- Replaces the actual student/staff IDs with dummy IDs for privacy reasons (assuming `uid` should hold the actual ID).
- Attempts to set data to Firebase RTDB at two paths:
    - `Staff/` path with student/staff information (currentDate, class period, UID, "Present" value).
    - `Students/` path with student information (UID, currentDate, class period, UID, "Present" value).
- Prints success/failure messages and the error reason if it fails.

**Overall Functionality:**

- The code connects to a Wi-Fi network and Firebase RTDB.
- It continuously reads for RFID card swipes.
- If an authorized card is swiped, it logs the attendance data
