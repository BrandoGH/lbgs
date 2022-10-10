docker run --name $1 -p $2:$2 -p $3:$3 --net=host -v /home/liubin/docker_cmd/redis_cluster_conf/$4:/data/redis_cluster_conf/$4 -v /var/docker/redis/data:/data --tty=true -d  --privileged=true --restart=always redis redis-server /data/redis_cluster_conf/$4

#--net=host ,use debian IP, not docker ip
#need open 2 port ,once listen port 6379 ,and +10000 port 16379(cluster need)
