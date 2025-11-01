# camdev flashing steps

1. Copy the .uf2 micropython file to pico 2. (It will show up as a file storage device). This brings the micropython env to the pico.

2. Copy the `mock_camera.py` using `mpremote`

https://docs.micropython.org/en/latest/reference/mpremote.html

# flashing from command line

From the main project folder:
`mpremote run scripts/mock_camera.py`

or `mpremote fs cp mock_camera.py :main.py`

