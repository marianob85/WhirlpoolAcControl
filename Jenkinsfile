properties(
	[
		buildDiscarder(logRotator(artifactDaysToKeepStr: '', artifactNumToKeepStr: '', daysToKeepStr: '', numToKeepStr: '10')),
		pipelineTriggers([pollSCM('0 H(5-6) * * *')])
	]
)

pipeline
{
	agent any
	options {
		skipDefaultCheckout true
	}
	stages
	{
		stage('Build'){
			agent{ label "linux/u18.04/platformio:6.1.4" }
			steps {
				checkout scm
				sh '''
					pio run
				'''
				stash includes: '**/firmware*', name: "bin"
			}
		}
		stage('Archive'){
			steps {
				unstash "bin"
				archiveArtifacts artifacts: '**/firmware*', fingerprint: true, onlyIfSuccessful: true
			}
		}
	}
	post { 
        changed { 
            emailext body: 'Please go to ${env.BUILD_URL}', to: '${DEFAULT_RECIPIENTS}', subject: "Job ${env.JOB_NAME} (${env.BUILD_NUMBER}) ${currentBuild.currentResult}".replaceAll("%2F", "/")
        }
    }
}