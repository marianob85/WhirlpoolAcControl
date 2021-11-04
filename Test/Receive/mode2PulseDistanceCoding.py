import subprocess
import shlex
import PulseDistanceCoding
import Whirlpool_YJ1B



if __name__ == "__main__":
    process = subprocess.Popen(shlex.split("mode2"), stdout=subprocess.PIPE)

    pulseDistanceCoding = PulseDistanceCoding.PulseDistanceCoding()
    whirpoolYJ1B = Whirlpool_YJ1B.WhirpoolYJ1B()
    pulseDistanceCoding.onCommand(whirpoolYJ1B.onCommand)
    while True:
        output = process.stdout.readline()
        if output == '' and process.poll() is not None:
            break
        if output:
            pulseDistanceCoding.analise( output.strip().decode('utf-8') )
    process.poll()