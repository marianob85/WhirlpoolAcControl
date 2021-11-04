properties(
	[
		buildDiscarder(logRotator(artifactDaysToKeepStr: '', artifactNumToKeepStr: '', daysToKeepStr: '', numToKeepStr: '10')),
		pipelineTriggers([pollSCM('0 H(5-6) * * 5')])
	]
)
pipeline
{
	agent none
		options {
		skipDefaultCheckout true
	}
	stages {
		stage('Build'){
			agent{ label "linux/u18.04/arduino" }
			steps {
				dir("WhirlpoolAcControl") {
					checkout scm

					sh '''
						arduino-cli core update-index
						#arduino-cli lib install "OneWire"
						#arduino-cli lib install "DallasTemperature"
						arduino-cli compile --fqbn arduino:avr:uno WhirlpoolAcControl
					'''
				}
			}
		}

	}
}
