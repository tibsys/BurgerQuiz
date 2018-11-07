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
				
				echo Calling qmake
				sh 'qmake CONFIG+=release'

				echo Compiling
				sh 'qmake'
			}
		}
		/*stage("Tests & coverage") {
			//Binaries are created at the end of this stage
			//Binaries are: executable binary + GNU/Linux bundle (zipped)	
			steps {

			}
		}
		stage("Documentation") {
			steps {

			}
		} */       
	}
	/*post {
	    //Nothing at the moment
    }*/
}