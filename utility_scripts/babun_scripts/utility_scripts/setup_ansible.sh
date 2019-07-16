#source ./utility_scripts/setup_ansible.sh


cd /c/___CybermedSolutions/90CMS-SystemAdmin/Vagrant/babun_scripts
wget https://bootstrap.pypa.io/ez_setup.py -O - | python
easy_install pip

#needed to fix address space needed by '_speedups.dll' (0x3E0000) is already occupied when running ansible
git clone https://github.com/mitsuhiko/markupsafe.git
cd markupsafe
git checkout restore-speedups-feature
python setup.py --without-speedups install

#
cd ..
pip install ansible

#
pip install mercurial-keyring

#edit .hgrc with [auth] section

#
mkdir /vagrant
cp -rf /c/___CybermedSolutions/90CMS-SystemAdmin/Vagrant/#vagrant_scripts/* /vagrant/

#
pact install sudo

babun shell /bin/bash  
babun check   


