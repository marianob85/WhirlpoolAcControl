import subprocess
import shlex
import collections
import PulseDistanceCoding

# 10001000 00011110 11100010 01011000 11001101 11001010 00100000 00000101 1010
# Bits: 0-2 - Mode
#   0: 6th sense
#   1: Cool
#   2: Dry
#   3: Fan
#   4: Heat
# Bit 3: Power
# Bits: 4-6 - Fan
#    0: Auto

class WhirpoolYJ1B:
    def onCommand(self, data, rawData):
        # print(rawData)
        protocol = collections.OrderedDict()

        protocol['Mode'] = self.getMode( data[0] & 0b111 )
        protocol["Power"] = bool(rawData[3])
        protocol['Fan'] = self.getFan( data[0] >> 4 )
        protocol["Swing"] = 
        print("")
        for v,k in protocol.items():
            print ("{0}:{1}".format(v,k))

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

if __name__ == "__main__":
    process = subprocess.Popen(shlex.split("mode2"), stdout=subprocess.PIPE)

    pulseDistanceCoding = PulseDistanceCoding.PulseDistanceCoding()
    whirpoolYJ1B = WhirpoolYJ1B()
    pulseDistanceCoding.onCommand(whirpoolYJ1B.onCommand)
    while True:
        output = process.stdout.readline()
        if output == '' and process.poll() is not None:
            break
        if output:
            pulseDistanceCoding.analise( output.strip().decode('utf-8') )
    process.poll()