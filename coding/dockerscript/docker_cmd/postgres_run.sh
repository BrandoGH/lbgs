docker run --name docker_postgres -p 5432:5432 --net=host -v /var/docker/postgres/datadb:/data/db -e POSTGRES_PASSWORD=123456  --tty=true -d --privileged=true --restart=always postgres
