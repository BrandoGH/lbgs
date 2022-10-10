docker run --name docker_mysql -p 3306:3306 -e MYSQL_ROOT_PASSWORD=123456 -v /var/docker/mysql/datadb:/data/db --tty=true  -d --privileged=true --restart=always mysql
