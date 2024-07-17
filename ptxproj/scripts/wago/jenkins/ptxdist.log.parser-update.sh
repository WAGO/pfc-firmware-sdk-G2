# The project ptxdist.log.parser is developed within a different repository.
# In order to avoid using git submodules and to avoid having files that are not necessary for productive use, the scripts are copied manually.
set -ex
TAG=v1.0.0
URL=git@svgithub01001.wago.local:BU-Automation/jenkins-ptxdist.log.parser.git
DIR=ptxdist.log.parser

rm -rf ${DIR}
#git clone -b ${TAG} ${URL} --depth 1 ${DIR} --separate-git-dir=$(mktemp -u) -c advice.detachedHead=false
git clone -b ${TAG} ${URL} --depth 1 ${DIR} -c advice.detachedHead=false
(cd ${DIR} && git describe --tags)
rm -rf ${DIR}/{.git,.gitignore,.venv,.vscode,Jenkinsfile,test}

