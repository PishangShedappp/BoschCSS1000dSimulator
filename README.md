# Bosch CCS 1000 D Simulator

A Qt6-based desktop application that simulates the Bosch Conference Control System (CCS 1000 D), providing a virtual environment for testing and developing applications that interact with Bosch conference hardware.

## 📋 Description

The Bosch CCS 1000 D Simulator is a comprehensive simulation tool that mimics the behavior of a real Bosch Conference Control System. It provides a visual interface showing microphone units, their states, and a central control unit, while simultaneously running an HTTP server that responds to API calls just like the actual hardware would.

This simulator is perfect for:
- **Developers** building applications for Bosch conference systems without physical hardware
- **Testing** conference management software in a controlled environment
- **Training** on how conference control systems work
- **Prototyping** new features before deploying to production hardware

## ✨ Features

### Visual Simulation
- **10 Microphone Units**: Visual representation of conference microphones with active/inactive states
- **Chairman Control**: Seat 1 is designated as the chairman with priority control
- **Real-time State Updates**: Visual feedback showing which microphones are active (red) or inactive (gray)
- **Priority Override**: Chairman can take control and mute all other microphones with a single button press
- **System Status Display**: Shows server status and connection information

### HTTP REST API Server
The simulator runs a local HTTP server (default port: 8090) that implements the Bosch CCS 1000 D API:

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/login` | POST | Authenticate and receive a session ID |
| `/api/seats` | GET | Retrieve list of all conference seats/microphones |
| `/api/speakers` | GET | Get currently active speakers (microphones that are ON) |
| `/api/speakers/{id}` | DELETE | Turn off a specific microphone |

All responses are JSON-formatted with proper CORS headers for web application compatibility.

### Server Activity Log
- Real-time logging of all API requests
- Physical button press simulation logging
- Timestamped entries for easy debugging
- Auto-scrolling log viewer

## 🏗️ Architecture

The application is built using:
- **Qt 6.5+**: Cross-platform C++ framework
- **QML**: For the user interface
- **Qt Network**: TCP server for HTTP API
- **CMake**: Build system

### Key Components

1. **BoschServer** (`boschserver.h/cpp`)
   - Manages TCP server on port 8090
   - Handles HTTP requests and responses
   - Maintains state of all microphone seats
   - Implements conference logic (priority, toggling)

2. **Main Application** (`main.cpp`)
   - Initializes Qt application
   - Sets up QML engine
   - Bridges C++ backend with QML frontend

3. **QML Interface** (`Main.qml`, `MicComponent.qml`)
   - Visual representation of microphones
   - Interactive buttons for simulation
   - Real-time status updates
   - Server log display

## 🚀 Getting Started

### Prerequisites

- **Qt 6.5 or higher** with the following modules:
  - Qt Quick
  - Qt Network
  - Qt Quick Controls 2
- **CMake 3.16 or higher**
- **C++17 compatible compiler**
  - GCC 7+ / Clang 5+ / MSVC 2017+ / AppleClang (Xcode 10+)

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/PishangShedappp/BoschCSS1000dSimulator.git
   cd BoschCSS1000dSimulator
   ```

2. **Build with CMake**
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

3. **Run the simulator**
   ```bash
   # On Linux/macOS
   ./appBoschSimulator
   
   # On Windows
   .\appBoschSimulator.exe
   ```

### Quick Start

1. Launch the application - the server starts automatically on port 8090
2. Click "TALK" buttons on microphones to simulate physical button presses
3. Click "PRIO" on the chairman's microphone (Seat 1) to activate priority mode
4. Watch the server log to see API activity
5. Connect your application to `http://localhost:8090` to test API integration

## 📖 Usage

### Simulating Physical Interactions

**Toggle Microphone**: Click the "TALK" button on any microphone to turn it on/off
- When ON: The microphone head turns red
- When OFF: The microphone head is gray

**Chairman Priority**: Click the "PRIO" button on Seat 1 (Chairman)
- Turns ON the chairman's microphone
- Turns OFF all other microphones
- Useful for simulating meeting control scenarios

### API Integration

Connect your application to the simulator's HTTP server:

```javascript
// Example: Login
fetch('http://localhost:8090/api/login', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ username: 'admin', password: 'pass' })
})

// Example: Get active speakers
fetch('http://localhost:8090/api/speakers')
  .then(response => response.json())
  .then(data => console.log('Active speakers:', data))

// Example: Turn off a microphone
fetch('http://localhost:8090/api/speakers/5', {
  method: 'DELETE'
})
```

### Understanding the Interface

**Left Panel - Room View**:
- Central control unit at the bottom
- Grid of 10 microphone units above
- Color-coded status indicators

**Right Panel - Server Activity Log**:
- Timestamped entries of all events
- API requests and responses
- Physical button simulation events
- Auto-scrolls to show latest activity

## 🔧 Configuration

### Changing the Port

To change the default port (8090), modify `main.cpp`:

```cpp
// In main.cpp, line 16
server.startServer(8090);  // Change to your desired port
```

### Customizing Seat Count

To change the number of microphone seats, modify `boschserver.cpp`:

```cpp
// In BoschServer constructor
for(int i=1; i<=10; i++) {  // Change 10 to desired count
    // ...
}
```

## 🛠️ Development

### Project Structure

```
BoschCSS1000dSimulator/
├── CMakeLists.txt          # Build configuration
├── main.cpp                # Application entry point
├── boschserver.h           # Server class header
├── boschserver.cpp         # Server implementation
├── Main.qml                # Main UI layout
└── MicComponent.qml        # Microphone component UI
```

### Building for Development

```bash
# Configure with debug symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build
cmake --build . --config Debug

# Run
./appBoschSimulator
```

## 🤝 Contributing

Contributions are welcome! Whether it's:
- Bug reports
- Feature requests
- Code improvements
- Documentation enhancements

Please feel free to open an issue or submit a pull request.

## 📄 License

This project is provided as-is for educational and development purposes. Please ensure compliance with Bosch's terms of service when using this simulator.

## 🙏 Acknowledgments

- Built with Qt Framework
- Simulates Bosch Conference Control System CCS 1000 D API
- Created for developers and testers working with conference systems

## 📞 Support

If you encounter any issues or have questions:
1. Check the server activity log for error messages
2. Ensure port 8090 is not already in use
3. Verify Qt 6.5+ is properly installed
4. Open an issue on GitHub with details about your problem

---

**Note**: This is a simulator for development purposes. It does not replace actual Bosch hardware and may not implement all features of the real CCS 1000 D system." 

