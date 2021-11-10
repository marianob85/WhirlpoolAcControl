import re
import sys
import datetime

from itertools import islice

def chunk(it, size):
    it = iter(it)
    return iter(lambda: tuple(islice(it, size)), ())

class PulseDistanceCoding:
    def __init__(self):
        self.commandTimeOut = 11000
        self.headerOffset = 2000
        self.count = 0
        self.pulse = 0
        self.receiveData = []
        self.events = []
        self.commandLength = None

    def onCommand(self, clb):
        self.events.append(clb)

    def translateToByte(self):
        bytes = []
        datas = (list(chunk( self.receiveData, 8)))
        for wordData in datas:
            word = 0
            index = 0
            for bitInWord in  wordData:
                #print(bitInWord)
                if bitInWord:
                    word |= 1 << index
                index += 1
            bytes.append(word)
        return bytes

    def sendEvent(self):
        for clb in self.events:
            clb(self.translateToByte(),  self.receiveData)

    def invalidate(self, newLine):
        self.prevData = None
        self.count = 0
        self.receiveData = []
        if newLine:
            sys.stdout.write('\n')
            sys.stdout.flush()

    def isHeader(self, data ):
        return abs( data[0] - data[1] ) > self.headerOffset

    def getBit(self, data):
        return data[0] * 2 < data[1]

    def analise(self, line):
        spaceData = re.match(r'space (\d+)', line)
        pulseData = re.match(r'pulse (\d+)', line)
        timeout = re.match(r'timeout', line)
        if spaceData:
            space = int(spaceData.group(1))
        elif pulseData:
            self.pulse = int(pulseData.group(1))
            return
        elif timeout:
            self.sendEvent()
            self.invalidate(True)
            return
        else:
            self.invalidate(False)
            return

        if space > self.commandTimeOut:
            if self.commandLength == None and  self.count > 0:
                self.commandLength = self.count
            self.invalidate(True)
            return

        if self.isHeader( [self.pulse,space] ):
            return

        if self.count == 0:
            print(datetime.datetime.now())

        if self.getBit([self.pulse,space]):
            self.receiveData.append(1)
            sys.stdout.write("1")
        else:
            self.receiveData.append(0)
            sys.stdout.write('0')

        self.count += 1

        if self.count % 8 == 0:
            sys.stdout.write(' ')
        sys.stdout.flush()

        if self.count == self.commandLength:
            self.sendEvent()
            self.invalidate(True)


