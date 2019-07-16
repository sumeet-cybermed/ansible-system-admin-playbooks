
#################################################
# setup ssh keys 
###add id_rsa_sumeetlightlove private key
mkdir -p /root/.ssh

#sumeet.light.love pub key
echo "ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQC08mLyi9KMDOa4I6VxoBII7OBUyDta0B6jS0GxykMtZPHf/4BjNKSp7bC7kWKO/AKcj+YDsZNvozT+qn3/5ZRvd0hNCK7GHM0dmOmo61eS9BHckdcSZHRx0/vjBmDQC8xlE9O371PrDkMBFAze5r0GNZG4FXHvPohEEaxdnknjIaGvLCSZ8+RHobDvuap1ICkuXb+GJ+x7wnSqzlJnTxTsWdCq4sogwCb86F8fN8Xs96tfzIBUpSv0/qppynZAm4sQKK3wIFP4TU8rRDawY8yRjV0JGGd2mkkJS5m0+xnkagKHtW7tCH1JE8TZRa56cDyoUV9dAkAxlYomnk3j4mJD sumeet.light.love@gmail.com" > /root/.ssh/id_rsa_sumeetlightlove.pub;

#sumeet.light.love pvt key

echo "
-----BEGIN RSA PRIVATE KEY-----

-----END RSA PRIVATE KEY-----
" > /root/.ssh/id_rsa_sumeetlightlove;

echo "
/root/.ssh/id_rsa_sumeetlightlove
"

cp -f /root/.ssh/id_rsa_sumeetlightlove /root/.ssh/id_rsa
cp -f /root/.ssh/id_rsa_sumeetlightlove.pub /root/.ssh/id_rsa.pub

cat /root/.ssh/id_rsa_sumeetlightlove;
cat /root/.ssh/id_rsa;


#################################################
#setup authorized keys for self
touch /root/.ssh/authorized_keys;

if [ ! -n "$(grep "sumeet@cybermedsolutions.com" /root/.ssh/authorized_keys)" ]; then  echo "ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQDJYai3OTGejma31BbWEir8D7MYj2bSwHUyPMl70WcO0+a/yWayrRV51DEeBLaesXmP51W1A/3XWPOCtBwpmSgWsnPEa1iSZybhDVcH1TjiJst7fut9f2jLon2UGrLpqesTfJooh5Ckl5Yg00+KIV0YzBBnJYyXbvPdc6xuU0Ma3hkrQXMZOaqHvhwROjvp2YAGn198bYEhbTRz5wdweB30r1oVPo3PU+wGok91ZsFK7mcONh5WaKKAoUzRfXDPUExeVHjoglnPT6A7Y59q48kSHFnDPP09FjUZ2cMFAmv53MVA8Zxa8u2j4xgTxCWVgW5kL5OlxvEQlF31TXnXiagn sumeet@cybermedsolutions.com" >> /root/.ssh/authorized_keys 2>/dev/null; fi;

if [ ! -n "$(grep "sumeet.light.love@gmail.com" /root/.ssh/authorized_keys)" ]; then cat /root/.ssh/id_rsa_sumeetlightlove.pub >> /root/.ssh/authorized_keys 2>/dev/null; fi;


#################################################

chown root:root /root/.ssh/*
chmod 0600 /root/.ssh/*

echo "
ls /root/.ssh/
"
ls /root/.ssh/*

echo "
/root/.ssh/authorized_keys
"
cat  /root/.ssh/authorized_keys;