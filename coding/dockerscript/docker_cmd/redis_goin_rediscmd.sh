docker exec -it $1 redis-cli -p $2 --pass 123456 $3

#$3: other option ,clulster = -c
#docker exec -it [name] redis-cli -p 6381 --pass 123456
