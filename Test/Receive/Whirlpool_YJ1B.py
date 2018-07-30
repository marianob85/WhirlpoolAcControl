import collections

# Example: 10001000 00011110 11100010 01011000 11001101 11001010 00100000 00000101 1010
# Byte 0:
#   Bit: 5-7 - Mode
#     0: 6th sense
#     1: Cool
#     2: Dry
#     3: Fan
#     4: Heat
#   Bit: 4: Power
#   Bit: 2-3 - Fan
#      0: Auto 1:1 2:2 3:3
#   Bit: 1 - Swing
#   Bit: 0 - Sleep
# Byte 1:
#   Bit: 0-3 - Temperature 0: 16deg, 1: 17deg  etc..
#   Bit: 4-7 - 4 second interval - 60sec/15
# Byte 2:
#   Bit: 0-5 - Minutes
#   Bit: 6 - AM/PM
#   Bit: 7 - Unknown
# Byte 3:
#   Bit: 0-3 - Hours
#   Bit: 4-7 - Unknown

# Byte 6:
#   Bit: 4 - Jet
#   Bit: 5 - Light


class WhirpoolYJ1B:
    def onCommand(self, data, rawData):
        # print(rawData)
        protocol = collections.OrderedDict()

        protocol['Mode'] = self.getMode( data[0] & 0b111 )
        protocol["Power"] = bool(rawData[3])
        protocol['Fan'] = self.getFan( ( data[0] >> 4 ) & 0b11 )
        protocol["Swing"] = bool(rawData[6])
        protocol["Sleep"] = bool(rawData[7])
        protocol["Temperature"] = ( data[1] & 0b1111 ) + 16
        protocol["4SecondsInterval"] = ( ( data[1] >> 4 ) & 0b1111 ) *  4
        protocol["Clock: Minutes"] = (data[2] & 0b111111 )
        protocol["Clock: Hours"] = (data[3] & 0b1111)
        protocol["Clock:"] = self.getAMPM( (data[3] >> 6 ) & 0b1)
        protocol["Jet"] = bool( ( data[6] >> 4 ) & 0b1 )
        protocol["Light"] = bool((data[6] >> 5) & 0b1)
        print("")
        for v,k in protocol.items():
            print ("{0}:{1}".format(v,k))

    def getAMPM(self,data):
        switcher={
            False:"AM",
            True:"PM",
        }
        return switcher.get(data, data)

    def getFan(self, data):
        switcher={
            0:"Auto",
        }
        return switcher.get(data, data)

    def getMode(self, data):
        switcher={
            0:"6th Sense",
            1:"Cool",
            2:"Dry",
            3:"Fan",
            4:"Heat"
        }
        return switcher.get(data, data)
