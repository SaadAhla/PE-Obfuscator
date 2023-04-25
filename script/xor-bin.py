import sys

b = bytearray(open(sys.argv[1], 'rb').read())
for i in range(len(b)):
    b[i] ^= 0x71
open('cipher.bin', 'wb').write(b)

