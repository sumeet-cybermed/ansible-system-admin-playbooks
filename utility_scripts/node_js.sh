# docker >=1.13
sudo yum -y remove docker docker-common container-selinux
sudo yum-config-manager \
    --add-repo \
    https://docs.docker.com/v1.13/engine/installation/linux/repo_files/centos/docker.repo

yum makecache fast
sudo yum -y install docker-engine

# node
yum install -y gcc-c++ make
curl -sL https://rpm.nodesource.com/setup_6.x | sudo -E bash -
yum install -y nodejs
cd /tmp
mkdir -p nodejs_test
cd nodejs_test
echo "var http = require('http');
http.createServer(function (req, res) {
  res.writeHead(200, {'Content-Type': 'text/plain'});
  res.end('Welcome');
}).listen(3001, '127.0.0.1');
console.log('Server running at http://127.0.0.1:3001/');" >  ./test_server.js
node --debug test_server.js &


###yarn
sudo wget https://dl.yarnpkg.com/rpm/yarn.repo -O /etc/yum.repos.d/yarn.repo

sudo yum install -y yarn

npm install -g react-native-cli

#brew
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"


# graphcool
npm install -g graphcool

# docker compose
pip install requests==2.5.3 && pip install docker-compose

#set updev dir & initial project
mkdir -p /home/dev/Sympatica/_misc/graphcool
cd /home/dev/Sympatica/Dev/_misc/graphcool

# create services project
graphcool init sympatica-poc-neo
cd sympatica-poc-neo

# add auth
graphcool add-template auth/facebook

# start local cluster
graphcool local up

# deploy docker app
graphcool deploy


##sample projects
cd /home/dev/Sympatica/Dev/graphcool
git clone https://github.com/react-native-training/apollo-subscriptions-book-club

graphcool init --schema ./schema.graphql --name BookClub
graphcool deploy


#note project ID
nano app/index.js



##jdk
cd /opt/
wget --no-cookies --no-check-certificate --header "Cookie: gpw_e24=http%3A%2F%2Fwww.oracle.com%2F; oraclelicense=accept-securebackup-cookie" "http://download.oracle.com/otn-pub/java/jdk/8u161-b12/2f38c3b165be4555a1fa6e98c45e0808/jdk-8u161-linux-x64.tar.gz"
tar xzf jdk-8u161-linux-x64.tar.gz

cd /opt/jdk1.8.0_161/
alternatives --install /usr/bin/java java /opt/jdk1.8.0_161/bin/java 2
alternatives --config java

alternatives --install /usr/bin/jar jar /opt/jdk1.8.0_161/bin/jar 2
alternatives --install /usr/bin/javac javac /opt/jdk1.8.0_161/bin/javac 2
alternatives --set jar /opt/jdk1.8.0_161/bin/jar
alternatives --set javac /opt/jdk1.8.0_161/bin/javac
java -version

echo "
export JAVA_HOME=/opt/jdk1.8.0_161
export JRE_HOME=/opt/jdk1.8.0_161/jre
export PATH=$PATH:/opt/jdk1.8.0_161/bin:/opt/jdk1.8.0_161/jre/bin" > /etc/environment

#android sdk - from GUI
mdir -p /opt
cd /opt

cp /mnt/shares/__Soft/_Development/__Linux/Android\ SDK/android-studio-ide-171.4443003-linux.zip /opt
cd /opt

unzip android-studio-ide-171.4443003-linux.zip

cd /opt
cd android-studio/bin
./studio.sh

#clear yum cache
yum clean all
rm -rf /var/cache/yum/


# del archives
rm -f /opt/*.zip
rm -f /opt/*.gz
rm -f /opt/*.tar

# del archives
rm -f /tmp/*.zip
rm -f /tmp/*.gz
rm -f /tmp/*.tar


# google chrome
cd /tmp
wget https://dl.google.com/linux/linux_signing_key.pub
rpm --import linux_signing_key.pub
yum update
yum makecache fast

yum install google-chrome-stable




