pipeline {
    agent any 
    stages {
        stage('Build') { 
            steps { 
                sh 'mkdir -p _build && cd _build && cmake .. && make -j4' 
            }
        }
        stage('Test'){
            steps {
                sh 'cd _build && ctest --output-on-failure'
            }
        }
    }
}
