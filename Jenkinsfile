//
//	BurgerQuiz jenkins pipeline job
//
//	Depends on:
//		- no dependency
//
pipeline { 	
	agent { label 'master' }
	//Only keep the last 10 logs and the last artefact
	//Only keep the last logs and artefacts
	options {
		buildDiscarder(logRotator(numToKeepStr: '10', artifactNumToKeepStr: '1'))
		disableConcurrentBuilds()
	}	

	//Poll SCM every hour
    triggers { 
        cron('') 
        pollSCM('H * * * *')
    }	

	environment {
		projectName = "BurgerQuiz"    			
		jobDescription = """
## Burger Quiz

This project is dedicated to training only. None of the graphics are owned by tibSys or Tristan Israël. Commercial usage is forbidden.
"""	
	}

    stages {						
        stage('Build & analysis') {			
			steps {
				//Update the job description
				script { 
					def setDescription = { 
						def item = Jenkins.instance.getItemByFullName(env.JOB_NAME) 
						item.setDescription(jobDescription) 
						item.save()
					}
					setDescription()
				}


				//Build in Release
				echo "Starting build for Linux 64 bit with Qt 5.10 and GCC"
				
				echo "Calling qmake"
				sh 'qmake CONFIG+=release'

				echo "Cleaning"
				sh 'make clean'

				echo "Compiling"
				sh 'make'

				//stash includes: 'BurgerQuiz', name: 'bin'
				archiveArtifacts artifacts: 'BurgerQuiz'
			}
		}
		stage("Tests & coverage") {			
			steps {
				//Build in Debug with coverage
				echo "Starting build for Linux 64 bit with Qt 5.10 and GCC"				

				dir('tests') {
					echo "Calling qmake"
					sh 'qmake CONFIG+=debug CONFIG+=coverage'
					
					echo "Cleaning"
					sh 'make clean'

					echo "Compiling"
					sh 'make'

					echo "Execute unit tests"
					sh 'make check TESTARGS="-o testresults.xml,xml"'
					
					echo "Analyse test results"
					step([$class: 'XUnitBuilder',
						thresholds: [
							[$class: 'FailedThreshold', failureThreshold: '0']
						],
						tools: [
							[$class: 'QTestLibType', pattern: '**/testresults.xml']
						]
					])					
				}
			}
		}
		/*stage("Documentation") {
			steps {
				echo "Generate documentation"

				echo "Publish documentation"
				publishHTML target: [
					allowMissing: false,
					alwaysLinkToLastBuild: true,
					keepAll: false,
					reportDir: 'html',
					reportFiles: 'index.html',
					reportName: 'CppCheck Report'
				]
			}
		} 
		stage("Packaging") {

		}
		*/       
	}
	/*post {
	    //Nothing at the moment
    }*/
}