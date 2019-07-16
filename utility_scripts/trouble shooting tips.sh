#1 /bin/git ls-remote  fails behind firewall
/bin/git ls-remote git://github.com/preinheimer/xhprof.git -h refs/heads/HEAD

stderr: fatal: unable to connect to github.com:
github.com[0: 192.30.252.128]: errno=Connection refused

#resolutions
git config --global url.https://github.com/.insteadOf git://github.com/

###########
# Test if cron is working

# Add the following entry to your crontab. To do so, enter the command

crontab -e

# And then add:

* * * * * /bin/echo "foobar" >> /your-path/your-file-name.txt 

# After a minute, a file should be created under the specified path, holding the "foobar" content. If you can see the file, congrats, cron is working fine.
# Test if your command is working

# Add the following to your crontab:

* * * * * /bin/foobar > /your-path/your-file-name.txt 2>&1

and check for errors in look for errors in your-file-name.txt
# Test for cron errors - the cron log

# You can always check the cron log for errors. The usual location for that would be:

/var/log/cron

or

/var/log/messages

############
# MAILTO is not working for CRON. How can I fix this?
yum install mailx

# Install mailx 
    
# Install sendmail ##not if another smtp server is installed
yum install sendmail
# Start sendmail
# Put the MAILTO line into the crontab using crontab -e : MAILTO=my@email.com and not in /etc/crontab

