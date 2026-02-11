# Bosch CCS 1000 D Simulator - Technical Documentation

## Table of Contents
1. [Architecture Overview](#architecture-overview)
2. [API Specification](#api-specification)
3. [State Management](#state-management)
4. [UI Components](#ui-components)
5. [Network Protocol](#network-protocol)
6. [Extension Guide](#extension-guide)

---

## Architecture Overview

### System Components

The simulator consists of three main layers:

```
┌─────────────────────────────────────────┐
│         QML User Interface              │
│  (Main.qml, MicComponent.qml)          │
└────────────┬────────────────────────────┘
             │ Q_PROPERTY / Q_INVOKABLE
             │
┌────────────▼────────────────────────────┐
│       BoschServer (C++ Backend)         │
│  - State Management                     │
│  - Business Logic                       │
│  - TCP Server                           │
└────────────┬────────────────────────────┘
             │ TCP/IP (HTTP)
             │
┌────────────▼────────────────────────────┐
│     Client Applications                 │
│  (Web/Mobile Conference Apps)           │
└─────────────────────────────────────────┘
```

### Key Design Patterns

**Observer Pattern**: QML UI observes C++ backend through Qt's property binding system
- `serverLog` property emits `serverLogChanged()` signal
- `activeSeats` property emits `activeSeatsChanged()` signal

**Singleton Pattern**: Single BoschServer instance manages all conference state

**RESTful API**: Stateless HTTP endpoints for conference control

---

## API Specification

### Authentication

#### POST /api/login

**Request:**
```json
{
  "username": "string",
  "password": "string"
}
```

**Response (200 OK):**
```json
{
  "sid": "simulated-session-id-123",
  "id": 1
}
```

**Notes:**
- Simulated endpoint - always returns success
- Session ID is static for testing purposes
- In production, implement actual authentication

---

### Seat Management

#### GET /api/seats

Retrieves all conference seats/microphones.

**Response (200 OK):**
```json
[
  {
    "id": 1,
    "name": "Chairman",
    "connected": true,
    "prio": true
  },
  {
    "id": 2,
    "name": "Seat 2",
    "connected": true,
    "prio": false
  },
  ...
]
```

**Fields:**
- `id` (integer): Unique seat identifier (1-10)
- `name` (string): Display name for the seat
- `connected` (boolean): Whether the microphone is connected (always true in simulator)
- `prio` (boolean): Whether this seat has chairman/priority privileges

---

### Speaker Management

#### GET /api/speakers

Retrieves currently active speakers (microphones that are turned ON).

**Response (200 OK):**
```json
[
  {
    "id": 1,
    "name": "Chairman",
    "micOn": true
  },
  {
    "id": 5,
    "name": "Seat 5",
    "micOn": true
  }
]
```

**Notes:**
- Only returns seats with `micOn: true`
- Empty array if no microphones are active
- Updates in real-time when states change

---

#### DELETE /api/speakers/{id}

Turns off a specific microphone.

**URL Parameters:**
- `id` (integer): Seat ID to turn off (1-10)

**Response (200 OK):**
```json
{}
```

**Error Responses:**
- `400 Bad Request`: Invalid ID format
- `404 Not Found`: Seat ID does not exist

**Behavior:**
- Idempotent: calling multiple times has same effect
- Sets microphone state to OFF (does not toggle)
- Broadcasts state change to all connected clients

---

### CORS Support

#### OPTIONS (any endpoint)

Handles CORS preflight requests.

**Response (204 No Content)**

**Headers:**
```
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, POST, DELETE, OPTIONS
Access-Control-Allow-Headers: Content-Type, Bosch-Sid, Cookie
```

---

## State Management

### Seat Structure

Each seat maintains the following state:

```cpp
struct Seat {
    int id;              // Unique identifier (1-10)
    QString name;        // Display name
    bool isOn;           // Microphone active state
    bool isChairman;     // Has priority privileges
};
```

### State Transitions

**Normal Toggle (TALK button):**
```
OFF → ON  (if user clicks when OFF)
ON  → OFF (if user clicks when ON)
```

**Priority Mode (PRIO button - Chairman only):**
```
Chairman: OFF → ON
All Others: (any state) → OFF
```

**API Delete Request:**
```
(any state) → OFF
```

### Concurrency

- All state changes are thread-safe through Qt's event loop
- TCP connections are handled on separate threads
- QML property updates are automatically marshaled to UI thread

---

## UI Components

### Main.qml

**Layout:**
- Left Panel (75%): Room visualization
- Right Panel (25%): Server activity log

**Properties:**
- `activeList`: Bound to `boschServer.activeSeats` (QVariantList)

**Signals:**
- Updates automatically when `activeSeatsChanged()` is emitted

---

### MicComponent.qml

**Properties:**
- `seatId` (int): Unique identifier for this microphone
- `isActive` (bool): Whether microphone is currently ON
- `isChairman` (bool): Whether this is the chairman's seat

**Dimensions:**
- Standard: 80×120 pixels
- Chairman: 80×150 pixels (extra height for PRIO button)

**Visual States:**
- **Active (ON)**: Red microphone head (#ff3333)
- **Inactive (OFF)**: Gray microphone head (#444)

**Buttons:**
1. **TALK**: Available on all seats - toggles microphone
2. **PRIO**: Available only on chairman - activates priority mode

---

## Network Protocol

### HTTP Request Format

Standard HTTP/1.1 requests:

```
POST /api/login HTTP/1.1
Host: localhost:8090
Content-Type: application/json
Content-Length: 45

{"username":"admin","password":"password123"}
```

### HTTP Response Format

```
HTTP/1.1 200 OK
Content-Type: application/json
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, POST, DELETE, OPTIONS
Access-Control-Allow-Headers: Content-Type, Bosch-Sid, Cookie
Content-Length: 46

{"sid":"simulated-session-id-123","id":1}
```

### Request Processing Flow

```
1. Client connects via TCP socket
2. Server reads HTTP request headers + body
3. Request is parsed and routed to handler
4. Handler processes request and updates state
5. Response is formatted and sent
6. Socket remains open for keep-alive (if requested)
7. Signals emitted to update UI
```

---

## Extension Guide

### Adding New API Endpoints

1. **Add handler in `handleRequest()` method:**

```cpp
else if (header.contains("GET /api/myendpoint")) {
    // Your logic here
    QJsonObject response;
    response["data"] = "value";
    sendResponse(socket, "200 OK", QJsonDocument(response).toJson());
}
```

2. **Update CORS configuration** if needed in `sendResponse()`

3. **Add logging** using `log()` method for debugging

---

### Adding New Seat Properties

1. **Update Seat structure** in `boschserver.h`:

```cpp
struct Seat {
    int id;
    QString name;
    bool isOn;
    bool isChairman;
    int volume;  // New property
};
```

2. **Initialize in constructor** in `boschserver.cpp`:

```cpp
m_seats.append({i, name, false, isBoss, 50});  // Default volume 50
```

3. **Expose to API** in relevant endpoints:

```cpp
s["volume"] = seat.volume;
```

---

### Adding UI Features

1. **Add property to MicComponent.qml:**

```qml
property int volume: 50
```

2. **Bind to visual element:**

```qml
Text {
    text: "Vol: " + volume
    color: "white"
}
```

3. **Connect to C++ backend:**

```cpp
// In boschserver.h
Q_PROPERTY(int seatVolume READ seatVolume NOTIFY seatVolumeChanged)

// Expose getter
int seatVolume(int seatId) const;

signals:
    void seatVolumeChanged(int seatId);
```

---

### Modifying Conference Logic

**Example: Implementing "Request to Speak" Queue**

1. **Add queue structure:**

```cpp
QQueue<int> m_requestQueue;
```

2. **Add request method:**

```cpp
Q_INVOKABLE void requestToSpeak(int seatId) {
    if (!m_requestQueue.contains(seatId)) {
        m_requestQueue.enqueue(seatId);
        emit queueChanged();
    }
}
```

3. **Add approval method:**

```cpp
Q_INVOKABLE void approveNextSpeaker() {
    if (!m_requestQueue.isEmpty()) {
        int nextSpeaker = m_requestQueue.dequeue();
        toggleMic(nextSpeaker);
    }
}
```

---

## Testing

### Manual Testing Checklist

- [ ] Server starts successfully on port 8090
- [ ] All 10 microphones are visible
- [ ] TALK button toggles microphone state
- [ ] Chairman PRIO button works correctly
- [ ] API login endpoint responds
- [ ] API seats endpoint returns all seats
- [ ] API speakers endpoint returns only active mics
- [ ] API delete endpoint turns off microphones
- [ ] Server log shows all events
- [ ] UI updates when API calls are made

### Testing with cURL

```bash
# Test login
curl -X POST http://localhost:8090/api/login \
  -H "Content-Type: application/json" \
  -d '{"username":"test","password":"test"}'

# Get all seats
curl http://localhost:8090/api/seats

# Get active speakers
curl http://localhost:8090/api/speakers

# Turn off microphone 5
curl -X DELETE http://localhost:8090/api/speakers/5
```

---

## Troubleshooting

### Port Already in Use

**Error:** "Could not start server! Port might be in use."

**Solution:**
```bash
# Find process using port 8090
lsof -i :8090        # macOS/Linux
netstat -ano | findstr :8090  # Windows

# Kill the process or change the port in main.cpp
```

### Qt Module Not Found

**Error:** "Could not find Qt6Quick" or similar

**Solution:**
```bash
# Ensure Qt 6.5+ is installed
# Set Qt path
export Qt6_DIR=/path/to/Qt/6.5.0/gcc_64/lib/cmake/Qt6

# Reconfigure
cmake -DQt6_DIR=$Qt6_DIR ..
```

### UI Not Updating

**Issue:** Clicking buttons doesn't update the UI

**Check:**
1. Signals are properly emitted in C++ code
2. Properties are correctly bound in QML
3. Console shows any QML warnings/errors

---

## Performance Considerations

### Scalability

Current implementation supports up to 10 seats. To scale:

1. **Increase seat count**: Modify loop in `BoschServer` constructor
2. **Optimize UI**: Use QML Loader for large seat counts
3. **Database integration**: Store state in SQLite for persistence

### Memory Usage

- Typical memory footprint: ~50-80 MB
- Each TCP connection: ~4-8 KB
- QML component instances: ~2-4 KB each

### Network Performance

- HTTP response time: <5ms (localhost)
- Concurrent connections: Limited by OS (typically 1000+)
- Throughput: Depends on network interface

---

## Future Enhancements

Potential features to add:

1. **Audio simulation**: Play sound when microphone is active
2. **Recording**: Save meeting transcripts
3. **Multiple rooms**: Simulate multiple conference rooms
4. **WebSocket support**: Real-time bidirectional communication
5. **Configuration file**: Load seat layouts from JSON
6. **Replay mode**: Record and replay conference sessions
7. **Statistics**: Track speaking time per participant
8. **Integration with actual hardware**: Bridge to real Bosch systems

---

## Glossary

- **CCS**: Conference Control System
- **Seat**: A participant position with associated microphone
- **Chairman**: Seat with priority/override capabilities
- **Active Seat**: Microphone currently turned ON
- **Priority Mode**: State where only chairman microphone is active
- **CORS**: Cross-Origin Resource Sharing (for web app access)

---

*This documentation is maintained alongside the project. For the latest version, please refer to the GitHub repository.*
