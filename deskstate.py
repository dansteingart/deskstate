import os
os.environ['DYLD_LIBRARY_PATH'] = '/opt/homebrew/lib:$DYLD_LIBRARY_PATH'

from flask import Flask
import usb.core
import usb.util
import json

app = Flask(__name__)

def color(r,g,b,brightness=255):
    VID = 0x5150  
    PID = 0x0812  

    # Find the USB device
    device = usb.core.find(idVendor=VID, idProduct=PID)
    if device is None:
        raise ValueError("Device not found. Ensure the CP is connected and powered on.")

    device.set_configuration()
    cfg = device.get_active_configuration()
    interface = cfg[(2, 0)] #this works for some reason

    # Find OUT and IN endpoints
    out_endpoint = None
    in_endpoint = None

    for ep in interface:
        if usb.util.endpoint_direction(ep.bEndpointAddress) == usb.util.ENDPOINT_OUT:
            out_endpoint = ep
        elif usb.util.endpoint_direction(ep.bEndpointAddress) == usb.util.ENDPOINT_IN:
            in_endpoint = ep

    if not out_endpoint: raise ValueError("OUT endpoint not found")

    # Function to send color change commands
    def change_led_color(led, r, g, b,brightness):
        # Create JSON command
        command = {
            "led": led,
            "brightness":brightness,
            "color": {
                "r": r,
                "g": g,
                "b": b
            }
        }
        # Convert JSON to bytes
        data = json.dumps(command)+"\n"
        data=data.encode('utf-8')
        try:
            # Write the command to the OUT endpoint
            out_endpoint.write(data)
        except usb.core.USBError as e:
            print(f"Error sending data: {e}")

    for i in range(10): change_led_color(i, r, g, b,brightness)
    usb.util.dispose_resources(device)

# Define a route
@app.route('/')
def home():
    return open("deskstate.html").read()


@app.route('/red')
def red():
    color(255,0,0)
    return "should be red"


@app.route('/set/<int:r>/<int:g>/<int:b>/<int:brightness>')
def set(r, g, b, brightness):
    try: color(r,g,b,brightness)
    except Exception as E: print(E)
    return f"{r},{g},{b},{brightness}"


@app.route('/blue')
def blue():
    color(0,0,255)
    return "should be blue"


# Main section to run the server
if __name__ == "__main__": app.run(host='0.0.0.0', port=7100, debug=True)