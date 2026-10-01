import unittest
import sys
import struct
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from starbot_serial.star_protocol import Frame, Parser, crc16, decode_chassis_status
from starbot_serial.orientation import Orientation

class CodecTests(unittest.TestCase):
    def test_crc_and_splits(self):
        self.assertEqual(crc16(b'123456789'), 0x29B1)
        expected = Frame(0, 0x1234, 2)
        data = expected.encode()
        for split in range(len(data) + 1):
            parser = Parser()
            actual = parser.feed(data[:split], 0) + parser.feed(data[split:], 1)
            self.assertEqual(actual, [expected])

    def test_bad_crc_concat_and_noise(self):
        frame = Frame(0, 1, 4, b'\xa5\x5a')
        bad = bytearray(frame.encode())
        bad[-1] ^= 1
        parser = Parser()
        self.assertEqual(parser.feed(bytes(bad) + b'garbage' + frame.encode() * 3, 0), [frame] * 3)
        self.assertGreater(parser.rejected, 0)

    def test_bounds_and_wrap_timeout(self):
        parser = Parser()
        parser.feed(b'\xa5\x5a\x01', 0xFFFFFFF0)
        parser.expire(90)
        self.assertEqual(parser.buffer, b'')
        with self.assertRaises(ValueError):
            Frame(0, 1, 2, b'x' * 129).encode()
        frame = Frame(0, 2, 3, b'x' * 128)
        self.assertEqual(parser.feed(frame.encode(), 100), [frame])

    def test_signed_imu_voltage(self):
        data = b'\x00\x01' + struct.pack('<fff', -1, 0.5, -0.1) + b'\x04' + struct.pack('<fhhhhhh', 12.34, -123, 0, 16384, 1, -2, 3)
        value = decode_chassis_status(data)
        self.assertAlmostEqual(value[5], 12.34, places=4)
        self.assertEqual(value[6:], (-123, 0, 16384, 1, -2, 3))

    def test_fusion_zero_acc_and_independent_instances(self):
        a, b = Orientation(), Orientation()
        for _ in range(200):
            a.step((0, 0, 1), (0, 0, 0), 0.005)
        self.assertNotEqual(a.q, b.q)
        self.assertAlmostEqual(sum(v * v for v in a.q), 1)

if __name__ == '__main__':
    unittest.main()
