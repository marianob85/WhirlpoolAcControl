import re
import sys
import PulseDistanceCoding

if __name__ == "__main__":
    file_object = open("TesterPulseDistanceCoding.data", "r")
    pulseDistanceCoding = PulseDistanceCoding.PulseDistanceCoding()

    for line in file_object:
        pulseDistanceCoding.analise(line)

