curl https://raw.githubusercontent.com/ajenti/ajenti/1.x/scripts/install-rhel7.sh | sh
firewall-cmd --permanent --zone=public --add-port=8000/tcp
firewall-cmd --reload

nano /etc/ajenti/config.json

#
service ajenti stop; ajenti-panel -v


########
upstream ajentiweb {
    server 127.0.0.1:8000 weight=1 fail_timeout=300s;
}

server {
    listen        443;
    server_name ajenti.{{ansible_hostname}} ajenti.{{ansible_hostname}} {{ default_backend }}.ajenti.{{ansible_hostname}} ;
    
    client_max_body_size 200m;
    access_log    /var/log/nginx/ajenti-access.log ;
    error_log    /var/log/nginx/ajenti-error.log;
    ssl on;
    ssl_certificate        /etc/nginx/certs/domain.com/server.crt;
    ssl_certificate_key    /etc/nginx/certs/domain.com/server.key;
    keepalive_timeout    60;
    ssl_ciphers            HIGH:!ADH:!MD5;
    ssl_protocols            SSLv3 TLSv1;
    ssl_prefer_server_ciphers    on;
    proxy_buffers 16 64k;
    proxy_buffer_size 128k;
    location / {

        proxy_pass    http://ajentiweb;
        proxy_next_upstream error timeout invalid_header http_500 http_502 http_503;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forward-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto https;
        proxy_redirect off;
        proxy_read_timeout 5m;
    }
}

#################


##cwp
cd /usr/local/src
wget http://centos-webpanel.com/cwp-latest
sh cwp-latest


firewall-cmd --permanent --zone=public --add-port=2030/tcp
firewall-cmd --reload

http://localhost:2030

root
<pam>


