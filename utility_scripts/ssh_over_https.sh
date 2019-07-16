
# To test if SSH over the HTTPS port is possible, run this SSH command:

ssh -T -p 443 git@gitlab.com
# Hi username! You've successfully authenticated, but gitlab does not
# provide shell access.


# If that worked, great! If not, you may need to follow our troubleshooting guide.
# Enabling SSH connections over HTTPS

nano ~/.ssh/config

Host gitlab.com
  Hostname gitlab.com
  Port 443

#  
ssh -T -p 443 git@github.com  
nano ~/.ssh/config

Host github.com
  Hostname ssh.github.com
  Port 443
  
# To make sure you are connecting to the right domain, you can enter the following command:
ssh -vT git@github.com  

