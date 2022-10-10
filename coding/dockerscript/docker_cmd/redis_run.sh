docker run --name $1 -p $2:$2 -v /home/liubin/docker_cmd/redis_conf/$3:/data/redis_conf/redis.conf -v /var/docker/redis/data:/data --tty=true -d  --privileged=true --restart=always redis redis-server /data/redis_conf/redis.conf --appendonly yes

#docker run --name docker_redis -p 6379:6379 -v /home/liubin/docker_cmd/redis_conf/redis.conf:/data/redis_conf/redis.conf -v /var/docker/redis/data:/data --tty=true -d  --privileged=true --restart=always redis redis-server /data/redis_conf/redis.conf --appendonly yes
