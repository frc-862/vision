import socket

from SimpleCV import Camera, Image
import cv2
import numpy
import sys
import os


# Really hacky way to detect if the camera is connected. Always returns true on Windows and OSX.
# TODO maybe this doesn't have to return true always on OSX, but I doubt we'll ever have an OSX machine running this.
def camera_connected():
    if sys.platform == 'linux' or sys.platform == 'linux2':
        return os.path.exists('/dev/video0')  # The system sees a camera
    else:
        return True


port = 8621
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(('localhost', port))
server_socket.listen(1)  # max 1 connection - we only have one camera, so we can't send images to two people at once
print('Listening on port %s' % port)

c = Camera()

lastimg = None

while True:
    connection, address = server_socket.accept()
    data = connection.recv(1024)
    print('Got some data:')
    print(data)
    print("Data over.")
    if data == 'GETIMG':  # They want a raw image
        try:
            if camera_connected():
                # give them the current camera image
                img = c.getImage()
            else:  # Looks like the camera disconnected randomly
                img = Image("res/img/connect_failed.png")
        except AttributeError:  # Occurs when the camera was never connected in the first place
            img = Image("res/img/connect_failed.png")

        # Encode as JPEG so we don't have to send so much
        encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), 90]
        result, encoded_img = cv2.imencode('.jpg', img.getNumpy(), encode_param)

        img_numpy = numpy.array(encoded_img)
        img_str = img_numpy.tostring()

        # Send the size of the image, so that the client can handle if we have to send it in multiple parts
        connection.send(str(len(img_str)).ljust(16))
        # Send the image itself
        connection.send(img_str)
        lastimg = img_numpy