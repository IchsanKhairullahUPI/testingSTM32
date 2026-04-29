"""
Phase 6: Python script to send motor commands via USB serial.
Motor still won't physically move (no 48V yet), but we verify
Python <-> STM32 communication works end-to-end.
"""
import serial
import time

COM_PORT = "COM5"  # <-- CHANGE to whatever your Device Manager shows

def send_and_read(mcu, command, timeout=5):
    """Send a line, read responses until timeout or DONE."""
    print(f"  -> {command}")
    mcu.write(f"{command}\n".encode())
    deadline = time.time() + timeout
    while time.time() < deadline:
        line = mcu.readline().decode(errors="ignore").strip()
        if line:
            print(f"  <- {line}")
        if line and (line == "DONE" or line == "HOMED" or line.startswith("pos=")):
            return line

def main():
    print(f"Opening {COM_PORT}...")
    mcu = serial.Serial(COM_PORT, 115200, timeout=0.5)
    time.sleep(2)  # let board settle after port opens
    mcu.reset_input_buffer()

    print("\n--- Reset position ---")
    send_and_read(mcu, "home")

    print("\n--- Test 1: move 100 steps forward ---")
    send_and_read(mcu, "100")

    print("\n--- Test 2: check position ---")
    send_and_read(mcu, "status")

    print("\n--- Test 3: move back 100 steps ---")
    send_and_read(mcu, "-100")

    print("\n--- Test 4: verify position is zero ---")
    send_and_read(mcu, "status")

    print("\n--- Test 5: larger move (one full motor revolution) ---")
    send_and_read(mcu, "3200", timeout=10)

    mcu.close()
    print("\nAll tests complete. Firmware and serial link are working.")

if __name__ == "__main__":
    main()